# Declarative UI Slice 22: FIFO

**Status:** In progress

**Goal:** Move the FIFO Player and FIFO Analyzer static layouts to Qt Designer forms without
changing playback, recording, decoding, or search behavior.

The approved design groups FIFO with the standalone-dialog migrations. Achievements and NetPlay
used slices 20 and 21 on this branch, so FIFO continues as slice 22.

## Task 1: Preserve the behavior boundary

- [x] Keep FIFO loading, saving, playback, recording, callbacks, range updates, and emulation-state
  handling in C++.
- [x] Keep analyzer tree population, command decoding, descriptions, search results, and debug-font
  updates in C++.
- [x] Preserve window geometry, splitter state, feature settings, button roles, and Escape-to-hide
  behavior.
- [x] Move only static structure, widget properties, labels, tab order, and layout positioning to
  `.ui` files.

## Task 2: Migrate the static layouts

- [ ] Add a Designer form for the FIFO Player shell, play/record page, groups, range controls,
  recording controls, and button box.
- [ ] Add a Designer form for the FIFO Analyzer tree, detail list, description browser, search
  splitters, and search controls.
- [ ] Keep the analyzer tab and custom action-role buttons populated in C++ because they require
  runtime objects and `QDialogButtonBox` roles.
- [ ] Replace legacy tooltip checkboxes with stock `QCheckBox` controls and the generic persistent
  description helper.
- [ ] Express non-default search-button behavior as stock `QPushButton` properties in the form.
- [ ] Remove all C++ layout construction from `DolphinQt/FIFO`.

## Task 3: Cover the form structure

- [ ] Add Qt tests for the player tabs, groups, ranges, defaults, button box, analyzer splitters,
  search controls, and keyboard order.
- [ ] Add the FIFO forms to DolphinQt and Qt test AUTOUIC inputs.
- [ ] Run translation extraction and preserve existing strings and descriptions.

## Task 4: Verify and clean up

- [ ] Build DolphinQt and both test binaries on macOS and run both test suites.
- [ ] Review the play/record and analyzer tabs on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review the play/record and analyzer tabs on Windows.
- [ ] Remove temporary profiles, scripts, captures, and transferred source changes from both
  hosts.

## Definition of done

- [ ] No `new Q*Layout` construction remains in `DolphinQt/FIFO`.
- [ ] FIFO retains its existing behavior, strings, dynamic data, settings, and persistence.
- [ ] Both forms open as stock Qt widgets in Designer.
- [ ] macOS and Windows builds, tests, extraction, and native review pass.
