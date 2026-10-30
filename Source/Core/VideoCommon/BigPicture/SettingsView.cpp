// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/BigPicture/Navigation.h"

#include <algorithm>
#include <cmath>

#include <imgui_internal.h>

#include "VideoCommon/BigPicture/Style.h"

namespace VideoCommon::BigPicture
{
MenuResult NavigationToolkit::DrawSettings(const MenuPage& page,
                                           ::BigPicture::NavigationOptions options,
                                           const std::string& prompts, ImFont* font)
{
  MenuResult result;
  options = ::BigPicture::SanitizeNavigationOptions(options);
  const auto display = ImGui::GetIO().DisplaySize;
  const float scale = std::max(0.01f, std::min(display.x / 1280, display.y / 720) * options.scale);
  const float font_scale = ImGui::GetStyle().FontScaleMain * ImGui::GetStyle().FontScaleDpi;
  const auto& palette = Style::GetPalette(page.theme);
  const float header = Style::BAR_HEIGHT * scale;
  const float footer = Style::FOOTER_HEIGHT * scale;
  const float margin = std::max(12 * scale, display.x * options.safe_area);
  const float top = std::max(header, display.y * options.safe_area);
  const float bottom =
      std::min(display.y - footer, display.y * (1 - options.safe_area)) - 16 * scale;
  const float content_width = std::max(scale, display.x - margin * 2);
  auto& memory = m_memory[page.id];
  const auto row = [&](const std::string& id) {
    return std::ranges::find_if(
        page.items, [&](const auto& item) { return item.id == id && (item.enabled || page.busy); });
  };
  const auto header_item = [&](const std::string& id) {
    return std::ranges::any_of(page.toolbar, [&](const auto& item) {
      return item.id == id && (item.enabled || page.busy);
    });
  };
  const auto previous = memory.item;
  if (row(memory.item) == page.items.end() && !header_item(memory.item))
  {
    auto first = row(page.default_focus);
    if (first == page.items.end())
      first = std::ranges::find_if(page.items, &MenuItem::enabled);
    memory.item = first == page.items.end() ? "back" : first->id;
  }
  if (row(memory.selected) == page.items.end())
    memory.selected = memory.item;
  const auto focus_id = memory.item;
  const bool entering = TrackLayout(page.id, display, scale).Any() || previous != memory.item ||
                        GImGui->NavWindow == nullptr || (m_settings_busy && !page.busy);
  m_settings_busy = page.busy;

  const Style::ScopedMenuStyle style(
      palette, {.background = palette.background, .scrollbar_size = 12 * scale});
  ImGui::PushFont(font, Style::TITLE_SIZE * scale / font_scale);
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(display);
  if (entering)
    ImGui::SetNextWindowFocus();
  ImGui::Begin("###BigPictureSettings", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse);
  auto* draw = ImGui::GetWindowDrawList();
  draw->AddRectFilled(ImVec2(0, 0), ImVec2(display.x, header), ImGui::GetColorU32(palette.header));
  draw->AddRectFilled(ImVec2(0, display.y - footer), display, ImGui::GetColorU32(palette.header));
  const auto button = [&](const MenuItem& item, float x, float width) {
    ImGui::SetCursorPos(ImVec2(x, 0));
    ImGui::PushID(item.id.c_str());
    if (entering && focus_id == item.id)
      ImGui::SetKeyboardFocusHere();
    ImGui::BeginDisabled(!item.enabled);
    const bool pressed = ImGui::Button("##category", ImVec2(width, header));
    const float icon_size = std::min(24 * scale, width - 8 * scale);
    auto color = palette.header_text;
    if (!item.active && item.id != "back")
      color.w *= 0.55f;
    DrawMenuIcon(draw, item.icon,
                 ImVec2(x + (width - icon_size) * 0.5f, (header - icon_size) * 0.5f), icon_size,
                 ImGui::GetColorU32(color));
    if (item.active)
      draw->AddRectFilled(ImVec2(x + 10 * scale, header - 2 * scale),
                          ImVec2(x + width - 10 * scale, header),
                          ImGui::GetColorU32(palette.header_text));
    if (ImGui::IsItemFocused() && !entering)
      memory.item = item.id;
    if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
      ImGui::SetTooltip("%s%s%s", item.label.c_str(), item.detail.empty() ? "" : "\n",
                        item.detail.c_str());
    ImGui::EndDisabled();
    ImGui::PopID();
    return pressed;
  };
  std::vector<const MenuItem*> categories;
  std::vector<const MenuItem*> context;
  for (const auto& item : page.toolbar)
  {
    if (item.id == "back")
    {
      if (button(item, 5 * scale, 48 * scale))
        result.activated = item.id;
    }
    else if (item.id == "global" || item.id == "game")
      context.push_back(&item);
    else
      categories.push_back(&item);
  }
  const int count = static_cast<int>(categories.size() + context.size());
  const float tab_width = std::min(56 * scale, display.x * 0.68f / std::max(1, count));
  const float tabs_x = display.x - 8 * scale - count * tab_width;
  draw->PushClipRect(ImVec2(63 * scale, 0),
                     ImVec2(std::max(63 * scale, tabs_x - 10 * scale), header), true);
  draw->AddText(ImVec2(63 * scale, (header - ImGui::GetTextLineHeight()) * 0.5f),
                ImGui::GetColorU32(palette.header_text), page.title.c_str());
  draw->PopClipRect();
  float x = tabs_x;
  for (const auto* item : categories)
  {
    if (button(*item, x, tab_width))
      result.activated = item->id;
    x += tab_width;
  }
  for (const auto* item : context)
  {
    if (button(*item, x, tab_width))
      result.activated = item->id;
    x += tab_width;
  }
  ImGui::SetCursorPos(ImVec2(0, top));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 16 * scale));
  ImGui::BeginChild("##settings-rows", ImVec2(display.x, std::max(scale, bottom - top)),
                    ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AlwaysUseWindowPadding,
                    ImGuiWindowFlags_NoSavedSettings);
  ImGui::PopStyleVar();
  if (entering)
    ImGui::SetScrollY(memory.scroll * scale);
  ImGui::SetCursorPosX(margin + 16 * scale);
  // Revealing the first row also reveals the explanation above it.
  const float content_top = ImGui::GetCursorScreenPos().y;
  const auto context_end = page.description.find('\n');
  const auto explanation = !page.category_page ? page.description :
                           context_end == std::string::npos ?
                                                 std::string{} :
                                                 page.description.substr(context_end + 1);
  if (!explanation.empty())
  {
    ImGui::PushFont(font, Style::SUMMARY_SIZE * scale / font_scale);
    ImGui::PushTextWrapPos(display.x - margin - 16 * scale);
    ImGui::TextUnformatted(explanation.c_str());
    ImGui::PopTextWrapPos();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 12 * scale));
  }
  std::string section;
  for (const auto& item : page.items)
  {
    // Revealing a row that starts a section also reveals the section's heading.
    const bool first = &item == page.items.data();
    const bool heading = item.section != section && !item.section.empty();
    const float heading_top = first ? content_top : ImGui::GetCursorScreenPos().y;
    if (heading)
    {
      section = item.section;
      ImGui::SetCursorPosX(margin + 16 * scale);
      ImGui::Dummy(ImVec2(0, &item == page.items.data() ? 0 : 10 * scale));
      ImGui::SetCursorPosX(margin + 16 * scale);
      const auto min = ImGui::GetCursorScreenPos();
      auto* rows = ImGui::GetWindowDrawList();
      rows->AddText(min, ImGui::GetColorU32(palette.secondary), section.c_str());
      rows->AddLine(ImVec2(min.x, min.y + 28 * scale),
                    ImVec2(display.x - margin - 16 * scale, min.y + 28 * scale),
                    ImGui::GetColorU32(palette.secondary), scale);
      ImGui::Dummy(ImVec2(0, 34 * scale));
    }
    const float width = std::max(scale, content_width - 12 * scale);
    const float value_width = item.toggle        ? 62 * scale :
                              item.value.empty() ? 0 :
                                                   std::min(320 * scale, width * 0.32f);
    const float label_width = std::max(scale, width - 64 * scale - value_width);
    const auto title_size = ImGui::CalcTextSize(item.label.c_str(), nullptr, false, label_width);
    ImGui::PushFont(font, Style::SUMMARY_SIZE * scale / font_scale);
    const float help_width = std::max(scale, width - 32 * scale - value_width);
    // Keep long desktop tooltips from filling the viewport. The options editor retains the
    // complete description, including requirements and the current override state.
    std::string summary = item.detail;
    const char* line = summary.c_str();
    const char* end = line + summary.size();
    for (int i = 0; i < 3 && line < end; ++i)
    {
      const auto* paragraph_end = std::find(line, end, '\n');
      const float wrap_width =
          i == 2 ? std::max(scale, help_width - ImGui::CalcTextSize("...").x) : help_width;
      const auto* next = paragraph_end == line ?
                             line + 1 :
                             ImGui::GetFont()->CalcWordWrapPosition(ImGui::GetFontSize(), line,
                                                                    paragraph_end, wrap_width);
      if (next == line)
        break;
      if (next == paragraph_end && paragraph_end < end)
        ++next;
      line = next;
      while (line < end && (*line == ' ' || *line == '\r' || *line == '\t'))
        ++line;
      if (i == 2 && line < end)
      {
        summary.resize(static_cast<std::size_t>(line - summary.c_str()));
        while (!summary.empty() &&
               (summary.back() == '\n' || summary.back() == '\r' || summary.back() == ' '))
          summary.pop_back();
        summary += "...";
      }
    }
    const auto help_size = ImGui::CalcTextSize(summary.c_str(), nullptr, false, help_width);
    ImGui::PopFont();
    auto visible_value = item.value;
    if (visible_value.size() > 96)
    {
      std::size_t start = visible_value.size() - 96;
      while (start < visible_value.size() &&
             (static_cast<unsigned char>(visible_value[start]) & 0xc0) == 0x80)
        ++start;
      visible_value = "..." + visible_value.substr(start);
    }
    const auto value_size = ImGui::CalcTextSize(visible_value.c_str(), nullptr, false,
                                                std::max(scale, value_width - 16 * scale));
    const float height =
        std::max({item.detail.empty() ? 48 * scale : 72 * scale,
                  title_size.y + (item.detail.empty() ? 0 : help_size.y + 6 * scale) + 22 * scale,
                  item.toggle || item.value.empty() ? 0 : value_size.y + 22 * scale});
    ImGui::SetCursorPosX(margin);
    ImGui::PushID(item.id.c_str());
    if (entering && focus_id == item.id)
      ImGui::SetKeyboardFocusHere();
    ImGui::BeginDisabled(!item.enabled);
    ImGui::PushStyleColor(ImGuiCol_NavCursor, ImVec4(0, 0, 0, 0));
    const bool pressed = ImGui::Selectable(
        "##setting", false, ImGuiSelectableFlags_NoPadWithHalfSpacing, ImVec2(width, height));
    ImGui::PopStyleColor();
    const auto min = ImGui::GetItemRectMin();
    const auto max = ImGui::GetItemRectMax();
    if (pressed)
      result.activated = item.id;
    const bool options_clicked = item.has_options && ImGui::IsItemClicked(ImGuiMouseButton_Right);
    if (options_clicked)
    {
      result.activated = "edit:" + item.id;
      ImGui::SetFocusID(ImGui::GetItemID(), ImGui::GetCurrentWindow());
    }
    if (pressed || options_clicked ||
        (ImGui::IsItemFocused() && (!entering || focus_id == item.id)))
    {
      memory.item = item.id;
      memory.selected = item.id;
      if (entering || GImGui->NavJustMovedToId == ImGui::GetItemID())
      {
        ImRect reveal(ImVec2(min.x, first || heading ? heading_top : min.y), max);
        reveal.Expand(ImVec2(0, 16 * scale));
        ImGui::ScrollToRectEx(ImGui::GetCurrentWindow(), reveal,
                              ImGuiScrollFlags_KeepVisibleEdgeY | ImGuiScrollFlags_NoScrollParent);
      }
    }
    if (ImGui::IsItemVisible())
    {
      auto* rows = ImGui::GetWindowDrawList();
      const bool focused = ImGui::IsItemFocused() && ImGui::GetIO().NavVisible;
      if (ImGui::IsItemHovered() || focused)
      {
        auto hover = palette.hover;
        hover.w *= 0.7f;
        rows->AddRectFilled(min, max, ImGui::GetColorU32(hover), 6 * scale);
        rows->AddRect(min, max, ImGui::GetColorU32(palette.secondary), 6 * scale, 0, scale);
      }
      const float label_x = min.x + 48 * scale;
      DrawMenuIcon(rows, item.icon, ImVec2(min.x + 18 * scale, min.y + 12 * scale), 22 * scale,
                   ImGui::GetColorU32(palette.text));
      rows->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(label_x, min.y + 10 * scale),
                    ImGui::GetColorU32(palette.text), item.label.c_str(), nullptr, label_width);
      ImGui::PushFont(font, Style::SUMMARY_SIZE * scale / font_scale);
      rows->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                    ImVec2(min.x + 16 * scale, min.y + 16 * scale + title_size.y),
                    ImGui::GetColorU32(palette.secondary), summary.c_str(), nullptr, help_width);
      ImGui::PopFont();
      auto value_color = palette.text;
      if (!item.value_enabled)
        value_color.w *= 0.5f;
      if (item.toggle)
      {
        const float h = 24 * scale;
        const ImVec2 switch_min(max.x - 66 * scale, min.y + (height - h) * 0.5f);
        const ImVec2 switch_max(switch_min.x + 50 * scale, switch_min.y + h);
        auto track = *item.toggle ? palette.focus : Style::ColorFromHex(0x888888);
        if (!item.value_enabled)
          track.w *= 0.5f;
        rows->AddRectFilled(switch_min, switch_max, ImGui::GetColorU32(track), h * 0.5f);
        rows->AddCircleFilled(
            ImVec2(*item.toggle ? switch_max.x - h * 0.5f : switch_min.x + h * 0.5f,
                   switch_min.y + h * 0.5f),
            h * 0.5f - 2 * scale, ImGui::GetColorU32(value_color));
      }
      else if (!item.value.empty())
      {
        rows->AddText(
            ImGui::GetFont(), ImGui::GetFontSize(),
            ImVec2(max.x - 16 * scale - value_size.x, min.y + (height - value_size.y) * 0.5f),
            ImGui::GetColorU32(value_color), visible_value.c_str(), nullptr,
            std::max(scale, value_width - 16 * scale));
      }
      else if (item.active)
        DrawMenuIcon(rows, MenuIcon::Check,
                     ImVec2(max.x - 40 * scale, min.y + (height - 22 * scale) * 0.5f), 22 * scale,
                     ImGui::GetColorU32(palette.text));
    }
    ImGui::EndDisabled();
    ImGui::PopID();
  }
  memory.scroll = ImGui::GetScrollY() / scale;
  ImGui::EndChild();

  // A stepper page uses Left and Right (with key repeat) for its value; the shoulder buttons
  // still change the settings page.
  const bool stepper = !page.increase.empty() && !page.busy;
  const bool step_down = stepper && (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) ||
                                     ImGui::IsKeyPressed(ImGuiKey_GamepadDpadLeft));
  const bool step_up = stepper && (ImGui::IsKeyPressed(ImGuiKey_RightArrow) ||
                                   ImGui::IsKeyPressed(ImGuiKey_GamepadDpadRight));
  if (step_down || step_up)
  {
    ImGui::NavMoveRequestCancel();
    result.activated = step_down ? page.decrease : page.increase;
  }
  int direction = 0;
  if ((!stepper && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) ||
      ImGui::IsKeyPressed(ImGuiKey_GamepadL1))
    direction = -1;
  if ((!stepper && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) ||
      ImGui::IsKeyPressed(ImGuiKey_GamepadR1))
    direction = 1;
  if (direction && !categories.empty())
  {
    ImGui::NavMoveRequestCancel();
    const auto active =
        std::ranges::find_if(categories, [](const auto* item) { return item->active; });
    int index = active == categories.end() ? 0 : static_cast<int>(active - categories.begin());
    const int size = static_cast<int>(categories.size());
    for (int attempt = 0; attempt < size; ++attempt)
    {
      index = (index + direction + size) % size;
      if (categories[index]->enabled)
      {
        result.activated = categories[index]->id;
        break;
      }
    }
  }
  if (!page.busy && (ImGui::IsKeyPressed(ImGuiKey_F2, false) ||
                     (row(memory.item) != page.items.end() &&
                      ImGui::IsKeyPressed(ImGuiKey_GamepadDpadRight, false))))
  {
    const auto selected = row(memory.selected);
    if (selected != page.items.end() && selected->has_options)
    {
      ImGui::NavMoveRequestCancel();
      result.activated = "edit:" + selected->id;
    }
  }
  result.back = ImGui::IsKeyPressed(ImGuiKey_Escape, false) ||
                ImGui::IsKeyPressed(ImGuiKey_GamepadFaceRight, false);
  result.focused = memory.item;
  ImGui::PushFont(font, Style::SUMMARY_SIZE * scale / font_scale);
  const auto prompt =
      Style::DrawFooterText(draw, ImVec2(15 * scale, display.y - footer), display, prompts,
                            ImGui::GetColorU32(palette.header_text), 15 * scale);
  if (page.category_page)
  {
    draw->PushClipRect(ImVec2(15 * scale, display.y - footer),
                       ImVec2(std::max(15 * scale, prompt.x - 20 * scale), display.y), true);
    const auto end = page.description.find('\n');
    draw->AddText(ImVec2(15 * scale, prompt.y), ImGui::GetColorU32(palette.header_text),
                  page.description.c_str(),
                  page.description.c_str() +
                      (end == std::string::npos ? page.description.size() : end));
    draw->PopClipRect();
  }
  ImGui::PopFont();
  ImGui::End();
  ImGui::PopFont();
  return result;
}
}  // namespace VideoCommon::BigPicture
