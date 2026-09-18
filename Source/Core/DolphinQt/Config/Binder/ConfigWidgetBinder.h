// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm>
#include <functional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <QComboBox>
#include <QString>

#include "Common/Assert.h"
#include "Common/CommonTypes.h"
#include "Common/Config/ConfigInfo.h"
#include "DolphinQt/Config/Binder/ConfigBinding.h"
#include "DolphinQt/Config/Binder/ConfigSliderMapping.h"

class QCheckBox;
class QLabel;
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

void Bind(QComboBox* widget, const Config::Info<u32>& setting, Config::Layer* layer = nullptr);

// Two forms, matching ConfigStringChoice: the option text is the stored data, or display text and
// stored data are given separately.
void BindStringChoice(QComboBox* widget, const Config::Info<std::string>& setting,
                      std::span<const std::string> options, Config::Layer* layer = nullptr);
void BindStringChoice(QComboBox* widget, const Config::Info<std::string>& setting,
                      std::span<const std::pair<QString, QString>> options,
                      Config::Layer* layer = nullptr);

// Slider position i means tick_values[i]. The .ui must author minimum 0, maximum one less than the
// tick count, pageStep 1 and a tick position; the binding asserts the range and never changes it.
// Disables the slider while the config value matches no tick. The binding owns the enabled
// property.
void BindMapped(QSlider* widget, const Config::Info<int>& setting, std::span<const int> tick_values,
                Config::Layer* layer = nullptr);
// Stored value is the slider position times `scale`.
void BindScaled(QSlider* widget, const Config::Info<u32>& setting, u32 scale,
                Config::Layer* layer = nullptr);

// The .ui file authors minimum = 0 and maximum = MaximumPosition() for the float range it will be
// bound to. BindFloat asserts they agree rather than imposing them: Designer owns geometry, and a
// binding asserts agreement with it. The handle exists because call sites read the mapped value
// back to drive a value label.
struct FloatSliderHandle
{
  QSlider* slider = nullptr;
  FloatSliderRange range{};

  float Value() const;
};

FloatSliderHandle BindFloat(QSlider* widget, const Config::Info<float>& setting, float minimum,
                            float maximum, float step, Config::Layer* layer = nullptr);

// `format` is a printf-style format taking one double, e.g. "%.0f%%". Sets the label immediately
// and on every subsequent slider move.
void MirrorFloatValue(QLabel* label, FloatSliderHandle handle, const QString& format);

// Displays the config value if it is non-empty, otherwise File::GetUserPath(dir_index): an empty
// config value means "use the current user path". Saving sets both.
void BindUserPath(QLineEdit* widget, unsigned int dir_index,
                  const Config::Info<std::string>& setting, Config::Layer* layer = nullptr);

// Replaces the modal warning shown when a user path field is emptied. Passing an empty function
// restores the default. Exists so qt-tests can assert the rejection without a modal exec() that
// would block forever.
using PathWarningHandler = std::function<void(QWidget* parent, const QString& message)>;
void SetPathWarningHandlerForTesting(PathWarningHandler handler);

// The binding attached to `widget`, or nullptr if it has none.
ConfigBinding* FindBinding(QWidget* widget);

namespace detail
{
// value <-> combo-index mapping, shared by both BindMapped overloads. No Q_OBJECT: it is a
// template.
template <typename T>
class MappedComboBinding final : public ValueBinding<QComboBox, T>
{
public:
  MappedComboBinding(QComboBox* box, const Config::Info<T>& setting, std::vector<T> values,
                     Config::Layer* layer)
      : ValueBinding<QComboBox, T>(box, setting, layer), m_values(std::move(values))
  {
    QObject::connect(box, &QComboBox::currentIndexChanged, this,
                     &MappedComboBinding::OnIndexChanged);
    this->RefreshFromConfig();
  }

private:
  void LoadFromConfig() override
  {
    const T value = this->Read();
    const auto it = std::find(m_values.begin(), m_values.end(), value);
    // -1 when nothing matches, as ConfigChoiceMap does: better an empty combo than a wrong
    // selection that the user then saves by touching something else.
    const int index =
        it == m_values.end() ? -1 : static_cast<int>(std::distance(m_values.begin(), it));
    this->GetTypedWidget()->setCurrentIndex(index);
  }

  void OnIndexChanged(int index)
  {
    if (index < 0 || static_cast<size_t>(index) >= m_values.size())
      return;
    this->Save(m_values[static_cast<size_t>(index)]);
  }

  const std::vector<T> m_values;
};
}  // namespace detail

// The items are authored in the .ui file; `values` pairs with them by index, so the display text
// stays in the uic translation path.
template <typename T>
void BindMapped(QComboBox* widget, const Config::Info<T>& setting, std::span<const T> values,
                Config::Layer* layer = nullptr)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  DEBUG_ASSERT(!values.empty());
  DEBUG_ASSERT(widget->count() == static_cast<int>(values.size()));
  new detail::MappedComboBinding<T>{widget, setting, std::vector<T>(values.begin(), values.end()),
                                    layer};
}

// For option sets computed at runtime, which cannot be authored in Designer. Populates the combo.
template <typename T>
void BindMapped(QComboBox* widget, const Config::Info<T>& setting,
                std::span<const std::pair<QString, T>> options, Config::Layer* layer = nullptr)
{
  DEBUG_ASSERT(FindBinding(widget) == nullptr);
  DEBUG_ASSERT(!options.empty());
  DEBUG_ASSERT(widget->count() == 0);
  std::vector<T> values;
  values.reserve(options.size());
  for (const auto& [text, value] : options)
  {
    widget->addItem(text);
    values.push_back(value);
  }
  new detail::MappedComboBinding<T>{widget, setting, std::move(values), layer};
}
}  // namespace ConfigWidget
