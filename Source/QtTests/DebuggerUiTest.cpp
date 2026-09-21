// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractItemView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>
#include <QRadioButton>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include <gtest/gtest.h>

#include "ui_AssembleInstructionDialog.h"
#include "ui_BreakpointDialog.h"
#include "ui_BreakpointWidget.h"
#include "ui_EditSymbolDialog.h"
#include "ui_MemoryViewWidget.h"
#include "ui_PatchInstructionDialog.h"
#include "ui_RegisterWidget.h"
#include "ui_WatchWidget.h"

TEST(DebuggerUiTest, InstructionDialogsOwnPermanentControls)
{
  QDialog assemble_dialog;
  Ui::AssembleInstructionDialog assemble_ui;
  assemble_ui.setupUi(&assemble_dialog);

  EXPECT_EQ(assemble_ui.mainLayout->count(), 5);
  EXPECT_EQ(assemble_ui.messageLabel->text(), QStringLiteral("No input"));
  EXPECT_EQ(assemble_ui.buttonBox->standardButtons(),
            QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  QDialog patch_dialog;
  Ui::PatchInstructionDialog patch_ui;
  patch_ui.setupUi(&patch_dialog);

  EXPECT_EQ(patch_ui.mainLayout->count(), 4);
  EXPECT_EQ(patch_ui.instructionLabel->text(), QStringLiteral("New instruction:"));
  EXPECT_EQ(patch_ui.instructionLabel->buddy(), patch_ui.inputEdit);
  EXPECT_EQ(patch_ui.buttonBox->standardButtons(), QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
}

TEST(DebuggerUiTest, EditSymbolFormOwnsSizeRow)
{
  QDialog dialog;
  Ui::EditSymbolDialog ui;
  ui.setupUi(&dialog);

  EXPECT_EQ(ui.mainLayout->count(), 4);
  EXPECT_EQ(ui.addressEndLabel->buddy(), ui.addressEndEdit);
  EXPECT_EQ(ui.sizeHexLabel->buddy(), ui.sizeHexEdit);
  EXPECT_EQ(ui.sizeLinesLabel->buddy(), ui.sizeLinesSpinBox);
  EXPECT_EQ(ui.sizeHexEdit->maxLength(), 7);
  EXPECT_EQ(ui.sizeLinesSpinBox->maximum(), 99999);
  EXPECT_EQ(ui.buttonBox->standardButtons(), QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
}

TEST(DebuggerUiTest, BreakpointFormPreservesGroupsAndDefaults)
{
  QDialog dialog;
  Ui::BreakpointDialog ui;
  ui.setupUi(&dialog);

  EXPECT_EQ(ui.mainLayout->count(), 4);
  EXPECT_EQ(ui.instructionAddressLabel->buddy(), ui.instructionAddressEdit);
  EXPECT_TRUE(ui.memoryUseAddressRadio->isChecked());
  EXPECT_TRUE(ui.memoryOnWriteRadio->isChecked());
  EXPECT_TRUE(ui.writeLogAndBreakRadio->isChecked());
  EXPECT_EQ(ui.memoryConditionBox->title(), QStringLiteral("Condition"));
  EXPECT_EQ(ui.actionBox->title(), QStringLiteral("Action"));
  EXPECT_EQ(ui.buttonBox->standardButtons(),
            QDialogButtonBox::Cancel | QDialogButtonBox::Help | QDialogButtonBox::Ok);
}

TEST(DebuggerUiTest, TableDockFormsOwnShellProperties)
{
  QWidget breakpoint;
  Ui::BreakpointWidget breakpoint_ui;
  breakpoint_ui.setupUi(&breakpoint);
  EXPECT_EQ(breakpoint_ui.mainLayout->contentsMargins(), QMargins(2, 2, 2, 2));
  EXPECT_EQ(breakpoint_ui.mainLayout->spacing(), 0);
  EXPECT_EQ(breakpoint_ui.toolbar->toolButtonStyle(), Qt::ToolButtonTextBesideIcon);
  EXPECT_FALSE(breakpoint_ui.table->tabKeyNavigation());
  EXPECT_EQ(breakpoint_ui.table->selectionMode(), QAbstractItemView::NoSelection);

  QWidget watch;
  Ui::WatchWidget watch_ui;
  watch_ui.setupUi(&watch);
  EXPECT_EQ(watch_ui.mainLayout->contentsMargins(), QMargins(2, 2, 2, 2));
  EXPECT_EQ(watch_ui.mainLayout->spacing(), 0);
  EXPECT_EQ(watch_ui.table->selectionMode(), QAbstractItemView::ExtendedSelection);
  EXPECT_EQ(watch_ui.table->selectionBehavior(), QAbstractItemView::SelectRows);

  QWidget registers;
  Ui::RegisterWidget register_ui;
  register_ui.setupUi(&registers);
  EXPECT_EQ(register_ui.mainLayout->contentsMargins(), QMargins(2, 2, 2, 2));
  EXPECT_FALSE(register_ui.table->tabKeyNavigation());
  EXPECT_EQ(register_ui.table->selectionMode(), QAbstractItemView::NoSelection);
}

TEST(DebuggerUiTest, MemoryViewFormOwnsZeroMarginInsertionLayout)
{
  QWidget widget;
  Ui::MemoryViewWidget ui;
  ui.setupUi(&widget);

  EXPECT_EQ(ui.mainLayout->contentsMargins(), QMargins());
  EXPECT_EQ(ui.mainLayout->count(), 0);
}
