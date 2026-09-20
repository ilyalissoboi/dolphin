# Declarative UI Slice 5: General Settings

**Status:** In progress

**Goal:** Move the General settings pane into a Qt Designer form, replace its plain config-widget
subclasses with bind-after-construction controls, and use a compact checkbox grid while preserving
every setting, runtime restriction, side effect, description, and translated string.

## Task 1: Migrate the existing pane

- [ ] Add `GeneralPane.ui` for the four settings groups and their controls.
- [ ] Convert `GeneralPane` into the form's controller and remove its C++ layout construction.
- [ ] Keep unsupported auto-update, Discord, and analytics controls absent from the visible layout.
- [ ] Preserve control order, label buddies, keyboard traversal, and scroll sizing.
- [ ] Add a Qt form test for the pane's permanent structure.

## Task 2: Move plain config controls to the binder

- [ ] Bind Dual Core, Cheats, Load Whole Game Into Memory, Mismatched Region Settings, and
  Automatic Disc Changes through `ConfigWidget::Bind`.
- [ ] Move their help text to `ConfigWidget::SetDescription` so balloon and persistent help use
  the same metadata.
- [ ] Keep speed-limit rounding in the controller because arbitrary existing float values are
  rounded to the nearest displayed tenth.
- [ ] Keep update-track, fallback-region, analytics, and Discord handling in the controller because
  they use `Settings` storage, enum/string mappings, or runtime side effects.
- [ ] Record the binding-coverage and `.pot` msgid diffs.

## Task 3: Add General-pane layout parity

- [ ] Arrange the Basic Settings checkboxes in a compact two-column grid, following the useful
  grouping pattern in PCSX2's Designer-authored emulation pane without copying its source or text.
- [ ] Keep Speed Limit as a labeled full-width row below the checkbox grid.
- [ ] Preserve the logical tab order independently of visual row placement.
- [ ] Verify the grid collapses cleanly inside the existing scroll wrapper at narrow widths.

## Task 4: Preserve compatibility

- [ ] Keep the existing config keys and default values.
- [ ] Keep the `EnableCheatsChanged`, update-track, fallback-region, analytics, Discord, and
  identity-generation behavior.
- [ ] Keep all emulation-state restrictions and Hardcore Mode speed-limit guidance.
- [ ] Keep the current group titles, control labels, combo choices, and descriptions.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and compare the source catalog.
- [ ] Review General settings in light and dark themes at wide and narrow sizes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review layout, help, focus order, disabled-while-running behavior, and narrow sizing on
  Windows.

## Definition of done

- [ ] `GeneralPane.ui` owns all General-pane layout and permanent controls.
- [ ] Every former `ConfigBool` construction site uses the stock-widget binder.
- [ ] The Basic Settings grid is compact, readable, and keyboard accessible.
- [ ] Existing config semantics, side effects, help, and translations are retained.
- [ ] macOS and Windows builds, tests, gettext checks, and visual review pass.
