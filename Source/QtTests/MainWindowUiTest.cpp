// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QMainWindow>
#include <QMargins>
#include <QStatusBar>
#include <gtest/gtest.h>

#include "ui_MainWindow.h"

TEST(MainWindowUiTest, FormOwnsTheCentralStackAndPromotedStatusBar)
{
  QMainWindow window;
  Ui::MainWindow ui;

  ui.setupUi(&window);

  EXPECT_EQ(window.centralWidget(), ui.mainStack);
  EXPECT_EQ(window.statusBar(), ui.statusBar);
  EXPECT_EQ(ui.mainStack->count(), 1);
  EXPECT_EQ(ui.mainStack->currentWidget(), ui.gameListPage);
  EXPECT_EQ(ui.gameListLayout->contentsMargins(), QMargins());
  EXPECT_EQ(ui.gameListLayout->spacing(), 0);
  EXPECT_TRUE(window.acceptDrops());
}

TEST(MainWindowUiTest, PromotedStatusBarKeepsTheExistingGameCountText)
{
  QMainWindow window;
  Ui::MainWindow ui;
  ui.setupUi(&window);

  ui.statusBar->OnGameCountUpdated(10, 8);

  EXPECT_EQ(ui.statusBar->currentMessage(),
            QStringLiteral("10 game(s) in your collection (8 visible, 2 filtered)"));
}
