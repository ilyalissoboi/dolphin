// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <vector>

#include <QPushButton>
#include <gtest/gtest.h>

#include "DolphinQt/Settings/BigPicturePane.h"

TEST(BigPicturePaneTest, ConfigureButtonsRequestTheMatchingInputEditor)
{
  BigPicturePane pane;
  std::vector<int> requests;
  QObject::connect(&pane, &BigPicturePane::ConfigureInput, &pane,
                   [&](int controller) { requests.push_back(controller); });

  for (int controller = 0; controller < 4; ++controller)
  {
    auto* button = pane.findChild<QPushButton*>(
        QStringLiteral("configureController%1Button").arg(controller + 1));
    ASSERT_NE(button, nullptr);
    button->click();
  }
  EXPECT_EQ(requests, (std::vector<int>{0, 1, 2, 3}));
}
