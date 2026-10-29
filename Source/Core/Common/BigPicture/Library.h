// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace BigPicture
{
constexpr std::uint32_t MAX_COVER_WIDTH = 512;
constexpr std::uint32_t MAX_COVER_HEIGHT = 768;
// Decoded covers retained by a library snapshot: enough for several grid pages of scrolling,
// with a memory bound for large custom artwork.
constexpr std::size_t MAX_DECODED_COVERS = 64;
constexpr std::size_t MAX_DECODED_COVER_BYTES = 64 * 1024 * 1024;

enum class LibraryOperation
{
  None,
  Select,
  Options,
  Search,
  AddFolder,
  OpenFile,
  OpenDisc,
  EnterDirectory,
  ChooseFolder,
  Refresh,
  View,
  Sort,
  Platform,
  Region,
  Tag,
  ClearFilters,
  TextKey,
  TextErase,
  TextDone,
  EditPath,
  Cover,
  Recursive,
  ClearSearch,
  TextClear,
  TextReplace,
  BrowseDiscs,
  ControllerKeyboard,
  ChooseBootDisc,
  Count
};

struct LibraryFile
{
  std::string path;
  std::string name;
  bool directory = false;
};

struct LibraryGame
{
  std::string path;
  std::string title;
  std::string game_id;
  std::string platform;
  std::string region;
  std::string description;
  std::vector<std::string> tags;
  std::uint64_t played_ms = 0;
  std::uint64_t size = 0;
  std::string details;
  std::vector<LibraryFile> discs;
};

struct LibraryImage
{
  std::uint64_t revision = 0;
  std::uint32_t width = 0;
  std::uint32_t height = 0;
  std::vector<std::uint8_t> rgba;
};

// Published by the Qt owner. Covers are decoded off-thread, bounded thumbnails; GPU resources
// never cross this boundary. A scan or filter change cannot mutate a retained snapshot.
struct LibrarySnapshot
{
  std::uint64_t catalog_revision = 0;
  std::uint64_t view_revision = 0;
  std::uint64_t picker_revision = 0;
  std::vector<LibraryGame> games;
  std::string selected_path;
  std::size_t total_games = 0;
  bool scanning = false;
  bool grid = true;
  std::string query;
  std::string sort;
  std::string platform;
  std::string region;
  std::string tag;
  std::string directory;
  std::string parent_directory;
  std::vector<LibraryFile> files;
  bool picker_loading = false;
  bool picker_folders_only = false;
  bool picker_disc = false;
  bool picker_library = false;
  bool picker_boot_disc = false;
  bool recursive = false;
  std::string picker_error;
  std::string draft;
  bool editing_path = false;
  bool native_text_input = false;
  std::map<std::string, std::shared_ptr<const LibraryImage>> covers;
  std::set<std::string> missing_covers;
};
}  // namespace BigPicture
