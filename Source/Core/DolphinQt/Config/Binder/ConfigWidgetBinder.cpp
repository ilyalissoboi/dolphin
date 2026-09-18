// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"

#include <algorithm>
#include <vector>

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

class ComboBoxU32Binding final : public ValueBinding<QComboBox, u32>
{
public:
  ComboBoxU32Binding(QComboBox* box, const Config::Info<u32>& setting, Config::Layer* layer)
      : ValueBinding(box, setting, layer)
  {
    connect(box, &QComboBox::currentIndexChanged, this, &ComboBoxU32Binding::OnIndexChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override { GetTypedWidget()->setCurrentIndex(static_cast<int>(Read())); }
  void OnIndexChanged(int index)
  {
    if (index >= 0)
      Save(static_cast<u32>(index));
  }
};

class StringChoiceBinding final : public ValueBinding<QComboBox, std::string>
{
public:
  StringChoiceBinding(QComboBox* box, const Config::Info<std::string>& setting,
                      Config::Layer* layer)
      : ValueBinding(box, setting, layer)
  {
    connect(box, &QComboBox::currentIndexChanged, this, &StringChoiceBinding::OnIndexChanged);
    RefreshFromConfig();
  }

private:
  // findData returns -1 when nothing matches, which clears the selection - the same outcome as
  // before, and better than showing a value that is not the one in effect.
  void LoadFromConfig() override
  {
    auto* const box = GetTypedWidget();
    box->setCurrentIndex(box->findData(QString::fromStdString(Read())));
  }

  void OnIndexChanged(int index)
  {
    if (index >= 0)
      Save(GetTypedWidget()->itemData(index).toString().toStdString());
  }
};

class TickSliderBinding final : public ValueBinding<QSlider, int>
{
public:
  TickSliderBinding(QSlider* slider, const Config::Info<int>& setting, std::vector<int> ticks,
                    Config::Layer* layer)
      : ValueBinding(slider, setting, layer), m_ticks(std::move(ticks))
  {
    ASSERT(!m_ticks.empty());
    connect(slider, &QSlider::valueChanged, this, &TickSliderBinding::OnValueChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override
  {
    auto* const slider = GetTypedWidget();
    const int value = Read();
    const auto it = std::find(m_ticks.begin(), m_ticks.end(), value);
    if (it == m_ticks.end())
    {
      // The config holds a value this slider cannot represent. Disable rather than snap, so the
      // user is not shown a value that is not the one in effect.
      slider->setEnabled(false);
      return;
    }
    slider->setEnabled(true);
    slider->setValue(static_cast<int>(std::distance(m_ticks.begin(), it)));
  }

  void OnValueChanged(int position)
  {
    if (position >= 0 && static_cast<size_t>(position) < m_ticks.size())
      Save(m_ticks[static_cast<size_t>(position)]);
  }

  const std::vector<int> m_ticks;
};

class ScaledSliderBinding final : public ValueBinding<QSlider, u32>
{
public:
  ScaledSliderBinding(QSlider* slider, const Config::Info<u32>& setting, u32 scale,
                      Config::Layer* layer)
      : ValueBinding(slider, setting, layer), m_scale(scale)
  {
    ASSERT(scale != 0);
    connect(slider, &QSlider::valueChanged, this, &ScaledSliderBinding::OnValueChanged);
    RefreshFromConfig();
  }

private:
  void LoadFromConfig() override
  {
    // Qt clamps to the slider's range, so a stored value outside that range shows a clamped
    // position while the stored value remains in effect — inherited from ConfigSliderU32.
    GetTypedWidget()->setValue(static_cast<int>(Read() / m_scale));
  }
  void OnValueChanged(int position)
  {
    if (position < 0)
      return;
    Save(static_cast<u32>(position) * m_scale);
  }

  const u32 m_scale;
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

void Bind(QComboBox* widget, const Config::Info<u32>& setting, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new ComboBoxU32Binding{widget, setting, layer};
}

void BindStringChoice(QComboBox* widget, const Config::Info<std::string>& setting,
                      std::span<const std::string> options, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  DEBUG_ASSERT(!options.empty());
  DEBUG_ASSERT(widget->count() == 0);
  for (const std::string& option : options)
  {
    const QString text = QString::fromStdString(option);
    widget->addItem(text, text);
  }
  new StringChoiceBinding{widget, setting, layer};
}

void BindStringChoice(QComboBox* widget, const Config::Info<std::string>& setting,
                      std::span<const std::pair<QString, QString>> options, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  DEBUG_ASSERT(!options.empty());
  DEBUG_ASSERT(widget->count() == 0);
  for (const auto& [text, value] : options)
    widget->addItem(text, value);
  new StringChoiceBinding{widget, setting, layer};
}

void BindMapped(QSlider* widget, const Config::Info<int>& setting, std::span<const int> tick_values,
                Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  DEBUG_ASSERT(!tick_values.empty());
  DEBUG_ASSERT(widget->minimum() == 0 &&
               widget->maximum() == static_cast<int>(tick_values.size()) - 1);
  new TickSliderBinding{widget, setting, std::vector<int>(tick_values.begin(), tick_values.end()),
                        layer};
}

void BindScaled(QSlider* widget, const Config::Info<u32>& setting, u32 scale, Config::Layer* layer)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  new ScaledSliderBinding{widget, setting, scale, layer};
}

ConfigBinding* FindBinding(QWidget* widget)
{
  return widget->findChild<ConfigBinding*>(QString{}, Qt::FindDirectChildrenOnly);
}
}  // namespace ConfigWidget
