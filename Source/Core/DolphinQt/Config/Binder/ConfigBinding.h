// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include "Common/Config/ConfigInfo.h"

class QEvent;
class QWidget;

namespace Config
{
class Layer;
}

namespace ConfigWidget
{
// Attached as a child of the widget it binds, so it lives exactly as long as the widget and call
// sites do no ownership bookkeeping. Replaces ConfigControl<Derived>, whose config location was a
// constructor argument and therefore could not be produced by uic.
//
// This is the only class in the binder with Q_OBJECT: moc cannot process templates, so typed
// state lives in non-Q_OBJECT subclasses that add no signals or slots.
class ConfigBinding : public QObject
{
  Q_OBJECT

public:
  ConfigBinding(QWidget* widget, Config::Location location, Config::Layer* layer);
  ~ConfigBinding() override;

  const Config::Location& GetLocation() const { return m_location; }
  Config::Layer* GetLayer() const { return m_layer; }

protected:
  QWidget* GetWidget() const;

  // True while LoadFromConfig() is running. Subclasses must not write config in that window:
  // refresh sets the widget, the widget emits its value-changed signal, and a naive handler would
  // save straight back, clobbering a per-game layer or recursing.
  bool IsUpdating() const { return m_updating; }

  // Re-reads config into the widget and re-applies the overridden-value font.
  void RefreshFromConfig();

  virtual void LoadFromConfig() = 0;

  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  void ApplyOverrideFont();

  const Config::Location m_location;
  Config::Layer* m_layer;  // Caller must keep the layer alive at least as long as the widget.
  bool m_updating = false;
};
}  // namespace ConfigWidget
