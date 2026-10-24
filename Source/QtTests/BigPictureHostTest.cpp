// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <thread>

#include <QCoreApplication>
#include <QThread>
#include <gtest/gtest.h>

#include "Common/BigPicture/PresentationState.h"
#include "DolphinQt/BigPictureHost.h"

namespace
{
using namespace BigPicture;

TEST(BigPictureHostTest, QueuedNavigationCannotExecuteAfterFocusReset)
{
  auto& presentation = GetPresentationState();
  presentation.SetFocused(true);
  BigPictureHost host(
      nullptr, [] { return HostState{Lifecycle::Running}; },
      [](const Command& command) { return Result{command}; });
  host.RequestAction(host.GetSnapshot()->generation, Action::OpenMenu,
                     presentation.GetFrameInputEpoch());
  presentation.SetFocused(false);
  presentation.SetFocused(true);
  QCoreApplication::processEvents();
  EXPECT_EQ(host.GetSnapshot()->mode, Mode::Desktop);
  host.RequestAction(host.GetSnapshot()->generation, Action::OpenMenu,
                     presentation.GetFrameInputEpoch());
  QCoreApplication::processEvents();
  EXPECT_EQ(host.GetSnapshot()->mode, Mode::InGame);
}

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

TEST(BigPictureHostTest, ClosingMenuOnlyResumesItsOwnPause)
{
  HostState state{Lifecycle::Running};
  int pauses = 0;
  int resumes = 0;
  BigPictureHost host(
      nullptr, [&] { return state; },
      [](const Command& command) { return Result{command, ResultStatus::Dispatched}; },
      [&](bool pause) {
        state.lifecycle = pause ? Lifecycle::Paused : Lifecycle::Running;
        ++state.pause_revision;
        pause ? ++pauses : ++resumes;
      });
  host.Submit({host.GetSnapshot()->generation, 1, Action::OpenMenu});
  QCoreApplication::processEvents();
  EXPECT_EQ(pauses, 1);
  EXPECT_EQ(state.lifecycle, Lifecycle::Paused);
  host.RequestBack();
  QCoreApplication::processEvents();
  EXPECT_EQ(resumes, 1);
  EXPECT_EQ(state.lifecycle, Lifecycle::Running);
}

TEST(BigPictureHostTest, ExternalPauseResumeAndFocusLossInvalidateMenuPauseOwnership)
{
  for (int change = 0; change < 3; ++change)
  {
    HostState state{Lifecycle::Running};
    int resumes = 0;
    BigPictureHost host(
        nullptr, [&] { return state; },
        [](const Command& command) { return Result{command, ResultStatus::Dispatched}; },
        [&](bool pause) {
          state.lifecycle = pause ? Lifecycle::Paused : Lifecycle::Running;
          ++state.pause_revision;
          if (!pause)
            ++resumes;
        });
    host.Submit({host.GetSnapshot()->generation, 1, Action::OpenMenu});
    QCoreApplication::processEvents();
    if (change == 0)
      ++state.pause_revision;  // Another pause request even though it was already paused.
    else if (change == 1)
    {
      state.lifecycle = Lifecycle::Running;
      ++state.pause_revision;
    }
    else
      host.InvalidatePauseOwnership();
    host.Refresh();
    host.RequestBack();
    QCoreApplication::processEvents();
    EXPECT_EQ(resumes, 0);
  }
}

TEST(BigPictureHostTest, AlreadyPausedAndNetplaySessionsNeverAcquireMenuPause)
{
  for (const bool netplay : {false, true})
  {
    HostState state{netplay ? Lifecycle::Running : Lifecycle::Paused, netplay};
    int changes = 0;
    BigPictureHost host(
        nullptr, [&] { return state; },
        [](const Command& command) { return Result{command, ResultStatus::Dispatched}; },
        [&](bool) { ++changes; });
    host.Submit({host.GetSnapshot()->generation, 1, Action::OpenMenu});
    QCoreApplication::processEvents();
    host.RequestBack();
    QCoreApplication::processEvents();
    EXPECT_EQ(changes, 0);
  }
}

TEST(BigPictureHostTest, InterveningFocusOrPauseCannotAcquireMenuPauseOwnership)
{
  for (const bool external_pause : {false, true})
  {
    HostState state{Lifecycle::Running};
    BigPictureHost* current = nullptr;
    int resumes = 0;
    BigPictureHost host(
        nullptr, [&] { return state; },
        [](const Command& command) { return Result{command, ResultStatus::Dispatched}; },
        [&](bool pause) {
          if (pause)
          {
            state.lifecycle = Lifecycle::Paused;
            ++state.pause_revision;
            if (external_pause)
              ++state.pause_revision;
            else
              current->InvalidatePauseOwnership();
          }
          else
            ++resumes;
        });
    current = &host;
    host.Submit({host.GetSnapshot()->generation, 1, Action::OpenMenu});
    QCoreApplication::processEvents();
    host.RequestBack();
    QCoreApplication::processEvents();
    EXPECT_EQ(resumes, 0);
  }
}
}  // namespace
