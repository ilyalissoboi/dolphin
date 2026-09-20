// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <gtest/gtest.h>

#include "ui_GeneralPane.h"

namespace
{
QWidget* NextTabFocusWidget(QWidget* widget)
{
  QWidget* next = widget;
  do
  {
    next = next->nextInFocusChain();
  } while (!(next->focusPolicy() & Qt::TabFocus));
  return next;
}
}  // namespace

TEST(GeneralPaneUiTest, FormOwnsTheGeneralSettingsStructure)
{
  QWidget widget;
  Ui::GeneralPane ui;
  ui.setupUi(&widget);

  ASSERT_EQ(ui.rootLayout->count(), 5);
  EXPECT_EQ(ui.rootLayout->itemAt(0)->widget(), ui.basicGroup);
  EXPECT_EQ(ui.rootLayout->itemAt(1)->widget(), ui.autoUpdateGroup);
  EXPECT_EQ(ui.rootLayout->itemAt(2)->widget(), ui.fallbackRegionGroup);
  EXPECT_EQ(ui.rootLayout->itemAt(3)->widget(), ui.analyticsGroup);
  EXPECT_NE(ui.rootLayout->itemAt(4)->spacerItem(), nullptr);

  ASSERT_EQ(ui.basicLayout->count(), 7);
  EXPECT_EQ(ui.basicLayout->itemAt(0)->widget(), ui.dualCoreCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(1)->widget(), ui.cheatsCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(2)->widget(), ui.loadGameIntoMemoryCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(3)->widget(), ui.overrideRegionSettingsCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(4)->widget(), ui.autoDiscChangeCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(5)->widget(), ui.discordPresenceCheckBox);
  EXPECT_EQ(ui.basicLayout->itemAt(6)->layout(), ui.speedLimitLayout);

  EXPECT_EQ(ui.speedLimitLayout->fieldGrowthPolicy(),
            QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
  EXPECT_EQ(ui.speedLimitLabel->buddy(), ui.speedLimitComboBox);
  EXPECT_EQ(ui.autoUpdateLabel->buddy(), ui.autoUpdateComboBox);
  EXPECT_EQ(ui.fallbackRegionLabel->buddy(), ui.fallbackRegionComboBox);

  ASSERT_EQ(ui.autoUpdateComboBox->count(), 3);
  EXPECT_EQ(ui.autoUpdateComboBox->itemText(0), QStringLiteral("Don't Update"));
  EXPECT_EQ(ui.autoUpdateComboBox->itemText(1), QStringLiteral("Releases (every few months)"));
  EXPECT_EQ(ui.autoUpdateComboBox->itemText(2), QStringLiteral("Dev (multiple times a day)"));

  ASSERT_EQ(ui.fallbackRegionComboBox->count(), 4);
  EXPECT_EQ(ui.fallbackRegionComboBox->itemText(0), QStringLiteral("NTSC-J"));
  EXPECT_EQ(ui.fallbackRegionComboBox->itemText(1), QStringLiteral("NTSC-U"));
  EXPECT_EQ(ui.fallbackRegionComboBox->itemText(2), QStringLiteral("PAL"));
  EXPECT_EQ(ui.fallbackRegionComboBox->itemText(3), QStringLiteral("NTSC-K"));

  EXPECT_EQ(NextTabFocusWidget(ui.dualCoreCheckBox), ui.cheatsCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.cheatsCheckBox), ui.loadGameIntoMemoryCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.loadGameIntoMemoryCheckBox), ui.overrideRegionSettingsCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.overrideRegionSettingsCheckBox), ui.autoDiscChangeCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.autoDiscChangeCheckBox), ui.discordPresenceCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.discordPresenceCheckBox), ui.speedLimitComboBox);
  EXPECT_EQ(NextTabFocusWidget(ui.speedLimitComboBox), ui.autoUpdateComboBox);
  EXPECT_EQ(NextTabFocusWidget(ui.autoUpdateComboBox), ui.fallbackRegionComboBox);
  EXPECT_EQ(NextTabFocusWidget(ui.fallbackRegionComboBox), ui.analyticsCheckBox);
  EXPECT_EQ(NextTabFocusWidget(ui.analyticsCheckBox), ui.generateIdentityButton);
}
