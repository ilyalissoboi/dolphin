# Declarative UI Slice 20: Achievements

**Status:** In progress

**Goal:** Move the standalone Achievements window and its child widgets to Qt Designer forms
without changing RetroAchievements behavior.

The approved design called this standalone-dialog migration slice 19. The branch used slice 17 for
requested settings layout polish, slice 18 for the setup wizard, and slice 19 for cover management,
so this plan continues with slice 20.

## Task 1: Preserve the behavior boundary

- [x] Keep account login, logout, configuration, and confirmation behavior in C++.
- [x] Keep achievement and leaderboard data population dynamic.
- [x] Keep existing tab visibility, scrolling, update, and feature-flag behavior.
- [x] Move only static structure, widget properties, and layout positioning to `.ui` files.

## Task 2: Migrate the static layouts

- [ ] Add Designer forms for the Achievements window, settings, header, achievement box, progress,
  and leaderboard widgets.
- [ ] Replace settings-only tooltip checkbox subclasses with stock checkboxes and attach the same
  descriptions through `ConfigWidget`.
- [ ] Add a reusable Designer-authored leaderboard cell so dynamic rows do not construct nested
  layouts in C++.
- [ ] Remove all C++ layout construction from `DolphinQt/Achievements`.

## Task 3: Cover the form structure

- [ ] Add Qt tests for the window shell, settings controls and tab order, progress overlays,
  dynamic-content layouts, and reusable leaderboard cells.
- [ ] Add the Achievements forms to the Qt test AUTOUIC search path.
- [ ] Run translation extraction and preserve existing strings and translator comments.

## Task 4: Verify and clean up

- [ ] Build DolphinQt and both test binaries on macOS and run both test suites.
- [ ] Review the logged-out Achievements window and its tabs on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review the Achievements window on Windows.
- [ ] Remove temporary profiles, scripts, captures, and transferred source changes from both hosts.

## Definition of done

- [ ] No `new Q*Layout` construction remains in `DolphinQt/Achievements`.
- [ ] The Achievements window retains its existing behavior and strings.
- [ ] Every new form opens as stock Qt widgets in Designer.
- [ ] macOS and Windows builds, tests, extraction, and native review pass.
