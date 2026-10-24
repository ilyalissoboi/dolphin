// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <functional>
#include <memory>
#include <mutex>

#include <QObject>

#include "UICommon/BigPicture/Controller.h"

// The host owns the controller on Qt's thread. Render/input owners may only read a published
// snapshot or queue a value command; neither operation touches a QWidget or live core object.
class BigPictureHost final : public QObject, private BigPicture::Host
{
public:
  using ReadStateCallback = std::function<BigPicture::HostState()>;
  using ExecuteCallback = std::function<BigPicture::Result(const BigPicture::Command&)>;

  BigPictureHost(QObject* parent, ReadStateCallback read_state, ExecuteCallback execute);
  void Submit(BigPicture::Command command);
  std::shared_ptr<const BigPicture::Snapshot> GetSnapshot() const;
  void Refresh();
  void InvalidateSession();

private:
  BigPicture::HostState ReadState() const override;
  BigPicture::Result Execute(const BigPicture::Command& command) override;
  void Publish();

  ReadStateCallback m_read_state;
  ExecuteCallback m_execute;
  BigPicture::Controller m_controller;
  mutable std::mutex m_snapshot_mutex;
  std::shared_ptr<const BigPicture::Snapshot> m_snapshot;
};
