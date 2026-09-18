// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Common/Config/Config.h"
#include "Common/Config/Layer.h"

// Qt-free half of the config binder: everything about deciding *which* config layer a bound
// widget reads from and writes to. Kept free of Qt so it can be tested in the Qt-free `tests`
// binary. Mirrors the semantics of ConfigControl::ReadValue / SaveValue / IsConfigLocal.
namespace ConfigWidget::Logic
{
// A null `layer` means the binding edits global config. A non-null `layer` means it edits a
// per-game layer, falling back to the base layer for display when the key is absent there.
template <typename T>
T ReadValue(const Config::Info<T>& setting, Config::Layer* layer)
{
  if (layer != nullptr)
  {
    if (layer->Exists(setting.GetLocation()))
      return layer->Get(setting);
    // There is no way to know which game is being edited, so GlobalGame settings cannot be
    // shown; the base layer is the closest meaningful value.
    return Config::GetBase(setting);
  }
  return Config::Get(setting);
}

template <typename T>
void WriteValue(const Config::Info<T>& setting, const Config::Location& location,
                Config::Layer* layer, const T& value)
{
  if (layer != nullptr)
  {
    layer->Set(location, value);
    Config::OnConfigChanged();
    return;
  }
  Config::SetBaseOrCurrent(setting, value);
}

// Whether the value shown comes from somewhere other than the base layer. Drives the bold font
// that marks an overridden setting.
inline bool IsLocal(const Config::Location& location, Config::Layer* layer)
{
  if (layer != nullptr)
    return layer->Exists(location);
  return Config::GetActiveLayerForConfig(location) != Config::LayerType::Base;
}

// Drops a per-game override so the setting reverts to the global value. A no-op for global
// bindings, which have no override to drop.
inline void ClearLocal(const Config::Location& location, Config::Layer* layer)
{
  if (layer == nullptr)
    return;
  layer->DeleteKey(location);
  Config::OnConfigChanged();
}
}  // namespace ConfigWidget::Logic
