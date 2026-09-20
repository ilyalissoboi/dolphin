// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractScrollArea>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QMargins>
#include <QSize>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QWidget>

#include <gtest/gtest.h>

#include "ui_SettingsWindow.h"

TEST(SettingsWindowUiTest, FormOwnsTheSharedSettingsShell)
{
  QDialog dialog;
  Ui::SettingsWindow ui;
  ui.setupUi(&dialog);

  EXPECT_EQ(ui.rootLayout->contentsMargins(), QMargins());
  EXPECT_EQ(ui.rootLayout->spacing(), 0);
  ASSERT_EQ(ui.rootLayout->count(), 2);
  EXPECT_EQ(ui.rootLayout->itemAt(0)->widget(), ui.navigationList);
  EXPECT_EQ(ui.rootLayout->itemAt(1)->widget(), ui.contentWidget);

  EXPECT_EQ(ui.navigationList->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
  EXPECT_EQ(ui.navigationList->sizeAdjustPolicy(),
            QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
  EXPECT_EQ(ui.navigationList->iconSize(), QSize(32, 32));
  EXPECT_TRUE(ui.navigationList->wordWrap());

  EXPECT_EQ(ui.contentLayout->contentsMargins(), QMargins());
  EXPECT_EQ(ui.contentLayout->spacing(), 0);
  ASSERT_EQ(ui.contentLayout->count(), 3);
  EXPECT_EQ(ui.contentLayout->itemAt(0)->widget(), ui.stackedPanes);
  EXPECT_EQ(ui.contentLayout->itemAt(1)->widget(), ui.helpText);
  EXPECT_EQ(ui.contentLayout->itemAt(2)->widget(), ui.footerFrame);

  EXPECT_TRUE(ui.helpText->isReadOnly());
  EXPECT_EQ(ui.helpText->minimumHeight(), 122);
  EXPECT_EQ(ui.helpText->maximumHeight(), 122);
  EXPECT_TRUE(ui.helpText->isHidden());

  EXPECT_NE(ui.buttonBox->button(QDialogButtonBox::StandardButton::Close), nullptr);
}
