// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <thread>

#include <QCoreApplication>
#include <QThread>
#include <gtest/gtest.h>

#include "DolphinQt/BigPictureHost.h"

namespace
{
using namespace BigPicture;

TEST(BigPictureHostTest, WorkerCommandsExecuteOnlyOnQtOwnerAndSnapshotsStayValid)
{
  HostState state{Lifecycle::Running};
  int calls = 0;
  BigPictureHost host(
      nullptr, [&] { return state; },
      [&](const Command& command) {
        EXPECT_EQ(QThread::currentThread(), QCoreApplication::instance()->thread());
        ++calls;
        return Result{command, ResultStatus::Dispatched};
      });
  const auto old = host.GetSnapshot();
  std::thread worker([&] {
    const auto snapshot = host.GetSnapshot();
    host.Submit({snapshot->generation, 1, Action::SaveState, {}, 1});
  });
  worker.join();
  EXPECT_EQ(calls, 0);
  QCoreApplication::processEvents();
  EXPECT_EQ(calls, 1);
  EXPECT_FALSE(old->result);
  ASSERT_TRUE(host.GetSnapshot()->result);
  EXPECT_EQ(host.GetSnapshot()->result->status, ResultStatus::Dispatched);
}

TEST(BigPictureHostTest, SessionChangeAndPolicyChangeAreCheckedAfterQueueing)
{
  HostState state{Lifecycle::Running};
  int calls = 0;
  BigPictureHost host(
      nullptr, [&] { return state; },
      [&](const Command& command) {
        ++calls;
        return Result{command, ResultStatus::Dispatched};
      });
  host.Submit({host.GetSnapshot()->generation, 1, Action::LoadState, {}, 1});
  state.hardcore = true;
  QCoreApplication::processEvents();
  EXPECT_EQ(calls, 0);
  EXPECT_EQ(host.GetSnapshot()->result->reason, UnavailableReason::Hardcore);
  host.Submit({host.GetSnapshot()->generation, 2, Action::SaveState, {}, 1});
  host.InvalidateSession();
  QCoreApplication::processEvents();
  EXPECT_EQ(calls, 0);
  EXPECT_EQ(host.GetSnapshot()->result->status, ResultStatus::Stale);
}

TEST(BigPictureHostTest, DestructionDropsQueuedActions)
{
  int calls = 0;
  {
    BigPictureHost host(
        nullptr, [] { return HostState{Lifecycle::Running}; },
        [&](const Command& command) {
          ++calls;
          return Result{command, ResultStatus::Dispatched};
        });
    host.Submit({host.GetSnapshot()->generation, 1, Action::SaveState, {}, 1});
  }
  QCoreApplication::processEvents();
  EXPECT_EQ(calls, 0);
}
}  // namespace
