// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <memory>

#include <QCheckBox>
#include <QPointer>
#include <gtest/gtest.h>

#include "Common/Config/Config.h"
#include "Common/Config/Layer.h"
#include "DolphinQt/Config/Binder/ConfigBinding.h"
#include "DolphinQt/Config/Binder/ConfigChangeBroadcaster.h"
#include "DolphinQt/Config/Binder/ConfigWidgetBinder.h"

namespace
{
const Config::Info<bool> TEST_BOOL{{Config::System::Main, "BinderRoundTrip", "Bool"}, false};

class NullLoader final : public Config::ConfigLayerLoader
{
public:
  NullLoader() : ConfigLayerLoader(Config::LayerType::Base) {}
  void Load(Config::Layer*) override {}
  void Save(Config::Layer*) override {}
};

class ConfigBinderRoundTripTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    Config::Init();
    Config::AddLayer(std::make_unique<NullLoader>());
  }
  void TearDown() override { Config::Shutdown(); }

  // Bindings refresh on ConfigChangeBroadcaster::Changed, which dolphin-emu drives from
  // Settings::ConfigChanged. Tests drive it directly.
  static void NotifyConfigChanged()
  {
    ConfigWidget::ConfigChangeBroadcaster::Instance().Broadcast();
  }
};
}  // namespace

TEST_F(ConfigBinderRoundTripTest, CheckBoxAdoptsTheConfigValueWhenBound)
{
  Config::SetBase(TEST_BOOL, true);
  QCheckBox box;
  ASSERT_FALSE(box.isChecked());

  ConfigWidget::Bind(&box, TEST_BOOL);

  EXPECT_TRUE(box.isChecked());
}

TEST_F(ConfigBinderRoundTripTest, CheckBoxFollowsLaterConfigChanges)
{
  QCheckBox box;
  ConfigWidget::Bind(&box, TEST_BOOL);
  ASSERT_FALSE(box.isChecked());

  Config::SetBase(TEST_BOOL, true);
  NotifyConfigChanged();

  EXPECT_TRUE(box.isChecked());
}

TEST_F(ConfigBinderRoundTripTest, TogglingTheCheckBoxWritesConfig)
{
  QCheckBox box;
  ConfigWidget::Bind(&box, TEST_BOOL);

  box.setChecked(true);

  EXPECT_TRUE(Config::Get(TEST_BOOL));
}

TEST_F(ConfigBinderRoundTripTest, ReverseInvertsBothDirections)
{
  Config::SetBase(TEST_BOOL, true);
  QCheckBox box;
  ConfigWidget::Bind(&box, TEST_BOOL, nullptr, true);
  EXPECT_FALSE(box.isChecked()) << "reverse means a true setting shows as unchecked";

  box.setChecked(true);
  EXPECT_FALSE(Config::Get(TEST_BOOL));
}

TEST_F(ConfigBinderRoundTripTest, LayeredBindingWritesToTheLayerNotTheBase)
{
  Config::Layer layer{Config::LayerType::LocalGame};
  Config::SetBase(TEST_BOOL, false);
  QCheckBox box;
  ConfigWidget::Bind(&box, TEST_BOOL, &layer);

  box.setChecked(true);

  EXPECT_TRUE(layer.Exists(TEST_BOOL.GetLocation()));
  EXPECT_TRUE(layer.Get(TEST_BOOL));
  EXPECT_FALSE(Config::GetBase(TEST_BOOL));
}

TEST_F(ConfigBinderRoundTripTest, RefreshingFromConfigDoesNotWriteBack)
{
  // The constructor's RefreshFromConfig() calls setChecked(true), which emits toggled. Without the
  // IsUpdating() guard, OnToggled would write into the empty layer, creating a per-game override
  // that nobody asked for.
  Config::Layer layer{Config::LayerType::LocalGame};
  Config::SetBase(TEST_BOOL, true);
  QCheckBox box;
  ConfigWidget::Bind(&box, TEST_BOOL, &layer);

  NotifyConfigChanged();

  EXPECT_TRUE(box.isChecked()) << "shows the base value";
  EXPECT_FALSE(layer.Exists(TEST_BOOL.GetLocation()))
      << "a refresh must not create a per-game override";
}

TEST_F(ConfigBinderRoundTripTest, FindBindingReturnsTheAttachedBinding)
{
  QCheckBox unbound;
  EXPECT_EQ(ConfigWidget::FindBinding(&unbound), nullptr);

  QCheckBox bound;
  ConfigWidget::Bind(&bound, TEST_BOOL);
  EXPECT_NE(ConfigWidget::FindBinding(&bound), nullptr);
}

TEST_F(ConfigBinderRoundTripTest, BindingIsDestroyedWhenWidgetIsDestroyed)
{
  auto* box = new QCheckBox;
  ConfigWidget::Bind(box, TEST_BOOL);
  auto* binding = ConfigWidget::FindBinding(box);
  ASSERT_NE(binding, nullptr);
  QPointer<QObject> binding_ptr{qobject_cast<QObject*>(binding)};

  delete box;

  EXPECT_TRUE(binding_ptr.isNull()) << "binding must be destroyed with its widget";
}
