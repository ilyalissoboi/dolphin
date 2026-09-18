// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Common/Config/ConfigInfo.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QRadioButton;
class QSlider;
class QSpinBox;
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

// Binds the combo box's current index. For an index-to-value mapping use BindMapped.
void Bind(QComboBox* widget, const Config::Info<int>& setting, Config::Layer* layer = nullptr);
// The spin box's and slider's minimum and maximum come from the .ui file and are left alone.
void Bind(QSpinBox* widget, const Config::Info<int>& setting, Config::Layer* layer = nullptr);
void Bind(QSlider* widget, const Config::Info<int>& setting, Config::Layer* layer = nullptr);
// Checked exactly when the setting equals `value`; writes `value` when it becomes checked.
void Bind(QRadioButton* widget, const Config::Info<int>& setting, int value,
          Config::Layer* layer = nullptr);
// Saves on editingFinished, so a half-typed path is never written to the config file.
void Bind(QLineEdit* widget, const Config::Info<std::string>& setting,
          Config::Layer* layer = nullptr);

// The binding attached to `widget`, or nullptr if it has none.
ConfigBinding* FindBinding(QWidget* widget);
}  // namespace ConfigWidget
