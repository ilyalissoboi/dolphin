# Declarative UI Slice 21: NetPlay

**Status:** In progress

**Goal:** Move the NetPlay setup, browser, room, mapping, selection, digest, and transfer dialogs
to Qt Designer forms without changing networking behavior.

The approved design groups NetPlay into the standalone-dialog phase. Achievements used slice 20 on
this branch, so NetPlay continues as slice 21.

## Task 1: Preserve the behavior boundary

- [x] Keep connection, hosting, refresh-thread, room, chat, player, and synchronization behavior in
  C++.
- [x] Keep game, player, region, session, progress, and table contents dynamically populated.
- [x] Keep existing feature-flag, configuration, geometry, splitter, menu, and confirmation
  behavior.
- [x] Move only static structure, widget properties, actions, menus, and layout positioning to
  `.ui` files.

## Task 2: Migrate the static layouts

- [ ] Add Designer forms for the setup dialog, session browser, room dialog, game selector,
  controller mapping, digest progress, and chunked-transfer progress.
- [ ] Promote the existing `NonDefaultQPushButton` and `ClickBlurLabel` controls where their
  behavior is part of the existing UI.
- [ ] Keep dynamic progress rows and table contents in C++ while placing their containers in the
  forms.
- [ ] Remove all C++ layout construction from `DolphinQt/NetPlay`.

## Task 3: Cover the form structure

- [ ] Add Qt tests for each dialog shell, the setup tabs, filter controls, room menus and panes,
  mapping rows, dynamic progress containers, and keyboard order.
- [ ] Add the NetPlay forms to the Qt test AUTOUIC search path.
- [ ] Run translation extraction and preserve existing strings and translator comments.

## Task 4: Verify and clean up

- [ ] Build DolphinQt and both test binaries on macOS and run both test suites.
- [ ] Review the NetPlay setup, browser, and room surfaces on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review the NetPlay setup, browser, and room surfaces on Windows.
- [ ] Remove temporary profiles, scripts, captures, and transferred source changes from both
  hosts.

## Definition of done

- [ ] No `new Q*Layout` construction remains in `DolphinQt/NetPlay`.
- [ ] NetPlay retains its existing behavior, strings, dynamic population, and feature flags.
- [ ] Every new form opens as stock or explicitly promoted Qt widgets in Designer.
- [ ] macOS and Windows builds, tests, extraction, and native review pass.
