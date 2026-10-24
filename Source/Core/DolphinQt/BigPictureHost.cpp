// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/BigPictureHost.h"

#include <utility>

#include <QThread>

#include "DolphinQt/QtUtils/QueueOnObject.h"

BigPictureHost::BigPictureHost(QObject* parent, ReadStateCallback read_state,
                               ExecuteCallback execute)
    : QObject(parent), m_read_state(std::move(read_state)), m_execute(std::move(execute)),
      m_controller(*this)
{
  Publish();
}

BigPicture::HostState BigPictureHost::ReadState() const
{
  Q_ASSERT(QThread::currentThread() == thread());
  return m_read_state();
}

BigPicture::Result BigPictureHost::Execute(const BigPicture::Command& command)
{
  Q_ASSERT(QThread::currentThread() == thread());
  const auto reason = BigPicture::GetAvailability(command.action, ReadState());
  if (reason != BigPicture::UnavailableReason::None)
    return {command, BigPicture::ResultStatus::Unavailable, reason};
  return m_execute(command);
}

void BigPictureHost::Submit(BigPicture::Command command)
{
  // Always queue, including on the UI thread. A drawing/input callback never executes a host
  // action inline, and Qt discards the callback if the host is destroyed before delivery.
  QueueOnObject(this, [this, command = std::move(command)] {
    m_controller.Submit(command);
    Publish();
  });
}

std::shared_ptr<const BigPicture::Snapshot> BigPictureHost::GetSnapshot() const
{
  std::lock_guard lock(m_snapshot_mutex);
  return m_snapshot;
}

void BigPictureHost::Publish()
{
  std::lock_guard lock(m_snapshot_mutex);
  m_snapshot = m_controller.GetSnapshot();
}

void BigPictureHost::Refresh()
{
  Q_ASSERT(QThread::currentThread() == thread());
  m_controller.Refresh();
  Publish();
}

void BigPictureHost::InvalidateSession()
{
  Q_ASSERT(QThread::currentThread() == thread());
  m_controller.InvalidateSession();
  Publish();
}
