// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"

#include <QCheckBox>

#include "Common/Assert.h"
#include "DolphinQt/Config/Binder/ConfigBinding.h"
#include "DolphinQt/Config/Binder/ConfigBindingLogic.h"

namespace ConfigWidget
{
namespace
{
// No Q_OBJECT: moc cannot process templates, and this adds no signals or slots of its own.
class CheckBoxBinding final : public ConfigBinding
{
public:
  CheckBoxBinding(QCheckBox* box, const Config::Info<bool>& setting, Config::Layer* layer,
                  bool reverse)
      : ConfigBinding(box, setting.GetLocation(), layer), m_setting(setting), m_reverse(reverse)
  {
    connect(box, &QCheckBox::toggled, this, &CheckBoxBinding::OnToggled);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override
  {
    auto* const box = qobject_cast<QCheckBox*>(GetWidget());
    if (box == nullptr)
      return;
    box->setChecked(static_cast<bool>(Logic::ReadValue(m_setting, GetLayer()) ^ m_reverse));
  }

  void OnToggled(bool checked)
  {
    if (IsUpdating())
      return;
    Logic::WriteValue(m_setting, GetLocation(), GetLayer(), static_cast<bool>(checked ^ m_reverse));
  }

  const Config::Info<bool> m_setting;
  const bool m_reverse;
};
}  // namespace

void Bind(QCheckBox* widget, const Config::Info<bool>& setting, Config::Layer* layer, bool reverse)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new CheckBoxBinding{widget, setting, layer, reverse};
}

ConfigBinding* FindBinding(QWidget* widget)
{
  return widget->findChild<ConfigBinding*>(QString{}, Qt::FindDirectChildrenOnly);
}
}  // namespace ConfigWidget
