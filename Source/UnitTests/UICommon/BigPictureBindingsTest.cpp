// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>

#include "Common/IniFile.h"
#include "InputCommon/BigPictureInput.h"
#include "InputCommon/ControlReference/ControlReference.h"

TEST(BigPictureBindingsTest, BindingsAndPreferencesRoundTripWithoutGameControllerConfig)
{
  BigPictureInput::MenuController original(0);
  original.SetDefaultDevice("SDL/0/Test pad");
  original.GetButtons()->SetControlExpression(0, "@(Back+Start)");
  original.GetButtons()->SetControlExpression(1, "`Pad N` | `Left Y+`");
  original.SetOptions({true, 1.25f, 0.1f});
  Common::IniFile ini;
  auto* section = ini.GetOrCreateSection(original.GetName());
  original.SaveConfig(section);
  BigPictureInput::MenuController restored(0);
  restored.LoadConfig(section);
  EXPECT_EQ(restored.GetDefaultDevice().ToString(), "SDL/0/Test pad");
  EXPECT_EQ(restored.GetButtons()->controls[0]->control_ref->GetExpression(), "@(Back+Start)");
  EXPECT_EQ(restored.GetButtons()->controls[1]->control_ref->GetExpression(),
            "`Pad N` | `Left Y+`");
  EXPECT_EQ(restored.GetOptions(), (BigPicture::NavigationOptions{true, 1.25f, 0.1f}));
  EXPECT_EQ(restored.GetButtons()->controls.size(), BigPicture::NAV_BUTTON_COUNT);
}

TEST(BigPictureBindingsTest, BindingSetsUseIndependentIniSections)
{
  Common::IniFile ini;
  for (int port = 0; port < BigPictureInput::CONTROLLER_COUNT; ++port)
  {
    BigPictureInput::MenuController controller(port);
    controller.GetButtons()->SetControlExpression(0, "`Button " + std::to_string(port) + "`");
    controller.SaveConfig(ini.GetOrCreateSection(controller.GetName()));
  }
  for (int port = 0; port < BigPictureInput::CONTROLLER_COUNT; ++port)
  {
    BigPictureInput::MenuController controller(port);
    controller.LoadConfig(ini.GetOrCreateSection(controller.GetName()));
    EXPECT_EQ(controller.GetButtons()->controls[0]->control_ref->GetExpression(),
              "`Button " + std::to_string(port) + "`");
  }
}
