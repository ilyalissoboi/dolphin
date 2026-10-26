// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/BigPicture/Navigation.h"
#include "VideoCommon/BigPicture/Style.h"

#include <algorithm>
#include <cmath>

#include <imgui_internal.h>

namespace VideoCommon::BigPicture
{
namespace
{
ImU32 Color(const ImVec4& color)
{
  return ImGui::GetColorU32(color);
}

void DrawCover(ImDrawList* draw, ImVec2 min, ImVec2 size, ImTextureID texture, float scale)
{
  const ImVec2 max(min.x + size.x, min.y + size.y);
  if (texture)
    draw->AddImage(texture, min, max);
  else
  {
    draw->AddRectFilled(min, max, Color(Style::DARK.header), Style::ROUNDING * scale);
    // A neutral disc motif also works before asynchronous cover loading completes.
    const ImVec2 center(min.x + size.x * 0.5f, min.y + size.y * 0.5f);
    const float radius = std::min(size.x, size.y) * 0.28f;
    draw->AddCircle(center, radius, Color(Style::DARK.secondary), 32, 2 * scale);
    draw->AddCircle(center, radius * 0.18f, Color(Style::DARK.secondary), 16, 2 * scale);
  }
}
}  // namespace

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
    size.y = std::min(size.y, 320.0f * scale);
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

  const auto& palette = Style::DARK;
  const float padding = Style::PADDING * scale;
  const float gap = Style::GAP * scale;
  const float title_size = Style::TITLE_SIZE * scale;
  const float summary_size = Style::SUMMARY_SIZE * scale;
  const bool pause = page.presentation == MenuPresentation::Pause;
  const bool landing = page.presentation == MenuPresentation::Landing;
  const bool library_list = page.presentation == MenuPresentation::Library && page.columns == 1 &&
                            !page.items.empty() && layout.size.x >= 850 * scale;
  auto* background = ImGui::GetBackgroundDrawList();
  const ImVec2 display = ImGui::GetIO().DisplaySize;
  background->AddRectFilled(ImVec2(0, 0), display,
                            pause || page.overlay || page.dialog ? IM_COL32(25, 25, 25, 200) :
                                                                   Color(palette.background));
  ImGui::PushStyleColor(ImGuiCol_WindowBg, palette.background);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_Text, palette.text);
  ImGui::PushStyleColor(ImGuiCol_TextDisabled, palette.secondary);
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_NavCursor, palette.focus);
  ImGui::PushStyleColor(ImGuiCol_Button, palette.header);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, palette.hover);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, palette.selected);
  ImGui::PushStyleColor(ImGuiCol_Border, palette.hover);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, palette.panel);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, palette.hover);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, palette.secondary);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, palette.focus);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(gap, pause ? 0 : gap));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, page.dialog ? 8 * scale : 0);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Style::ROUNDING * scale);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(gap, gap));
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10 * scale);
  ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, page.dialog ? scale : 0);
  ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
  ImGui::PushFont(font, title_size / font_scale);
  ImGui::SetNextWindowPos(layout.position);
  ImGui::SetNextWindowSize(layout.size);
  ImGui::SetNextWindowBgAlpha(pause ? 0.0f : 1.0f);
  if (entering)
    ImGui::SetNextWindowFocus();
  ImGui::Begin("###BigPictureNavigation", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse);
  auto* window_draw = ImGui::GetWindowDrawList();
  const float header_height = (pause ? 110 : Style::HEADER_HEIGHT) * scale;
  const ImVec2 header_max(layout.position.x + layout.size.x, layout.position.y + header_height);
  if (!pause)
    window_draw->AddRectFilled(layout.position, header_max, Color(palette.header),
                               page.dialog ? 8 * scale : 0);
  ImGui::SetCursorPos(ImVec2(padding, gap));
  if (pause && !page.context.label.empty())
  {
    DrawCover(window_draw, ImVec2(layout.position.x + padding, layout.position.y + gap),
              ImVec2(60 * scale, 90 * scale),
              image && !page.context.image_key.empty() ? image(page.context.image_key) :
                                                         ImTextureID{},
              scale);
    const ImVec2 text_min(layout.position.x + padding + 80 * scale, layout.position.y + gap);
    window_draw->PushClipRect(text_min, header_max, true);
    window_draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(), text_min, Color(palette.text),
                         page.context.label.c_str(), nullptr,
                         std::max(scale, layout.size.x - padding * 2 - 80 * scale));
    window_draw->PopClipRect();
  }
  else
    ImGui::TextUnformatted(page.title.c_str());
  ImGui::SetCursorPos(ImVec2(padding, header_height + gap));
  if (!page.toolbar.empty())
  {
    ImGui::PushFont(font, summary_size / font_scale);
    const float toolbar_width =
        (ImGui::GetContentRegionAvail().x - static_cast<float>(page.toolbar.size() - 1) * gap) /
        page.toolbar.size();
    float toolbar_height = Style::SHORT_ROW_HEIGHT * scale;
    for (const auto& item : page.toolbar)
      toolbar_height =
          std::max(toolbar_height, ImGui::CalcTextSize(item.label.c_str(), nullptr, false,
                                                       std::max(scale, toolbar_width - gap * 2))
                                           .y +
                                       gap * 2);
    for (const auto& item : page.toolbar)
    {
      if (&item != &page.toolbar.front())
        ImGui::SameLine();
      ImGui::PushID(item.id.c_str());
      if (restore_focus && item.id == focus_id)
        ImGui::SetKeyboardFocusHere();
      ImGui::BeginDisabled(!item.enabled);
      if (item.active)
        ImGui::PushStyleColor(ImGuiCol_Button, palette.selected);
      const bool clicked = ImGui::Button("##toolbar", ImVec2(toolbar_width, toolbar_height));
      if (item.active)
        ImGui::PopStyleColor();
      const auto min = ImGui::GetItemRectMin();
      const auto max = ImGui::GetItemRectMax();
      window_draw->PushClipRect(min, max, true);
      window_draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(min.x + gap, min.y + gap),
                           Color(palette.text), item.label.c_str(), nullptr,
                           std::max(scale, toolbar_width - gap * 2));
      window_draw->PopClipRect();
      if (item.enabled && (clicked || ImGui::IsItemFocused()))
        memory.item = item.id;
      if (clicked)
        result.activated = item.id;
      ImGui::EndDisabled();
      ImGui::PopID();
    }
    ImGui::PopFont();
  }
  // The prompt strip is outside the scrolling region, including on small/high-scale displays.
  ImGui::PushFont(font, summary_size / font_scale);
  const float footer_height =
      std::max(Style::FOOTER_HEIGHT * scale,
               ImGui::CalcTextSize(prompts.c_str(), nullptr, false,
                                   std::max(scale, layout.size.x - padding * 2))
                       .y +
                   gap * 2);
  ImGui::PopFont();
  const float content_top = ImGui::GetCursorPosY();
  const float content_height = std::max(scale, layout.size.y - content_top - footer_height - gap);
  const float content_width =
      std::max(scale, ImGui::GetContentRegionAvail().x - (library_list ? 440 * scale : 0));
  if (entering || m_restore_frames != 0)
    ImGui::SetNextWindowScroll(
        ImVec2(0, (m_restore_frames ? m_restore_scroll : memory.scroll) * scale));
  ImGui::BeginChild(
      page.id.c_str(),
      ImVec2(pause ? std::min(content_width, 500 * scale) : content_width, content_height),
      page.toolbar.empty() ? ImGuiChildFlags_None : ImGuiChildFlags_NavFlattened,
      ImGuiWindowFlags_AlwaysVerticalScrollbar);
  if (!page.description.empty())
  {
    ImGui::PushFont(font, summary_size / font_scale);
    ImGui::PushStyleColor(ImGuiCol_Text, palette.secondary);
    ImGui::TextWrapped("%s", page.description.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Spacing();
  }

  const bool previous =
      ImGui::IsKeyPressed(ImGuiKey_GamepadL1) || ImGui::IsKeyPressed(ImGuiKey_PageUp);
  const bool next =
      ImGui::IsKeyPressed(ImGuiKey_GamepadR1) || ImGui::IsKeyPressed(ImGuiKey_PageDown);
  const int columns =
      page.artwork && page.columns > 1 ?
          std::clamp(static_cast<int>(ImGui::GetContentRegionAvail().x / (210 * scale)), 1, 6) :
          std::clamp(page.columns, 1, 6);
  std::string requested_focus;
  if (focus != page.items.end() && (previous || next))
  {
    const int current = static_cast<int>(focus - page.items.begin());
    const float row_height = (page.compact ? 48 :
                              page.artwork ? (columns == 1 ? 90 : 310) :
                                             Style::ROW_HEIGHT + Style::GAP) *
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
  const float width =
      std::max(scale, (ImGui::GetContentRegionAvail().x - (columns - 1) * gap) / columns);
  for (std::size_t i = 0; i < page.items.size(); ++i)
  {
    const auto& item = page.items[i];
    if (i % columns != 0)
      ImGui::SameLine();
    const float artwork_height =
        page.artwork ? (columns == 1 ? 70 * scale : std::min(210 * scale, width * 1.05f)) : 0;
    const float artwork_width = artwork_height * 2 / 3;
    const float text_offset = page.artwork && columns == 1 ? artwork_width + gap : 0;
    const float text_top = page.artwork && columns > 1 ? artwork_height + 12 * scale : 0;
    const float wrap_width = std::max(scale, width - padding * 2 - text_offset);
    const float label_height =
        ImGui::CalcTextSize(item.label.c_str(), nullptr, false, wrap_width).y;
    ImGui::PushFont(font, summary_size / font_scale);
    const float detail_height =
        item.detail.empty() ?
            0 :
            ImGui::CalcTextSize(item.detail.c_str(), nullptr, false, wrap_width).y + 4 * scale;
    ImGui::PopFont();
    // Fixed grid cells keep all subsequent rows aligned even with long translated titles.
    const float height = columns > 1 ? (page.compact ? 40 * scale :
                                        page.artwork ? text_top + 90 * scale :
                                                       (landing ? 220 : 150) * scale) :
                                       std::max((page.compact        ? 40 :
                                                 page.artwork        ? 90 :
                                                 item.detail.empty() ? Style::SHORT_ROW_HEIGHT :
                                                                       Style::ROW_HEIGHT) *
                                                    scale,
                                                label_height + detail_height + gap * 2);
    ImGui::PushID(item.id.c_str());
    const bool request_focus = (restore_focus && item.id == focus_id) || item.id == requested_focus;
    if (request_focus && item.enabled)
      ImGui::SetKeyboardFocusHere();
    ImGui::BeginDisabled(!item.enabled);
    const bool clicked =
        ImGui::Selectable("##item", item.enabled && item.id == memory.item,
                          ImGuiSelectableFlags_NoPadWithHalfSpacing, ImVec2(width, height));
    if (item.enabled && (ImGui::IsItemFocused() || clicked))
      memory.item = item.id;
    if (request_focus && !requested_focus.empty())
      ImGui::SetScrollHereY();
    const auto min = ImGui::GetItemRectMin();
    const auto max = ImGui::GetItemRectMax();
    if ((m_restore_frames == 1 && item.id == focus_id) ||
        (item.id == memory.item && memory.item != previous_focus))
    {
      // On the first resized frame ImGui clamps to the previous content size. Wait for
      // layout to settle before deciding whether wrapping requires revealing the control.
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
    const bool selected = item.enabled && item.id == memory.item;
    const bool hovered = ImGui::IsItemHovered();
    if (selected || hovered || columns > 1)
      draw->AddRectFilled(min, max,
                          Color(selected ? palette.selected :
                                hovered  ? palette.hover :
                                           palette.header),
                          Style::ROUNDING * scale);
    if (selected)
      draw->AddRect(min, max, Color(palette.focus), Style::ROUNDING * scale, 0, 2 * scale);
    draw->PushClipRect(min, max, true);
    if (page.artwork && max.y >= ImGui::GetWindowPos().y &&
        min.y <= ImGui::GetWindowPos().y + ImGui::GetWindowHeight())
    {
      const ImVec2 image_min(min.x + (columns == 1 ? 12 * scale : (width - artwork_width) * 0.5f),
                             min.y + 8 * scale);
      const auto texture = image && !item.image_key.empty() ? image(item.image_key) : ImTextureID{};
      DrawCover(draw, image_min, ImVec2(artwork_width, artwork_height), texture, scale);
    }
    const float title_height = columns > 1 ? std::min(label_height, title_size * 2) : label_height;
    draw->PushClipRect(min, ImVec2(max.x, min.y + gap + text_top + title_height), true);
    draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                  ImVec2(min.x + padding + text_offset, min.y + gap + text_top),
                  ImGui::GetColorU32(ImGuiCol_Text), item.label.c_str(), nullptr, wrap_width);
    draw->PopClipRect();
    if (!item.detail.empty())
    {
      ImGui::PushFont(font, summary_size / font_scale);
      // Grid titles have two lines; clipping prevents unusually long titles covering summaries.
      const float detail_top = min.y + gap + text_top + title_height + 4 * scale;
      draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                    ImVec2(min.x + padding + text_offset, detail_top),
                    Color(selected ? palette.text : palette.secondary), item.detail.c_str(),
                    nullptr, wrap_width);
      ImGui::PopFont();
    }
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
  if (library_list)
  {
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, palette.panel);
    ImGui::BeginChild("##game-details", ImVec2(0, content_height), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus);
    const auto selected = std::ranges::find(page.items, memory.item, &MenuItem::id);
    if (selected != page.items.end())
    {
      const float available = ImGui::GetContentRegionAvail().x;
      const float cover_height = std::min(240 * scale, content_height * 0.5f);
      const ImVec2 cover_size(cover_height * 2 / 3, cover_height);
      const auto pos = ImGui::GetCursorScreenPos();
      DrawCover(ImGui::GetWindowDrawList(),
                ImVec2(pos.x + (available - cover_size.x) / 2, pos.y + gap), cover_size,
                image && !selected->image_key.empty() ? image(selected->image_key) : ImTextureID{},
                scale);
      ImGui::Dummy(ImVec2(0, cover_height + gap * 2));
      ImGui::TextWrapped("%s", selected->label.c_str());
      ImGui::PushFont(font, summary_size / font_scale);
      ImGui::TextWrapped("%s", selected->detail.c_str());
      ImGui::PopFont();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }
  const ImVec2 footer_min(layout.position.x, layout.position.y + layout.size.y - footer_height);
  window_draw->AddRectFilled(footer_min,
                             ImVec2(footer_min.x + layout.size.x, footer_min.y + footer_height),
                             Color(palette.panel));
  ImGui::SetCursorScreenPos(ImVec2(footer_min.x + padding, footer_min.y + gap));
  ImGui::PushFont(font, summary_size / font_scale);
  window_draw->PushClipRect(
      footer_min, ImVec2(footer_min.x + layout.size.x, footer_min.y + footer_height), true);
  window_draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImGui::GetCursorScreenPos(),
                       Color(palette.text), prompts.c_str(), nullptr,
                       std::max(scale, layout.size.x - padding * 2));
  window_draw->PopClipRect();
  ImGui::PopFont();
  ImGui::Dummy(ImVec2(0, 0));
  result.back = ImGui::IsKeyPressed(ImGuiKey_Escape, false) ||
                ImGui::IsKeyPressed(ImGuiKey_GamepadFaceRight, false);
  result.focused = memory.item;
  ImGui::End();
  ImGui::PopFont();
  ImGui::PopStyleVar(10);
  ImGui::PopStyleColor(16);
  return result;
}
}  // namespace VideoCommon::BigPicture
