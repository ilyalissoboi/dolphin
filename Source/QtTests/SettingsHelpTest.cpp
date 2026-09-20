// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QPushButton>

#include <gtest/gtest.h>

#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"
#include "DolphinQt/Config/SettingsHelp.h"
#include "DolphinQt/Config/ToolTipControls/ToolTipWidget.h"

namespace
{
class LegacyHelpButton final : public ToolTipWidget<QPushButton>
{
private:
  QPoint GetToolTipPosition() const override { return rect().center(); }
};
}  // namespace

TEST(SettingsHelpTest, LegacyTooltipControlsPublishPersistentHelpMetadata)
{
  LegacyHelpButton button;
  button.SetTitle(QStringLiteral("Legacy title"));
  button.SetDescription(QStringLiteral("Legacy description"));

  EXPECT_EQ(SettingsHelp::Title(&button), QStringLiteral("Legacy title"));
  EXPECT_EQ(SettingsHelp::Description(&button), QStringLiteral("Legacy description"));
}

TEST(SettingsHelpTest, BinderControlsPublishPersistentHelpMetadata)
{
  QPushButton button(QStringLiteral("Button title"));
  ConfigWidget::SetDescription(&button, {}, QStringLiteral("Binder description"));

  EXPECT_EQ(SettingsHelp::Title(&button), QStringLiteral("Button title"));
  EXPECT_EQ(SettingsHelp::Description(&button), QStringLiteral("Binder description"));
  EXPECT_EQ(ConfigWidget::ToolTipTitle(&button), QStringLiteral("Button title"));
  EXPECT_EQ(ConfigWidget::ToolTipDescription(&button), QStringLiteral("Binder description"));
}
