// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/BigPicture/SettingsMenu.h"

#include <string_view>

namespace VideoCommon::BigPicture
{
InGameMenu BuildSettingsMenu(const ::BigPicture::Snapshot& snapshot,
                             const ::BigPicture::NavigationText& text,
                             ::BigPicture::NavigationOptions options)
{
  using namespace ::BigPicture;
  const auto& view = *snapshot.host.settings_menu;
  InGameMenu menu;
  menu.page.id = view.id;
  menu.page.title = view.title;
  menu.page.description = view.description;
  menu.page.default_focus = view.default_focus;
  menu.page.overlay = snapshot.mode == Mode::InGame;
  const auto add = [&](const SettingsRow& row, bool toolbar) {
    (toolbar ? menu.page.toolbar : menu.page.items)
        .push_back(
            {row.id, row.label, row.detail, row.enabled && !snapshot.pending, {}, row.active});
    if (!row.enabled || snapshot.pending)
      return;
    NavigationRequest request{snapshot.generation, Action::SettingsMenu};
    request.settings_menu = row.navigation;
    if (row.edit)
    {
      request.action = Action::EditSetting;
      request.setting = row.edit;
    }
    menu.requests.emplace(row.id, std::move(request));
  };
  for (const auto& row : view.toolbar)
    add(row, true);
  for (const auto& row : view.rows)
    add(row, false);
  if (view.preferences && !snapshot.pending)
  {
    menu.page.items.push_back({"swap", text.swap, options.swap_confirm_back ? text.on : text.off});
    menu.page.items.push_back(
        {"scale", text.scale, std::to_string(static_cast<int>(options.scale * 100)) + "%"});
    menu.page.items.push_back({"safe_area", text.safe_area,
                               std::to_string(static_cast<int>(options.safe_area * 100)) + "%"});
    for (const auto* id : {"swap", "scale", "safe_area"})
    {
      auto changed = options;
      if (std::string_view(id) == "swap")
        changed.swap_confirm_back = !changed.swap_confirm_back;
      else if (std::string_view(id) == "scale")
        changed.scale = changed.scale >= 1.5f ? 0.75f : changed.scale + 0.25f;
      else
        changed.safe_area = changed.safe_area >= 0.149f ? 0 : changed.safe_area + 0.05f;
      menu.requests.emplace(id, NavigationRequest{snapshot.generation, Action::Back, changed});
    }
  }
  NavigationRequest back{snapshot.generation, Action::Back};
  if (view.back)
  {
    back.action = Action::SettingsMenu;
    back.settings_menu = view.back;
  }
  if (!snapshot.pending)
  {
    menu.page.items.push_back({"back", text.back});
    menu.requests.emplace("back", std::move(back));
    menu.page.items.push_back({"desktop-settings",
                               text.actions[static_cast<size_t>(Action::DesktopSettings)],
                               text.desktop_settings_help});
    menu.requests.emplace("desktop-settings",
                          NavigationRequest{snapshot.generation, Action::DesktopSettings});
  }
  return menu;
}
}  // namespace VideoCommon::BigPicture
