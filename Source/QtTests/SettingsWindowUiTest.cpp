// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractScrollArea>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
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
  EXPECT_EQ(ui.rootLayout->itemAt(0)->widget(), ui.navigationFrame);
  EXPECT_EQ(ui.rootLayout->itemAt(1)->widget(), ui.contentWidget);

  EXPECT_EQ(ui.navigationFrame->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
  EXPECT_EQ(ui.navigationFrame->frameShape(), QFrame::NoFrame);
  EXPECT_EQ(ui.navigationLayout->contentsMargins(), QMargins(12, 0, 12, 0));
  EXPECT_EQ(ui.navigationLayout->spacing(), 0);
  ASSERT_EQ(ui.navigationLayout->count(), 1);
  EXPECT_EQ(ui.navigationLayout->itemAt(0)->widget(), ui.navigationList);

  EXPECT_EQ(ui.navigationList->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
  EXPECT_EQ(ui.navigationList->sizeAdjustPolicy(),
            QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
  EXPECT_EQ(ui.navigationList->iconSize(), QSize(32, 32));
  EXPECT_TRUE(ui.navigationList->wordWrap());
  EXPECT_EQ(ui.navigationList->accessibleName(), QStringLiteral("Settings categories"));

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
  EXPECT_EQ(ui.helpText->accessibleName(), QStringLiteral("Setting description"));

  EXPECT_NE(ui.buttonBox->button(QDialogButtonBox::StandardButton::Close), nullptr);
}
