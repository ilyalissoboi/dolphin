// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <imgui.h>

namespace VideoCommon::BigPicture::Style
{
// Independently authored fullscreen controls, measured in 1280 x 720 reference pixels.
// Keep these independent of the diagnostic OSD's font and color preferences.
constexpr float TITLE_SIZE = 22;
constexpr float SUMMARY_SIZE = 14;
constexpr float HEADER_HEIGHT = 48;
constexpr float FOOTER_HEIGHT = 34;
constexpr float ROW_HEIGHT = 70;
constexpr float SHORT_ROW_HEIGHT = 46;
constexpr float PADDING = 15;
constexpr float GAP = 10;
constexpr float ROUNDING = 6;

struct Palette
{
  ImVec4 background{0.129f, 0.129f, 0.129f, 1};
  ImVec4 header{0.180f, 0.180f, 0.180f, 1};
  ImVec4 panel{0.075f, 0.075f, 0.075f, 1};
  ImVec4 text{1, 1, 1, 1};
  ImVec4 secondary{0.72f, 0.72f, 0.72f, 1};
  ImVec4 hover{0.294f, 0.294f, 0.294f, 1};
  ImVec4 selected{0.051f, 0.278f, 0.631f, 1};
  ImVec4 focus{0.388f, 0.643f, 1, 1};
};
constexpr Palette DARK;
}  // namespace VideoCommon::BigPicture::Style
