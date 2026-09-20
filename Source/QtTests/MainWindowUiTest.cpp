// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAction>
#include <QMainWindow>
#include <QMargins>
#include <QStatusBar>
#include <QStringList>
#include <QToolBar>
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

TEST(MainWindowUiTest, FormOwnsTheToolbarAndItsStaticActions)
{
  QMainWindow window;
  Ui::MainWindow ui;
  ui.setupUi(&window);

  EXPECT_EQ(window.toolBarArea(ui.toolbar), Qt::TopToolBarArea);
  EXPECT_EQ(ui.toolbar->objectName(), QStringLiteral("toolbar"));
  EXPECT_EQ(ui.toolbar->toolButtonStyle(), Qt::ToolButtonTextUnderIcon);
  EXPECT_EQ(ui.toolbar->iconSize(), QSize(32, 32));
  EXPECT_FALSE(ui.toolbar->isFloatable());

  QStringList action_names;
  for (const QAction* const action : ui.toolbar->actions())
  {
    if (!action->isSeparator())
      action_names.push_back(action->objectName());
  }

  EXPECT_EQ(action_names,
            QStringList({QStringLiteral("actionToolbarStep"),
                         QStringLiteral("actionToolbarStepOver"),
                         QStringLiteral("actionToolbarStepOut"),
                         QStringLiteral("actionToolbarSkip"),
                         QStringLiteral("actionToolbarShowPC"),
                         QStringLiteral("actionToolbarSetPC"),
                         QStringLiteral("actionToolbarOpen"),
                         QStringLiteral("actionToolbarRefresh"),
                         QStringLiteral("actionToolbarPlayPause"),
                         QStringLiteral("actionToolbarStop"),
                         QStringLiteral("actionToolbarFullScreen"),
                         QStringLiteral("actionToolbarScreenShot"),
                         QStringLiteral("actionToolbarSettings"),
                         QStringLiteral("actionToolbarGraphics"),
                         QStringLiteral("actionToolbarControllers")}));
}

TEST(MainWindowUiTest, FormToolbarRestoresStateSavedByThePreviousToolbar)
{
  QMainWindow previous_window;
  auto* const previous_toolbar = new QToolBar(&previous_window);
  previous_toolbar->setObjectName(QStringLiteral("toolbar"));
  previous_window.addToolBar(Qt::BottomToolBarArea, previous_toolbar);
  previous_toolbar->hide();
  const QByteArray previous_state = previous_window.saveState();

  QMainWindow window;
  Ui::MainWindow ui;
  ui.setupUi(&window);

  ASSERT_TRUE(window.restoreState(previous_state));
  EXPECT_EQ(window.toolBarArea(ui.toolbar), Qt::BottomToolBarArea);
  EXPECT_TRUE(ui.toolbar->isHidden());
}
