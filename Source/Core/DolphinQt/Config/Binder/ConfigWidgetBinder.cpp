// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>

#include "Common/Assert.h"
#include "DolphinQt/Config/Binder/ConfigBinding.h"
#include "DolphinQt/Config/Binder/ConfigBindingLogic.h"

namespace ConfigWidget
{
namespace
{
class CheckBoxBinding final : public ValueBinding<QCheckBox, bool>
{
public:
  CheckBoxBinding(QCheckBox* box, const Config::Info<bool>& setting, Config::Layer* layer,
                  bool reverse)
      : ValueBinding(box, setting, layer), m_reverse(reverse)
  {
    connect(box, &QCheckBox::toggled, this, &CheckBoxBinding::OnToggled);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setChecked(Read() ^ m_reverse); }
  void OnToggled(bool checked) { Save(checked ^ m_reverse); }

  const bool m_reverse;
};

class ComboBoxBinding final : public ValueBinding<QComboBox, int>
{
public:
  ComboBoxBinding(QComboBox* box, const Config::Info<int>& setting, Config::Layer* layer)
      : ValueBinding(box, setting, layer)
  {
    connect(box, &QComboBox::currentIndexChanged, this, &ComboBoxBinding::OnIndexChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setCurrentIndex(Read()); }
  void OnIndexChanged(int index) { Save(index); }
};

class SpinBoxBinding final : public ValueBinding<QSpinBox, int>
{
public:
  SpinBoxBinding(QSpinBox* spin, const Config::Info<int>& setting, Config::Layer* layer)
      : ValueBinding(spin, setting, layer)
  {
    connect(spin, &QSpinBox::valueChanged, this, &SpinBoxBinding::OnValueChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setValue(Read()); }
  void OnValueChanged(int value) { Save(value); }
};

class SliderBinding final : public ValueBinding<QSlider, int>
{
public:
  SliderBinding(QSlider* slider, const Config::Info<int>& setting, Config::Layer* layer)
      : ValueBinding(slider, setting, layer)
  {
    connect(slider, &QSlider::valueChanged, this, &SliderBinding::OnValueChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setValue(Read()); }
  void OnValueChanged(int value) { Save(value); }
};

class RadioButtonBinding final : public ValueBinding<QRadioButton, int>
{
public:
  RadioButtonBinding(QRadioButton* button, const Config::Info<int>& setting, int value,
                     Config::Layer* layer)
      : ValueBinding(button, setting, layer), m_value(value)
  {
    connect(button, &QRadioButton::toggled, this, &RadioButtonBinding::OnToggled);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setChecked(Read() == m_value); }

  void OnToggled(bool checked)
  {
    // Autoexclusive grouping unchecks the previous button as part of checking the new one. Only
    // the button that won may write, or the two saves race.
    if (checked)
      Save(m_value);
  }

  const int m_value;
};

class LineEditBinding final : public ValueBinding<QLineEdit, std::string>
{
public:
  LineEditBinding(QLineEdit* edit, const Config::Info<std::string>& setting, Config::Layer* layer)
      : ValueBinding(edit, setting, layer)
  {
    connect(edit, &QLineEdit::editingFinished, this, &LineEditBinding::OnEditingFinished);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setText(QString::fromStdString(Read())); }

  void OnEditingFinished() { Save(GetTypedWidget()->text().toStdString()); }
};
}  // namespace

void Bind(QCheckBox* widget, const Config::Info<bool>& setting, Config::Layer* layer, bool reverse)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new CheckBoxBinding{widget, setting, layer, reverse};
}

void Bind(QComboBox* widget, const Config::Info<int>& setting, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new ComboBoxBinding{widget, setting, layer};
}

void Bind(QSpinBox* widget, const Config::Info<int>& setting, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new SpinBoxBinding{widget, setting, layer};
}

void Bind(QSlider* widget, const Config::Info<int>& setting, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new SliderBinding{widget, setting, layer};
}

void Bind(QRadioButton* widget, const Config::Info<int>& setting, int value, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new RadioButtonBinding{widget, setting, value, layer};
}

void Bind(QLineEdit* widget, const Config::Info<std::string>& setting, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new LineEditBinding{widget, setting, layer};
}

ConfigBinding* FindBinding(QWidget* widget)
{
  return widget->findChild<ConfigBinding*>(QString{}, Qt::FindDirectChildrenOnly);
}
}  // namespace ConfigWidget
