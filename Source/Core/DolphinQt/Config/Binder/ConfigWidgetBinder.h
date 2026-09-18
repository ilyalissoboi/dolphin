// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Common/Config/ConfigInfo.h"

class QCheckBox;
class QWidget;

namespace Config
{
class Layer;
}

// Binds stock Qt widgets to typed config settings after construction, so layouts can be authored
// in Qt Designer .ui files. A null `layer` binds global config; a non-null `layer` binds a
// per-game layer. Config::Info<T> already carries the key, type and default, so no key or default
// argument is needed.
namespace ConfigWidget
{
class ConfigBinding;

void Bind(QCheckBox* widget, const Config::Info<bool>& setting, Config::Layer* layer = nullptr,
          bool reverse = false);

// The binding attached to `widget`, or nullptr if it has none.
ConfigBinding* FindBinding(QWidget* widget);
}  // namespace ConfigWidget
