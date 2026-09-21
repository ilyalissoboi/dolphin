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

- [x] Add Designer forms for the setup dialog, session browser, room dialog, game selector,
  controller mapping, digest progress, and chunked-transfer progress.
- [x] Promote `ClickBlurLabel` and express the existing non-default button behavior as stock
  `QPushButton` properties in the forms.
- [x] Keep dynamic progress rows and table contents in C++ while placing their containers in the
  forms.
- [x] Remove all C++ layout construction from `DolphinQt/NetPlay`.

## Task 3: Cover the form structure

- [x] Add Qt tests for each dialog shell, the setup tabs, filter controls, room menus and panes,
  mapping rows, dynamic progress containers, and keyboard order.
- [x] Add the NetPlay forms to the Qt test AUTOUIC search path.
- [x] Run translation extraction and preserve existing strings and translator comments.

## Task 4: Verify and clean up

- [x] Build DolphinQt and both test binaries on macOS and run both test suites.
- [x] Review the NetPlay setup, browser, and room surfaces on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review the NetPlay setup, browser, and room surfaces on Windows.
- [ ] Remove temporary profiles, scripts, captures, and transferred source changes from both
  hosts.

## Definition of done

- [x] No `new Q*Layout` construction remains in `DolphinQt/NetPlay`.
- [ ] NetPlay retains its existing behavior, strings, dynamic population, and feature flags.
- [x] Every new form opens as stock or explicitly promoted Qt widgets in Designer.
- [ ] macOS and Windows builds, tests, extraction, and native review pass.
