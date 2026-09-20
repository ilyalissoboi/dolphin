// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QLineEdit>
#include <QMargins>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>

#include <gtest/gtest.h>

#include "ui_GameListWidget.h"

TEST(GameListUiTest, FormOwnsTheViewStackAndExistingSearchBar)
{
  QWidget widget;
  Ui::GameListWidget ui;
  ui.setupUi(&widget);

  EXPECT_EQ(ui.rootLayout->contentsMargins(), QMargins());
  EXPECT_EQ(ui.rootLayout->spacing(), 0);
  EXPECT_EQ(ui.rootLayout->count(), 2);
  EXPECT_EQ(ui.rootLayout->itemAt(0)->widget(), ui.viewStack);
  EXPECT_EQ(ui.rootLayout->itemAt(1)->widget(), ui.searchBar);

  EXPECT_TRUE(ui.searchBar->isHidden());
  EXPECT_EQ(ui.searchBar->minimumHeight(), 32);
  EXPECT_EQ(ui.searchBar->maximumHeight(), 32);
  EXPECT_EQ(ui.searchEdit->placeholderText(), QStringLiteral("Search games..."));
  EXPECT_EQ(ui.closeSearchButton->text(), QStringLiteral("Close"));
  EXPECT_EQ(ui.searchEdit->nextInFocusChain(), ui.closeSearchButton);
}
