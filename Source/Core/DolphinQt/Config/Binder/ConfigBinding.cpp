// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Binder/ConfigBinding.h"

#include <QFont>
#include <QWidget>
#include <utility>

#include "Common/ScopeGuard.h"
#include "DolphinQt/Config/Binder/ConfigBindingLogic.h"
#include "DolphinQt/Config/Binder/ConfigChangeBroadcaster.h"

namespace ConfigWidget
{
ConfigBinding::ConfigBinding(QWidget* widget, Config::Location location, Config::Layer* layer)
    : QObject(widget), m_location(std::move(location)), m_layer(layer)
{
  // The filter is the hook for override handling (right-click-to-clear); deliberately a
  // pass-through for now.
  widget->installEventFilter(this);
  connect(&ConfigChangeBroadcaster::Instance(), &ConfigChangeBroadcaster::Changed, this,
          &ConfigBinding::RefreshFromConfig);
}

ConfigBinding::~ConfigBinding() = default;

QWidget* ConfigBinding::GetWidget() const
{
  return qobject_cast<QWidget*>(parent());
}

void ConfigBinding::RefreshFromConfig()
{
  ApplyOverrideFont();

  const bool was_updating = m_updating;
  m_updating = true;
  Common::ScopeGuard guard{[this, was_updating] { m_updating = was_updating; }};
  LoadFromConfig();
}

void ConfigBinding::ApplyOverrideFont()
{
  QWidget* const widget = GetWidget();
  if (widget == nullptr)
    return;

  QFont font = widget->font();
  font.setBold(Logic::IsLocal(m_location, m_layer));
  widget->setFont(font);
}

bool ConfigBinding::eventFilter(QObject* watched, QEvent* event)
{
  return QObject::eventFilter(watched, event);
}
}  // namespace ConfigWidget
