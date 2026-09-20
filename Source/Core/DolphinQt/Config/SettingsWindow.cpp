// Copyright 2016 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/SettingsWindow.h"

#include <memory>
#include <utility>

#include <QApplication>
#include <QChildEvent>
#include <QColor>
#include <QDialogButtonBox>
#include <QEvent>
#include <QIcon>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPalette>
#include <QStackedWidget>
#include <QStyle>
#include <QTabWidget>
#include <QTextBrowser>

#include "DiscIO/Enums.h"
#include "DolphinQt/Config/ControllersPane.h"
#include "DolphinQt/Config/Graphics/GraphicsPane.h"
#include "DolphinQt/Config/SettingsHelp.h"
#include "DolphinQt/MainWindow.h"
#include "DolphinQt/QtUtils/QtUtils.h"
#include "DolphinQt/QtUtils/WrapInScrollArea.h"
#include "DolphinQt/Resources.h"
#include "DolphinQt/Settings.h"
#include "DolphinQt/Settings/AdvancedPane.h"
#include "DolphinQt/Settings/AudioPane.h"
#include "DolphinQt/Settings/GameCubePane.h"
#include "DolphinQt/Settings/GeneralPane.h"
#include "DolphinQt/Settings/InterfacePane.h"
#include "DolphinQt/Settings/OnScreenDisplayPane.h"
#include "DolphinQt/Settings/PathPane.h"
#include "DolphinQt/Settings/TriforcePane.h"
#include "DolphinQt/Settings/WiiPane.h"

#include "ui_SettingsWindow.h"

StackedSettingsWindow::StackedSettingsWindow(QWidget* parent)
    : QDialog{parent}, m_ui{std::make_unique<Ui::SettingsWindow>()}
{
  m_ui->setupUi(this);

  // Calculated value for the padding in our list items.
  const int list_item_padding = style()->pixelMetric(QStyle::PM_LayoutLeftMargin) / 2;

  // FYI: "base" is the window color on Windows and "alternate-base" is very high contrast on macOS.
  const auto* const list_background =
#if !defined(__APPLE__)
      "palette(alternate-base)";
#else
      "palette(base)";
#endif

  m_ui->navigationList->setStyleSheet(
      QString::fromUtf8(
          // Remove border around entire widget and adjust background color.
          "QListWidget { border: 0; background: %1; } "
          // Note: padding-left is broken unless border is set, which then breaks colors.
          // see: https://bugreports.qt.io/browse/QTBUG-122698
          "QListWidget::item { padding-top: %2px; padding-bottom: %2px; } "
          // Maintain selected item color when unfocused.
          "QListWidget::item:selected { background: palette(highlight); "
          // Prevent text color change on focus loss.
          "color: palette(highlighted-text); "
          "} "
          // Remove ugly dotted outline on selected row (Windows and GNOME).
          "* { outline: none; } ")
          .arg(QString::fromUtf8(list_background))
          .arg(list_item_padding));

  UpdateNavigationListStyle();

  connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(m_ui->navigationList, &QListWidget::currentRowChanged, this,
          &StackedSettingsWindow::OnCurrentRowChanged);
}

StackedSettingsWindow::~StackedSettingsWindow() = default;

void StackedSettingsWindow::OnDoneCreatingPanes()
{
  // Make sure the first item is actually selected by default.
  ActivatePane(0);
  m_ui->helpText->setVisible(m_has_help);
  if (m_has_help)
  {
    InstallHelpEventFilters(m_ui->stackedPanes);
    ShowCategoryHelp();
  }
  // Take on the preferred size.
  QtUtils::AdjustSizeWithinScreen(this);
}

void StackedSettingsWindow::changeEvent(QEvent* event)
{
  QDialog::changeEvent(event);

  const auto type = event->type();

  const bool palette_changed = type == QEvent::PaletteChange;
  const bool application_palette_changed = type == QEvent::ApplicationPaletteChange;
  const bool style_changed = type == QEvent::StyleChange;
  const bool theme_event = type == QEvent::ThemeChange;

  const bool theme_changed = application_palette_changed || theme_event;

  if (theme_changed && !m_handling_theme_change)
  {
    m_handling_theme_change = true;
    Settings::Instance().ApplyStyle();
    // Ensure the dialog and its children adopt the new system palette.
    setPalette(qApp->palette());
    Settings::Instance().TriggerThemeChanged();
    m_handling_theme_change = false;
  }

  if (palette_changed || application_palette_changed || style_changed || theme_event)
    UpdateNavigationListStyle();
}

void StackedSettingsWindow::UpdateNavigationListStyle()
{
  if (!m_ui)
    return;

  QPalette list_palette = m_ui->navigationList->palette();
  const QPalette app_palette = qApp->palette();

  QColor highlight_color = app_palette.color(QPalette::Active, QPalette::Highlight);
  QColor highlighted_text = app_palette.color(QPalette::Active, QPalette::HighlightedText);

#if defined(__APPLE__)
  const bool is_dark_theme = Settings::Instance().IsThemeDark();
  // The default macOS accent is quite light in our list; darken it for readability in light mode.
  if (!is_dark_theme)
  {
    highlight_color = highlight_color.darker(130);
    highlighted_text = QColor(Qt::white);
  }
#endif

  for (const QPalette::ColorGroup group : {QPalette::Active, QPalette::Inactive})
  {
    list_palette.setColor(group, QPalette::Base, app_palette.color(group, QPalette::Base));
    list_palette.setColor(group, QPalette::AlternateBase,
                          app_palette.color(group, QPalette::AlternateBase));
    list_palette.setColor(group, QPalette::Highlight, highlight_color);
    list_palette.setColor(group, QPalette::HighlightedText, highlighted_text);
  }

  m_ui->navigationList->setPalette(list_palette);
}

void StackedSettingsWindow::AddPane(QWidget* widget, const QString& name)
{
  m_ui->stackedPanes->addWidget(widget);
  // Pad the left and right of each item.
  m_ui->navigationList->addItem(QStringLiteral("  %1  ").arg(name));
  m_category_help_text.emplace_back();
}

void StackedSettingsWindow::AddPane(QWidget* widget, const QString& name, const QIcon& icon,
                                    QString help_text)
{
  m_ui->stackedPanes->addWidget(widget);
  m_ui->navigationList->addItem(new QListWidgetItem{icon, name});
  m_has_help = m_has_help || !help_text.isEmpty();
  m_category_help_text.emplace_back(std::move(help_text));
}

void StackedSettingsWindow::AddWrappedPane(QWidget* widget, const QString& name)
{
  AddPane(GetWrappedWidget(widget), name);
}

void StackedSettingsWindow::AddWrappedPane(QWidget* widget, const QString& name, const QIcon& icon,
                                           QString help_text)
{
  AddPane(GetWrappedWidget(widget), name, icon, std::move(help_text));
}

void StackedSettingsWindow::ActivatePane(int index)
{
  m_ui->navigationList->setCurrentRow(index);
}

void StackedSettingsWindow::SetPaneIcon(int index, const QIcon& icon)
{
  if (QListWidgetItem* const item = m_ui->navigationList->item(index))
    item->setIcon(icon);
}

QWidget* StackedSettingsWindow::FindHelpWidget(QObject* object) const
{
  QWidget* widget = qobject_cast<QWidget*>(object);
  while (widget != nullptr && widget != m_ui->stackedPanes)
  {
    if (!SettingsHelp::Description(widget).isEmpty())
      return widget;
    widget = widget->parentWidget();
  }
  return nullptr;
}

void StackedSettingsWindow::InstallHelpEventFilters(QWidget* root)
{
  root->installEventFilter(this);
  for (QWidget* const widget : root->findChildren<QWidget*>())
    widget->installEventFilter(this);
}

void StackedSettingsWindow::OnCurrentRowChanged(int index)
{
  m_ui->stackedPanes->setCurrentIndex(index);
  m_current_help_widget.clear();
  ShowCategoryHelp();
}

void StackedSettingsWindow::ShowCategoryHelp()
{
  if (!m_has_help)
    return;

  const int index = m_ui->navigationList->currentRow();
  if (index < 0 || static_cast<size_t>(index) >= m_category_help_text.size())
  {
    m_ui->helpText->clear();
    return;
  }

  m_ui->helpText->setHtml(m_category_help_text[static_cast<size_t>(index)]);
}

void StackedSettingsWindow::ShowControlHelp(QWidget* widget)
{
  const QString description = SettingsHelp::Description(widget);
  if (description.isEmpty())
    return;

  const QString title = SettingsHelp::Title(widget);
  m_ui->helpText->setHtml(
      title.isEmpty() ?
          description :
          QStringLiteral("<strong>%1</strong><hr>%2").arg(title.toHtmlEscaped(), description));
  m_current_help_widget = widget;
}

bool StackedSettingsWindow::eventFilter(QObject* watched, QEvent* event)
{
  if (!m_has_help)
    return QDialog::eventFilter(watched, event);

  if (event->type() == QEvent::ChildAdded)
  {
    auto* const child_event = static_cast<QChildEvent*>(event);
    if (auto* const child = qobject_cast<QWidget*>(child_event->child()))
      InstallHelpEventFilters(child);
  }

  QWidget* const help_widget = FindHelpWidget(watched);
  if (help_widget == nullptr)
    return QDialog::eventFilter(watched, event);

  if (event->type() == QEvent::Enter || event->type() == QEvent::FocusIn)
  {
    ShowControlHelp(help_widget);
  }
  else if ((event->type() == QEvent::Leave || event->type() == QEvent::FocusOut) &&
           m_current_help_widget == help_widget && !help_widget->underMouse() &&
           !help_widget->hasFocus())
  {
    m_current_help_widget.clear();
    ShowCategoryHelp();
  }

  return QDialog::eventFilter(watched, event);
}

namespace
{
QIcon GetSettingsCategoryIcon(SettingsWindowPaneIndex index, QWidget* parent)
{
  switch (index)
  {
  case SettingsWindowPaneIndex::General:
    return Resources::GetThemeIcon("config");
  case SettingsWindowPaneIndex::Graphics:
    return Resources::GetThemeIcon("graphics");
  case SettingsWindowPaneIndex::Controllers:
    return Resources::GetThemeIcon("classic");
  case SettingsWindowPaneIndex::Interface:
    return Resources::GetAppIcon();
  case SettingsWindowPaneIndex::OnScreenDisplay:
    return Resources::GetThemeIcon("screenshot");
  case SettingsWindowPaneIndex::Audio:
    return parent->style()->standardIcon(QStyle::SP_MediaVolume);
  case SettingsWindowPaneIndex::Paths:
    return Resources::GetThemeIcon("open");
  case SettingsWindowPaneIndex::GameCube:
    return Resources::GetPlatform(DiscIO::Platform::GameCubeDisc);
  case SettingsWindowPaneIndex::Wii:
    return Resources::GetPlatform(DiscIO::Platform::WiiDisc);
  case SettingsWindowPaneIndex::Triforce:
    return Resources::GetPlatform(DiscIO::Platform::Triforce);
  case SettingsWindowPaneIndex::Advanced:
    return parent->style()->standardIcon(QStyle::SP_MessageBoxWarning);
  }
  return {};
}
}  // namespace

SettingsWindow::SettingsWindow(MainWindow* parent) : StackedSettingsWindow{parent}
{
  setWindowTitle(tr("Settings"));

  // If you change the order, don't forget to update the SettingsWindowPaneIndex enum.
  AddWrappedPane(
      new GeneralPane, tr("General"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::General, this),
      tr("<strong>General Settings</strong><hr>Configure startup behavior, emulation speed, "
         "automatic updates, and usage statistics."));
  AddPane(new GraphicsPane{parent, nullptr}, tr("Graphics"),
          GetSettingsCategoryIcon(SettingsWindowPaneIndex::Graphics, this),
          tr("<strong>Graphics Settings</strong><hr>Choose the rendering backend and configure "
             "display, enhancements, graphics hacks, and advanced graphics options."));
  AddWrappedPane(
      new ControllersPane, tr("Controllers"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::Controllers, this),
      tr("<strong>Controller Settings</strong><hr>Configure GameCube controllers, Wii Remotes, "
         "adapters, and input sources."));
  AddWrappedPane(
      new InterfacePane, tr("Interface"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::Interface, this),
      tr("<strong>Interface Settings</strong><hr>Control Dolphin's appearance, language, prompts, "
         "hotkeys, and window behavior."));
  AddWrappedPane(
      new OnScreenDisplayPane, tr("On-Screen Display"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::OnScreenDisplay, this),
      tr("<strong>On-Screen Display Settings</strong><hr>Choose which performance, input, and "
         "system messages Dolphin shows over games."));
  AddWrappedPane(
      new AudioPane, tr("Audio"), GetSettingsCategoryIcon(SettingsWindowPaneIndex::Audio, this),
      tr("<strong>Audio Settings</strong><hr>Configure the DSP engine, audio backend, volume, "
         "latency, and Wii Remote audio."));
  AddWrappedPane(
      new PathPane, tr("Paths"), GetSettingsCategoryIcon(SettingsWindowPaneIndex::Paths, this),
      tr("<strong>Path Settings</strong><hr>Choose the folders Dolphin scans and uses for games, "
         "saves, screenshots, and other data."));
  AddWrappedPane(
      new GameCubePane{parent}, tr("GameCube"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::GameCube, this),
      tr("<strong>GameCube Settings</strong><hr>Configure GameCube system language, memory cards, "
         "devices, and broadband adapter behavior."));
  AddWrappedPane(
      new WiiPane, tr("Wii"), GetSettingsCategoryIcon(SettingsWindowPaneIndex::Wii, this),
      tr("<strong>Wii Settings</strong><hr>Configure Wii system language, storage, input devices, "
         "and network services."));
  AddWrappedPane(
      new TriforcePane, tr("Triforce"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::Triforce, this),
      tr("<strong>Triforce Settings</strong><hr>Configure Triforce controllers and IP address "
         "redirections."));
  AddWrappedPane(
      new AdvancedPane, tr("Advanced"),
      GetSettingsCategoryIcon(SettingsWindowPaneIndex::Advanced, this),
      tr("<strong>Advanced Settings</strong><hr>Configure CPU, timing, custom clock, memory, and "
         "other expert options."));

  OnDoneCreatingPanes();
  connect(&Settings::Instance(), &Settings::ThemeChanged, this,
          &SettingsWindow::UpdateCategoryIcons);
}

void SettingsWindow::SelectPane(SettingsWindowPaneIndex index)
{
  ActivatePane(std::to_underlying(index));
}

void SettingsWindow::closeEvent(QCloseEvent*)
{
  Config::Save();
}

void SettingsWindow::UpdateCategoryIcons()
{
  for (int index = std::to_underlying(SettingsWindowPaneIndex::General);
       index <= std::to_underlying(SettingsWindowPaneIndex::Advanced); ++index)
  {
    SetPaneIcon(index, GetSettingsCategoryIcon(static_cast<SettingsWindowPaneIndex>(index), this));
  }
}
