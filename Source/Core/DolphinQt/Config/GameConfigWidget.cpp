// Copyright 2018 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/GameConfigWidget.h"

#include <array>
#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>
#include <QStyle>
#include <QTabWidget>
#include <QTimer>

#include "Common/CommonPaths.h"
#include "Common/Config/Config.h"
#include "Common/Config/Layer.h"
#include "Common/FileUtil.h"

#include "Core/Config/GraphicsSettings.h"
#include "Core/Config/MainSettings.h"
#include "Core/ConfigLoaders/GameConfigLoader.h"
#include "Core/ConfigManager.h"
#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"
#include "DolphinQt/Config/GameConfigEdit.h"
#include "DolphinQt/Config/Graphics/GraphicsPane.h"
#include "DolphinQt/Settings.h"

#include "UICommon/GameFile.h"

#include "ui_GameConfigWidget.h"

namespace
{
void PopulateTab(QTabWidget* tab, const std::string& path, const std::string& game_id, u16 revision,
                 bool read_only)
{
  for (const std::string& filename : ConfigLoaders::GetGameIniFilenames(game_id, revision))
  {
    const std::string ini_path = path + filename;
    if (File::Exists(ini_path))
    {
      auto* const edit = new GameConfigEdit(tab, QString::fromStdString(ini_path), read_only);
      tab->addTab(edit, QString::fromStdString(filename));
    }
  }
}
}  // namespace

GameConfigWidget::GameConfigWidget(const UICommon::GameFile& game)
    : m_ui{std::make_unique<Ui::GameConfigWidget>()}, m_game(game), m_game_id(m_game.GetGameID())
{
  m_gameini_local_path =
      QString::fromStdString(File::GetUserPath(D_GAMESETTINGS_IDX) + m_game_id + ".ini");

  m_layer = std::make_unique<Config::Layer>(
      ConfigLoaders::GenerateLocalGameConfigLoader(m_game_id, m_game.GetRevision()));
  m_global_layer = std::make_unique<Config::Layer>(
      ConfigLoaders::GenerateGlobalGameConfigLoader(m_game_id, m_game.GetRevision()));

  m_ui->setupUi(this);
  m_ui->helpIconLabel->setPixmap(
      style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(QSize{20, 20}));

  BindSettings();
  m_ui->graphicsLayout->addWidget(new GraphicsPane{nullptr, m_layer.get()});
  ConnectWidgets();
  AddDescriptions();
  PopulateEditorTabs();
  LoadSettings();

  connect(&Settings::Instance(), &Settings::ConfigChanged, this, &GameConfigWidget::LoadSettings);

  // Some platform styles replace widget fonts while polishing the page. Apply the game-default
  // marker after that polish and then let every binding re-apply its local-override state.
  QTimer::singleShot(100, this, [this] {
    SetItalics();
    Config::OnConfigChanged();
  });
}

GameConfigWidget::~GameConfigWidget()
{
  // Destructor saves the layer to file.
  m_layer.reset();

  // If a game is running and the game properties window is closed, update local game layer with
  // any new changes. Not sure if doing it more frequently is safe.
  auto local_layer = Config::GetLayer(Config::LayerType::LocalGame);
  if (local_layer && SConfig::GetInstance().GetGameID() == m_game_id)
  {
    local_layer->DeleteAllKeys();
    local_layer->Load();
    Config::OnConfigChanged();
  }

  // Delete empty configs
  if (File::GetSize(m_gameini_local_path.toStdString()) == 0)
    File::Delete(m_gameini_local_path.toStdString());
}

void GameConfigWidget::BindSettings()
{
  Config::Layer* const layer = m_layer.get();
  ConfigWidget::Bind(m_ui->enableDualCoreCheckBox, Config::MAIN_CPU_THREAD, layer);
  ConfigWidget::Bind(m_ui->enableMmuCheckBox, Config::MAIN_MMU, layer);
  ConfigWidget::Bind(m_ui->enableFprfCheckBox, Config::MAIN_FPRF, layer);
  ConfigWidget::Bind(m_ui->syncGpuCheckBox, Config::MAIN_SYNC_GPU, layer);
  ConfigWidget::Bind(m_ui->emulateDiscSpeedCheckBox, Config::MAIN_FAST_DISC_SPEED, layer, true);
  ConfigWidget::Bind(m_ui->dspHleCheckBox, Config::MAIN_DSP_HLE, layer);

  const std::array<std::string, 3> deterministic_choices{
      tr("auto").toStdString(),
      tr("none").toStdString(),
      tr("fake-completion").toStdString(),
  };
  ConfigWidget::BindStringChoice(m_ui->deterministicDualCoreComboBox,
                                 Config::MAIN_GPU_DETERMINISM_MODE, deterministic_choices, layer);

  const auto depth = ConfigWidget::BindFloat(m_ui->depthSlider, Config::GFX_STEREO_DEPTH_PERCENTAGE,
                                             100.0f, 200.0f, 1.0f, layer);
  const auto convergence = ConfigWidget::BindFloat(
      m_ui->convergenceSlider, Config::GFX_STEREO_CONVERGENCE, 0.0f, 1000.0f, 0.01f, layer);
  ConfigWidget::Bind(m_ui->monoscopicShadowsCheckBox, Config::GFX_STEREO_EFB_MONO_DEPTH, layer);

  ConfigWidget::MirrorFont(m_ui->deterministicDualCoreLabel, m_ui->deterministicDualCoreComboBox);
  ConfigWidget::MirrorFont(m_ui->depthLabel, m_ui->depthSlider);
  ConfigWidget::MirrorFont(m_ui->convergenceLabel, m_ui->convergenceSlider);
  ConfigWidget::MirrorFloatValue(m_ui->depthValueLabel, depth, QStringLiteral("%.0f%%"));
  ConfigWidget::MirrorFloatValue(m_ui->convergenceValueLabel, convergence, QStringLiteral("%.2f"));
}

void GameConfigWidget::ConnectWidgets()
{
  const int editor_index = m_ui->tabWidget->indexOf(m_ui->editorTab);
  connect(m_ui->tabWidget, &QTabWidget::currentChanged, this, [this, editor_index](int index) {
    // Update the INI editor after editing other tabs.
    if (index == editor_index)
    {
      // Layer only auto-saves when it is destroyed.
      m_layer->Save();
      RefreshLocalEditor();
    }

    // Update other tabs after using the INI editor.
    if (m_prev_tab_index == editor_index)
    {
      // Load won't clear deleted keys, so everything is wiped before loading.
      m_layer->DeleteAllKeys();
      m_layer->Load();
      Config::OnConfigChanged();
    }

    m_prev_tab_index = index;
  });

  connect(m_ui->depthSlider, &QSlider::valueChanged, this, &GameConfigWidget::UpdateSliderLabels);
  connect(m_ui->convergenceSlider, &QSlider::valueChanged, this,
          &GameConfigWidget::UpdateSliderLabels);
}

void GameConfigWidget::AddDescriptions()
{
  ConfigWidget::SetDescription(
      m_ui->enableMmuCheckBox, QString{},
      tr("Enables the Memory Management Unit, needed for some games. (ON = Compatible, OFF = "
         "Fast)"));
  ConfigWidget::SetDescription(
      m_ui->enableFprfCheckBox, QString{},
      tr("Enables Floating Point Result Flag calculation, needed for a few "
         "games. (ON = Compatible, OFF = Fast)"));
  ConfigWidget::SetDescription(
      m_ui->syncGpuCheckBox, QString{},
      tr("Synchronizes the GPU and CPU threads to help prevent random freezes "
         "in Dual core mode. (ON = Compatible, OFF = Fast)"));
  ConfigWidget::SetDescription(m_ui->emulateDiscSpeedCheckBox, QString{},
                               tr("Enable emulated disc speed. Disabling this can cause crashes "
                                  "and other problems in some games. "
                                  "(ON = Compatible, OFF = Unlocked)"));
  ConfigWidget::SetDescription(
      m_ui->depthSlider, tr("Depth Percentage:"),
      tr("This value is multiplied with the depth set in the graphics configuration."));
  ConfigWidget::SetDescription(
      m_ui->convergenceSlider, tr("Convergence:"),
      tr("This value is added to the convergence value set in the graphics configuration."));
  ConfigWidget::SetDescription(
      m_ui->monoscopicShadowsCheckBox, QString{},
      tr("Use a single depth buffer for both eyes. Needed for a few games."));

  const QString help_msg = tr(
      "Italics mark default game settings, bold marks user settings.\nRight-click to remove user "
      "settings.\nGraphics tabs don't display the value of a default game setting.\nAnti-Aliasing "
      "settings are disabled when the global graphics backend doesn't "
      "match the game setting.");
  m_ui->helpFrame->setToolTip(help_msg);
}

void GameConfigWidget::PopulateEditorTabs()
{
  PopulateTab(m_ui->defaultConfigTabWidget, File::GetSysDirectory() + GAMESETTINGS_DIR DIR_SEP,
              m_game_id, m_game.GetRevision(), true);
  PopulateTab(m_ui->userConfigTabWidget, File::GetUserPath(D_GAMESETTINGS_IDX), m_game_id,
              m_game.GetRevision(), false);

  for (int i = 0; i < m_ui->userConfigTabWidget->count(); ++i)
  {
    if (m_ui->userConfigTabWidget->tabText(i).toStdString() == m_game_id + ".ini")
      return;
  }

  // Create a new local game INI tab if none exists.
  auto* const edit = new GameConfigEdit(
      m_ui->userConfigTabWidget,
      QString::fromStdString(File::GetUserPath(D_GAMESETTINGS_IDX) + m_game_id + ".ini"), false);
  m_ui->userConfigTabWidget->addTab(edit, QString::fromStdString(m_game_id + ".ini"));
}

void GameConfigWidget::RefreshLocalEditor()
{
  for (int i = 0; i < m_ui->userConfigTabWidget->count(); ++i)
  {
    if (m_ui->userConfigTabWidget->tabText(i).toStdString() != m_game_id + ".ini")
      continue;

    QWidget* const old_editor = m_ui->userConfigTabWidget->widget(i);
    m_ui->userConfigTabWidget->removeTab(i);
    old_editor->deleteLater();

    auto* const edit = new GameConfigEdit(
        m_ui->userConfigTabWidget,
        QString::fromStdString(File::GetUserPath(D_GAMESETTINGS_IDX) + m_game_id + ".ini"), false);
    m_ui->userConfigTabWidget->insertTab(i, edit, QString::fromStdString(m_game_id + ".ini"));
    return;
  }
}

void GameConfigWidget::LoadSettings()
{
  const auto update_bool = [this](QCheckBox* box, const Config::Info<bool>& setting,
                                  bool reverse = false) {
    const Config::Location& location = setting.GetLocation();
    if (m_layer->Exists(location) || !m_global_layer->Exists(location))
      return;

    const std::optional<bool> value = m_global_layer->Get<bool>(location);
    if (!value.has_value())
      return;

    const QSignalBlocker blocker{box};
    box->setChecked(*value ^ reverse);
  };

  update_bool(m_ui->enableDualCoreCheckBox, Config::MAIN_CPU_THREAD);
  update_bool(m_ui->enableMmuCheckBox, Config::MAIN_MMU);
  update_bool(m_ui->enableFprfCheckBox, Config::MAIN_FPRF);
  update_bool(m_ui->syncGpuCheckBox, Config::MAIN_SYNC_GPU);
  update_bool(m_ui->emulateDiscSpeedCheckBox, Config::MAIN_FAST_DISC_SPEED, true);
  update_bool(m_ui->dspHleCheckBox, Config::MAIN_DSP_HLE);
  update_bool(m_ui->monoscopicShadowsCheckBox, Config::GFX_STEREO_EFB_MONO_DEPTH);

  const auto update_float = [this](QSlider* slider, const Config::Info<float>& setting,
                                   float minimum, float step) {
    const Config::Location& location = setting.GetLocation();
    if (m_layer->Exists(location) || !m_global_layer->Exists(location))
      return;

    const std::optional<float> value = m_global_layer->Get<float>(location);
    if (!value.has_value())
      return;

    const QSignalBlocker blocker{slider};
    slider->setValue(std::lround((*value - minimum) / step));
  };

  update_float(m_ui->depthSlider, Config::GFX_STEREO_DEPTH_PERCENTAGE, 100.0f, 1.0f);
  update_float(m_ui->convergenceSlider, Config::GFX_STEREO_CONVERGENCE, 0.0f, 0.01f);
  UpdateSliderLabels();
}

void GameConfigWidget::SetItalics()
{
  const auto mark = [this](QWidget* control, const Config::Location& location,
                           QLabel* label = nullptr) {
    if (!m_global_layer->Exists(location))
      return;

    QFont control_font = control->font();
    control_font.setItalic(true);
    control->setFont(control_font);

    if (label != nullptr)
    {
      QFont label_font = label->font();
      label_font.setItalic(true);
      label->setFont(label_font);
    }
  };

  mark(m_ui->enableDualCoreCheckBox, Config::MAIN_CPU_THREAD.GetLocation());
  mark(m_ui->enableMmuCheckBox, Config::MAIN_MMU.GetLocation());
  mark(m_ui->enableFprfCheckBox, Config::MAIN_FPRF.GetLocation());
  mark(m_ui->syncGpuCheckBox, Config::MAIN_SYNC_GPU.GetLocation());
  mark(m_ui->emulateDiscSpeedCheckBox, Config::MAIN_FAST_DISC_SPEED.GetLocation());
  mark(m_ui->dspHleCheckBox, Config::MAIN_DSP_HLE.GetLocation());
  mark(m_ui->deterministicDualCoreComboBox, Config::MAIN_GPU_DETERMINISM_MODE.GetLocation(),
       m_ui->deterministicDualCoreLabel);
  mark(m_ui->depthSlider, Config::GFX_STEREO_DEPTH_PERCENTAGE.GetLocation(), m_ui->depthLabel);
  mark(m_ui->convergenceSlider, Config::GFX_STEREO_CONVERGENCE.GetLocation(),
       m_ui->convergenceLabel);
  mark(m_ui->monoscopicShadowsCheckBox, Config::GFX_STEREO_EFB_MONO_DEPTH.GetLocation());
}

void GameConfigWidget::UpdateSliderLabels()
{
  const float depth = 100.0f + static_cast<float>(m_ui->depthSlider->value());
  const float convergence = static_cast<float>(m_ui->convergenceSlider->value()) * 0.01f;
  m_ui->depthValueLabel->setText(QString::asprintf("%.0f%%", static_cast<double>(depth)));
  m_ui->convergenceValueLabel->setText(QString::asprintf("%.2f", static_cast<double>(convergence)));
}
