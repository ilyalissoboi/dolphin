# Declarative UI Slice 7: Controller Settings

**Status:** In progress

**Goal:** Move the Controllers settings pane and its GameCube, Wii Remote, and common-input
sections into Qt Designer forms while preserving device selection, mapping-window launches,
Bluetooth passthrough, asynchronous refresh actions, emulation-state restrictions, and config
saves.

PCSX2's controller pages model PlayStation-specific devices and do not provide a useful parity
feature for Dolphin's GameCube and Wii hardware. This slice is therefore a behavior-preserving
form migration, consistent with the design's explicit exclusion of controller-mapping parity
features.

## Task 1: Migrate the pane shell

- [ ] Add `ControllersPane.ui` for the GameCube, Wii Remote, and common-input section order.
- [ ] Keep construction of the three section controllers in C++ and place them into form-owned
  layout slots.
- [ ] Preserve the shared settings scroll wrapper and top-aligned sizing.
- [ ] Add a Qt form test for the permanent pane structure.

## Task 2: Migrate GameCube controller selection

- [ ] Add `GamecubeControllersWidget.ui` for the four port rows.
- [ ] Keep platform-dependent device choices and translated numbered port labels in C++.
- [ ] Preserve device-to-menu mappings, Configure availability, NetPlay restrictions, config
  writes, and every mapping-dialog type.
- [ ] Replace the custom non-default buttons with stock form buttons whose `autoDefault` property
  is disabled.

## Task 3: Migrate Wii Remote selection

- [ ] Add `WiimoteControllersWidget.ui` for passthrough, emulated remotes, shared options, and
  refresh controls.
- [ ] Keep Bluetooth adapter discovery and device entries dynamic.
- [ ] Keep the Windows-only host Sync and Reset menu actions dynamic.
- [ ] Preserve all emulation-state, NetPlay, controller-interface, continuous-scanning, and
  Balance Board dependencies.
- [ ] Preserve asynchronous refresh indicator behavior and shutdown synchronization.

## Task 4: Migrate common input controls

- [ ] Add `CommonControllersWidget.ui` for Background Input and the two standalone configuration
  dialogs.
- [ ] Preserve explicit config persistence and both window-launch paths.
- [ ] Preserve the translator note for the Common group title.

## Task 5: Preserve compatibility

- [ ] Keep every existing config key and device enum mapping.
- [ ] Keep all labels, group titles, combo choices, action text, and dialog text.
- [ ] Keep config-change and emulation-state refresh connections.
- [ ] Record the targeted gettext msgid comparison.
- [ ] Verify no migrated section constructs a permanent control or layout in C++.

## Task 6: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite.
- [ ] Review Controller settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, keyboard traversal, disabled states, and platform-only
  refresh actions on Windows.

## Definition of done

- [ ] Four Designer forms own the Controllers pane and section layouts.
- [ ] Device discovery, dynamic menu contents, and behavior remain in C++.
- [ ] Existing controller selection, mapping, config persistence, and runtime restrictions remain
  intact.
- [ ] macOS and Windows builds, tests, gettext checks, and visual review pass.
