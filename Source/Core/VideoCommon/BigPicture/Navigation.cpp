// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/BigPicture/Navigation.h"

#include <algorithm>
#include <cmath>

#include <imgui_internal.h>

namespace VideoCommon::BigPicture
{
MenuLayout ComputeMenuLayout(ImVec2 viewport, ::BigPicture::NavigationOptions options, bool dialog)
{
  options = ::BigPicture::SanitizeNavigationOptions(options);
  viewport.x = std::isfinite(viewport.x) ? std::max(1.0f, viewport.x) : 1.0f;
  viewport.y = std::isfinite(viewport.y) ? std::max(1.0f, viewport.y) : 1.0f;
  const float scale = std::min(viewport.x / 1280.0f, viewport.y / 720.0f) * options.scale;
  ImVec2 size(viewport.x * (1.0f - options.safe_area * 2.0f),
              viewport.y * (1.0f - options.safe_area * 2.0f));
  if (dialog)
  {
    size.x = std::min(size.x, 800.0f * scale);
    size.y = std::min(size.y, 500.0f * scale);
  }
  return {{(viewport.x - size.x) * 0.5f, (viewport.y - size.y) * 0.5f}, size, scale};
}

ImGuiKey GetNavigationKey(::BigPicture::NavButton button, bool swap_confirm_back)
{
  using ::BigPicture::NavButton;
  switch (button)
  {
  case NavButton::Up:
    return ImGuiKey_GamepadDpadUp;
  case NavButton::Down:
    return ImGuiKey_GamepadDpadDown;
  case NavButton::Left:
    return ImGuiKey_GamepadDpadLeft;
  case NavButton::Right:
    return ImGuiKey_GamepadDpadRight;
  case NavButton::Confirm:
    return swap_confirm_back ? ImGuiKey_GamepadFaceRight : ImGuiKey_GamepadFaceDown;
  case NavButton::Back:
    return swap_confirm_back ? ImGuiKey_GamepadFaceDown : ImGuiKey_GamepadFaceRight;
  case NavButton::Previous:
    return ImGuiKey_GamepadL1;
  case NavButton::Next:
    return ImGuiKey_GamepadR1;
  default:
    return ImGuiKey_None;
  }
}

void NavigationToolkit::Reset()
{
  m_current_page.clear();
  m_memory.clear();
  m_restore_frames = 0;
}

FocusMemory NavigationToolkit::GetMemory(const std::string& page) const
{
  const auto it = m_memory.find(page);
  return it != m_memory.end() ? it->second : FocusMemory{};
}

MenuResult NavigationToolkit::Draw(const MenuPage& page, ::BigPicture::NavigationOptions options,
                                   const std::string& prompts, ImFont* font,
                                   const std::function<ImTextureID(const std::string&)>& image)
{
  MenuResult result;
  const auto layout = ComputeMenuLayout(ImGui::GetIO().DisplaySize, options, page.dialog);
  const float scale = layout.scale;
  // DisplaySize is already in native pixels. Diagnostic OSD/DPI font scaling must not
  // multiply the menu's separately scaled geometry a second time.
  const float font_scale = ImGui::GetStyle().FontScaleMain * ImGui::GetStyle().FontScaleDpi;
  auto& memory = m_memory[page.id];
  const auto previous_focus = memory.item;
  const bool revision_changed = m_focus_revision != page.focus_revision;
  const bool entering = m_current_page != page.id || m_focus_revision != page.focus_revision ||
                        m_layout_width != layout.size.x || m_layout_height != layout.size.y ||
                        m_layout_scale != scale;
  m_current_page = page.id;
  m_focus_revision = page.focus_revision;
  // Destructive dialogs always start at their safe default, including another request using
  // the same page. Reuse one memory entry rather than accumulating entries per command ID.
  if (entering && (page.dialog || (page.reset_on_revision && revision_changed)))
    memory = {};
  m_layout_width = layout.size.x;
  m_layout_height = layout.size.y;
  m_layout_scale = scale;
  auto focus = std::ranges::find_if(page.items, [&](const MenuItem& item) {
    return item.enabled && item.id == (memory.item.empty() ? page.default_focus : memory.item);
  });
  bool toolbar_focus = std::ranges::any_of(
      page.toolbar, [&](const auto& item) { return item.enabled && item.id == memory.item; });
  if (focus == page.items.end() && !toolbar_focus)
  {
    toolbar_focus = std::ranges::any_of(page.toolbar, [&](const auto& item) {
      return item.enabled && item.id == page.default_focus;
    });
    if (toolbar_focus)
      memory.item = page.default_focus;
    focus = std::ranges::find_if(page.items, [&](const auto& item) {
      return item.enabled && item.id == page.default_focus;
    });
    if (focus == page.items.end() && !toolbar_focus)
      focus = std::ranges::find_if(page.items, &MenuItem::enabled);
  }
  if (focus != page.items.end())
    memory.item = focus->id;
  else if (!toolbar_focus)
    memory.item.clear();
  const auto focus_id = memory.item;
  const bool restore_focus = entering || previous_focus != focus_id;
  if (entering)
  {
    m_restore_frames = !previous_focus.empty() && previous_focus == focus_id ? 3 : 0;
    m_restore_scroll = memory.scroll;
  }
  for (const auto key : {ImGuiKey_UpArrow, ImGuiKey_DownArrow, ImGuiKey_LeftArrow,
                         ImGuiKey_RightArrow, ImGuiKey_GamepadDpadUp, ImGuiKey_GamepadDpadDown,
                         ImGuiKey_GamepadDpadLeft, ImGuiKey_GamepadDpadRight, ImGuiKey_GamepadL1,
                         ImGuiKey_GamepadR1, ImGuiKey_PageUp, ImGuiKey_PageDown})
  {
    if (ImGui::IsKeyPressed(key))
      m_restore_frames = 0;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24 * scale, 20 * scale));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12 * scale, 12 * scale));
  ImGui::PushFont(font, 24 * scale / font_scale);
  ImGui::SetNextWindowPos(layout.position);
  ImGui::SetNextWindowSize(layout.size);
  ImGui::SetNextWindowBgAlpha(0.97f);
  if (entering)
    ImGui::SetNextWindowFocus();
  ImGui::Begin("###BigPictureNavigation", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings);
  ImGui::PushFont(font, 32 * scale / font_scale);
  ImGui::TextWrapped("%s", page.title.c_str());
  ImGui::PopFont();
  if (!page.description.empty())
    ImGui::TextWrapped("%s", page.description.c_str());
  ImGui::Separator();
  if (!page.toolbar.empty())
  {
    ImGui::PushFont(font, 18 * scale / font_scale);
    const float toolbar_width = (ImGui::GetContentRegionAvail().x -
                                 static_cast<float>(page.toolbar.size() - 1) * 12 * scale) /
                                page.toolbar.size();
    for (const auto& item : page.toolbar)
    {
      if (&item != &page.toolbar.front())
        ImGui::SameLine();
      ImGui::PushID(item.id.c_str());
      if (restore_focus && item.id == focus_id)
        ImGui::SetKeyboardFocusHere();
      ImGui::BeginDisabled(!item.enabled);
      const bool clicked = ImGui::Button(item.label.c_str(), ImVec2(toolbar_width, 42 * scale));
      if (item.enabled && (clicked || ImGui::IsItemFocused()))
        memory.item = item.id;
      if (clicked)
        result.activated = item.id;
      ImGui::EndDisabled();
      ImGui::PopID();
    }
    ImGui::PopFont();
  }
  const float footer_height = 64 * scale;
  if (entering || m_restore_frames != 0)
    ImGui::SetNextWindowScroll(
        ImVec2(0, (m_restore_frames ? m_restore_scroll : memory.scroll) * scale));
  ImGui::BeginChild(page.id.c_str(), ImVec2(0, -footer_height),
                    page.toolbar.empty() ? ImGuiChildFlags_None : ImGuiChildFlags_NavFlattened,
                    ImGuiWindowFlags_AlwaysVerticalScrollbar);

  const bool previous =
      ImGui::IsKeyPressed(ImGuiKey_GamepadL1) || ImGui::IsKeyPressed(ImGuiKey_PageUp);
  const bool next =
      ImGui::IsKeyPressed(ImGuiKey_GamepadR1) || ImGui::IsKeyPressed(ImGuiKey_PageDown);
  const int columns =
      page.artwork && page.columns > 1 ?
          std::clamp(static_cast<int>(ImGui::GetContentRegionAvail().x / (220 * scale)), 1, 6) :
          std::clamp(page.columns, 1, 6);
  std::string requested_focus;
  if (focus != page.items.end() && (previous || next))
  {
    const int current = static_cast<int>(focus - page.items.begin());
    const float row_height = (page.compact ? 48 :
                              page.artwork ? (columns == 1 ? 96 : 280) :
                                             76) *
                             scale;
    const int step = std::max(1, static_cast<int>(ImGui::GetWindowHeight() / row_height)) * columns;
    int target =
        std::clamp(current + (next ? step : -step), 0, static_cast<int>(page.items.size()) - 1);
    while (target >= 0 && target < static_cast<int>(page.items.size()))
    {
      if (page.items[target].enabled)
      {
        requested_focus = page.items[target].id;
        break;
      }
      target += next ? 1 : -1;
    }
  }
  const float width = (ImGui::GetContentRegionAvail().x - (columns - 1) * 12 * scale) / columns;
  for (std::size_t i = 0; i < page.items.size(); ++i)
  {
    const auto& item = page.items[i];
    if (i % columns != 0)
      ImGui::SameLine();
    const float artwork_height = page.artwork ? (columns == 1 ? 72 : 144) * scale : 0;
    const float artwork_width = artwork_height * 2 / 3;
    const float text_offset = page.artwork && columns == 1 ? artwork_width + 24 * scale : 0;
    const float text_top = page.artwork && columns > 1 ? artwork_height + 12 * scale : 0;
    const float wrap_width = std::max(scale, width - 32 * scale - text_offset);
    const float label_height =
        ImGui::CalcTextSize(item.label.c_str(), nullptr, false, wrap_width).y;
    const float detail_height =
        item.detail.empty() ?
            0 :
            ImGui::CalcTextSize(item.detail.c_str(), nullptr, false, wrap_width).y + 8 * scale;
    const float height = std::max((page.compact ? 40 :
                                   page.artwork ? 96 :
                                   columns == 1 ? 64 :
                                                  150) *
                                      scale,
                                  text_top + label_height + detail_height + 24 * scale);
    ImGui::PushID(item.id.c_str());
    const bool request_focus = (restore_focus && item.id == focus_id) || item.id == requested_focus;
    if (request_focus && item.enabled)
      ImGui::SetKeyboardFocusHere();
    ImGui::BeginDisabled(!item.enabled);
    const bool clicked = ImGui::Selectable("##item", item.enabled && item.id == memory.item,
                                           ImGuiSelectableFlags_None, ImVec2(width, height));
    if (item.enabled && (ImGui::IsItemFocused() || clicked))
      memory.item = item.id;
    if (request_focus && !requested_focus.empty())
      ImGui::SetScrollHereY();
    const auto min = ImGui::GetItemRectMin();
    const auto max = ImGui::GetItemRectMax();
    if ((m_restore_frames && item.id == focus_id) ||
        (item.id == memory.item && memory.item != previous_focus))
    {
      // Wrapping can change row heights when aspect ratio or safe area changes. Retain
      // the saved offset when it still shows the selected control; otherwise reveal it.
      const float top = ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y;
      const float bottom =
          ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - ImGui::GetStyle().WindowPadding.y;
      if (min.y < top || max.y > bottom)
      {
        const float delta = min.y < top ? min.y - top : max.y - bottom;
        m_restore_scroll = std::max(0.0f, (ImGui::GetScrollY() + delta) / scale);
        ImGui::SetScrollY(m_restore_scroll * scale);
      }
    }
    if (clicked && item.enabled)
      result.activated = item.id;
    // Selectable still participates in navigation/scroll restoration when clipped. Custom
    // rounded backgrounds and text must not allocate geometry for thousands of hidden rows.
    if (!ImGui::IsItemVisible())
    {
      ImGui::EndDisabled();
      ImGui::PopID();
      continue;
    }
    auto* draw = ImGui::GetWindowDrawList();
    if (item.id != memory.item && !ImGui::IsItemHovered() && !ImGui::IsItemFocused())
      draw->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg), 4 * scale);
    draw->PushClipRect(min, max, true);
    if (page.artwork && max.y >= ImGui::GetWindowPos().y &&
        min.y <= ImGui::GetWindowPos().y + ImGui::GetWindowHeight())
    {
      const ImVec2 image_min(min.x + (columns == 1 ? 12 * scale : (width - artwork_width) * 0.5f),
                             min.y + 8 * scale);
      const ImVec2 image_max(image_min.x + artwork_width, image_min.y + artwork_height);
      const auto texture = image && !item.image_key.empty() ? image(item.image_key) : ImTextureID{};
      if (texture)
        draw->AddImage(texture, image_min, image_max);
      else
        draw->AddRectFilled(image_min, image_max, IM_COL32(50, 60, 80, 255), 4 * scale);
    }
    draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                  ImVec2(min.x + 16 * scale + text_offset, min.y + 12 * scale + text_top),
                  ImGui::GetColorU32(ImGuiCol_Text), item.label.c_str(), nullptr, wrap_width);
    if (!item.detail.empty())
      draw->AddText(
          ImGui::GetFont(), ImGui::GetFontSize(),
          ImVec2(min.x + 16 * scale + text_offset, min.y + 20 * scale + label_height + text_top),
          ImGui::GetColorU32(item.id == memory.item ? ImGuiCol_Text : ImGuiCol_TextDisabled),
          item.detail.c_str(), nullptr, wrap_width);
    draw->PopClipRect();
    ImGui::EndDisabled();
    ImGui::PopID();
  }
  // List menus loop in both directions, including when the opposite end is clipped.
  // Grids retain directional navigation between cards and their toolbar.
  if (columns == 1 && page.toolbar.empty())
    ImGui::NavMoveRequestTryWrapping(ImGui::GetCurrentWindow(), ImGuiNavMoveFlags_LoopY);
  memory.scroll = m_restore_frames ? m_restore_scroll : ImGui::GetScrollY() / scale;
  if (m_restore_frames)
    --m_restore_frames;
  ImGui::EndChild();
  ImGui::PushFont(font, 18 * scale / font_scale);
  ImGui::TextWrapped("%s", prompts.c_str());
  ImGui::PopFont();
  result.back = ImGui::IsKeyPressed(ImGuiKey_Escape, false) ||
                ImGui::IsKeyPressed(ImGuiKey_GamepadFaceRight, false);
  result.focused = memory.item;
  ImGui::End();
  ImGui::PopFont();
  ImGui::PopStyleVar(2);
  return result;
}
}  // namespace VideoCommon::BigPicture
