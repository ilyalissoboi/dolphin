// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Settings/MemoryCardSyncPane.h"

#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSysInfo>

#include "Core/Config/MemoryCardSyncSettings.h"
#include "Core/HW/GCMemcard/MemcardSync.h"

#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"
#include "DolphinQt/QtUtils/QueueOnObject.h"

#include "ui_MemoryCardSyncPane.h"

MemoryCardSyncPane::MemoryCardSyncPane(QWidget* parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::MemoryCardSyncPane>())
{
  m_ui->setupUi(this);
  BindSettings();
  ConnectLayout();
  AddDescriptions();

  // emusync names the device after the computer when no name is set.
  m_ui->deviceNameLineEdit->setPlaceholderText(QSysInfo::machineHostName());
  m_status_timer.setInterval(2000);
  connect(&m_status_timer, &QTimer::timeout, this, &MemoryCardSyncPane::UpdateStatus);

  UpdateEnabledState();
  UpdateSecretPlaceholders();
  UpdateStatus();
}

MemoryCardSyncPane::~MemoryCardSyncPane() = default;

void MemoryCardSyncPane::BindSettings()
{
  ConfigWidget::Bind(m_ui->enabledCheckBox, Config::MAIN_MEMCARD_SYNC_ENABLED);
  ConfigWidget::Bind(m_ui->storageLineEdit, Config::MAIN_MEMCARD_SYNC_REMOTE_URI);
  ConfigWidget::Bind(m_ui->accessKeyIdLineEdit, Config::MAIN_MEMCARD_SYNC_ACCESS_KEY_ID);
  ConfigWidget::Bind(m_ui->deviceNameLineEdit, Config::MAIN_MEMCARD_SYNC_DEVICE_NAME);
}

void MemoryCardSyncPane::ConnectLayout()
{
  // Connected after the bindings above, so the setting is written before it is applied.
  connect(m_ui->enabledCheckBox, &QCheckBox::toggled, this, [this] { Apply(); });
  for (QLineEdit* edit :
       {m_ui->storageLineEdit, m_ui->accessKeyIdLineEdit, m_ui->deviceNameLineEdit})
  {
    connect(edit, &QLineEdit::editingFinished, this, [this, edit] {
      if (edit->isModified())
      {
        edit->setModified(false);
        Apply();
      }
    });
  }
  connect(m_ui->secretLineEdit, &QLineEdit::editingFinished, this,
          &MemoryCardSyncPane::OnSecretEdited);
  connect(m_ui->secretShowButton, &QPushButton::toggled, this, [this](bool shown) {
    // The stored secret access key is never read back: Show only unmasks what is being typed.
    m_ui->secretLineEdit->setEchoMode(shown ? QLineEdit::Normal : QLineEdit::Password);
  });
  connect(m_ui->passphraseLineEdit, &QLineEdit::editingFinished, this,
          &MemoryCardSyncPane::OnPassphraseEdited);
  connect(m_ui->passphraseShowButton, &QPushButton::toggled, this,
          &MemoryCardSyncPane::OnPassphraseShowToggled);
  connect(m_ui->passphraseRemoveButton, &QPushButton::clicked, this,
          &MemoryCardSyncPane::OnPassphraseRemoveClicked);
  connect(m_ui->syncNowButton, &QPushButton::clicked, this, &MemoryCardSyncPane::OnSyncNowClicked);
}

void MemoryCardSyncPane::AddDescriptions()
{
  ConfigWidget::SetDescription(
      m_ui->enabledCheckBox, tr("Sync Memory Cards"),
      tr("Newer saves from your other devices are put on the cards before a game starts; your "
         "saves upload as Dolphin writes them, and when the game "
         "closes.<br><br><dolphin_emphasis>If "
         "unsure, leave this unchecked.</dolphin_emphasis>"));
  ConfigWidget::SetDescription(
      m_ui->storageLineEdit, tr("Storage"),
      tr("Bucket and folder, e.g. s3://my-saves/emusync/v1?region=eu-west-1. Use the same on "
         "every device. S3-compatible services take an endpoint: "
         "s3://bucket/folder?endpoint=https://...&amp;region=auto."));
  ConfigWidget::SetDescription(
      m_ui->accessKeyIdLineEdit, tr("Access Key ID"),
      tr("From your storage provider. A key that can only read and write this bucket is enough."));
  ConfigWidget::SetDescription(
      m_ui->secretLineEdit, tr("Secret Access Key"),
      tr("Kept in your system's credential store (Credential Manager, Keychain or Secret "
         "Service), never in Dolphin's settings file, and never shown again. Type or paste a new "
         "one to replace it."));
  ConfigWidget::SetDescription(
      m_ui->passphraseLineEdit, tr("Encryption Passphrase"),
      tr("Optional. Encrypts your saves on this computer before they upload, so the storage "
         "provider can't read them. New storage is created encrypted; for storage that is already "
         "encrypted, enter the passphrase used when it was set up. Use the same passphrase on "
         "every device. It can't be recovered, and encrypted storage can't be made unencrypted. "
         "Kept in your system's credential store."));
  ConfigWidget::SetDescription(
      m_ui->deviceNameLineEdit, tr("Device Name"),
      tr("How this computer appears in a save's history on your other devices. Empty: the "
         "computer's name."));
}

void MemoryCardSyncPane::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  UpdateStatus();
  m_status_timer.start();
}

void MemoryCardSyncPane::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  m_status_timer.stop();
}

void MemoryCardSyncPane::OnSecretEdited()
{
  const std::string value = m_ui->secretLineEdit->text().trimmed().toStdString();
  if (value.empty())
    return;
  // The field never holds the stored secret: empty it again once it is handed over.
  m_ui->secretLineEdit->clear();
  m_ui->secretShowButton->setChecked(false);
  Apply({{MemcardSync::SECRET_ACCESS_KEY, value}});
}

void MemoryCardSyncPane::OnPassphraseEdited()
{
  // Not trimmed: emusync keeps spaces too, and the same passphrase has to unlock the storage on
  // every device.
  const QString text = m_ui->passphraseLineEdit->text();
  const bool revealed_and_unchanged =
      m_ui->passphraseShowButton->isChecked() &&
      m_ui->passphraseLineEdit->property("revealed").toString() == text;
  if (text.isEmpty() || revealed_and_unchanged)
    return;
  m_ui->passphraseLineEdit->clear();
  m_ui->passphraseLineEdit->setProperty("revealed", QVariant());
  m_ui->passphraseShowButton->setChecked(false);
  Apply({{MemcardSync::ENCRYPTION_PASSPHRASE, text.toStdString()}});
}

void MemoryCardSyncPane::OnPassphraseShowToggled(bool shown)
{
  QLineEdit* const edit = m_ui->passphraseLineEdit;
  edit->setEchoMode(shown ? QLineEdit::Normal : QLineEdit::Password);
  if (!shown)
  {
    // Hide puts a stored passphrase that was shown away again, rather than only masking it.
    if (edit->property("revealed").toString() == edit->text())
      edit->clear();
    edit->setProperty("revealed", QVariant());
    return;
  }
  if (!edit->text().isEmpty() || !MemcardSync::HasSecret(MemcardSync::ENCRYPTION_PASSPHRASE))
    return;

  // Show loads the stored passphrase: it's needed to set up other devices, and losing it loses the
  // saves. Reading the credential store can block (an unlock prompt), so not on the UI thread.
  std::thread([pane = QPointer<MemoryCardSyncPane>(this)] {
    std::string error;
    std::optional<std::string> value =
        MemcardSync::LoadSecret(MemcardSync::ENCRYPTION_PASSPHRASE, &error);
    QueueOnObject(qApp, [pane, value = std::move(value), error = std::move(error)]() mutable {
      if (pane && pane->m_ui->passphraseShowButton->isChecked())
      {
        if (value.has_value())
        {
          const QString text = QString::fromStdString(*value);
          pane->m_ui->passphraseLineEdit->setText(text);
          pane->m_ui->passphraseLineEdit->setProperty("revealed", text);
        }
        else
        {
          pane->m_ui->statusLabel->setText(error.empty() ? tr("No passphrase is saved.") :
                                                           QString::fromStdString(error));
        }
      }
      if (value.has_value())
        MemcardSync::WipeSecret(*value);
    });
  }).detach();
}

void MemoryCardSyncPane::OnPassphraseRemoveClicked()
{
  m_ui->passphraseLineEdit->clear();
  m_ui->passphraseLineEdit->setProperty("revealed", QVariant());
  m_ui->passphraseShowButton->setChecked(false);
  Apply({{MemcardSync::ENCRYPTION_PASSPHRASE, std::string()}});
}

void MemoryCardSyncPane::OnSyncNowClicked()
{
  MemcardSync::SyncNow();
  m_ui->statusLabel->setText(tr("Syncing..."));
}

void MemoryCardSyncPane::Apply(std::vector<std::pair<const char*, std::string>> secrets)
{
  std::vector<MemcardSync::SecretUpdate> updates;
  for (auto& [name, value] : secrets)
    updates.push_back({name, std::move(value)});
  m_starting = true;
  UpdateStatus();
  MemcardSync::ApplySettingsAsync(std::move(updates),
                                  [pane = QPointer<MemoryCardSyncPane>(this)](const std::string&) {
                                    QueueOnObject(qApp, [pane] {
                                      if (!pane)
                                        return;
                                      pane->m_starting = false;
                                      pane->UpdateSecretPlaceholders();
                                      pane->UpdateStatus();
                                    });
                                  });
}

void MemoryCardSyncPane::UpdateEnabledState()
{
  if (MemcardSync::IsAvailable())
    return;
  for (QWidget* w : std::initializer_list<QWidget*>{
           m_ui->enabledCheckBox, m_ui->storageLineEdit, m_ui->accessKeyIdLineEdit,
           m_ui->secretLineEdit, m_ui->secretShowButton, m_ui->passphraseLineEdit,
           m_ui->passphraseShowButton, m_ui->passphraseRemoveButton, m_ui->deviceNameLineEdit,
           m_ui->syncNowButton})
  {
    w->setEnabled(false);
  }
}

void MemoryCardSyncPane::UpdateSecretPlaceholders()
{
  m_ui->secretLineEdit->setPlaceholderText(
      MemcardSync::HasSecret(MemcardSync::SECRET_ACCESS_KEY) ? tr("Saved") : tr("Not set"));
  const bool has_passphrase = MemcardSync::HasSecret(MemcardSync::ENCRYPTION_PASSPHRASE);
  m_ui->passphraseLineEdit->setPlaceholderText(has_passphrase ? tr("Saved") :
                                                                tr("No passphrase saved"));
  m_ui->passphraseRemoveButton->setEnabled(has_passphrase && MemcardSync::IsAvailable());
}

void MemoryCardSyncPane::UpdateStatus()
{
  QString text;
  if (!MemcardSync::IsAvailable())
    text = tr("This build of Dolphin has no memory card sync.");
  else if (m_starting)
    text = tr("Connecting to the storage...");
  else if (const std::string error = MemcardSync::GetLastStartError(); !error.empty())
    text = QString::fromStdString(error);
  else if (!m_ui->enabledCheckBox->isChecked())
    text = tr("Off.");
  else if (!MemcardSync::IsRunning())
    text = tr("Not syncing: fill in the storage, access key ID and secret access key.");
  else if (const std::string status = MemcardSync::GetStatus(); !status.empty())
    text = QString::fromStdString(status);
  else
    text = tr("Syncing. Nothing has changed yet.");
  m_ui->statusLabel->setText(text);
  m_ui->syncNowButton->setEnabled(MemcardSync::IsAvailable() && MemcardSync::IsRunning() &&
                                  !m_starting);
}
