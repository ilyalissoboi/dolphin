# Declarative UI Slice 16: Per-Game Properties

**Status:** In Progress

**Goal:** Move the per-game Game Config page and INI editor into Qt Designer forms, keep the
existing local and shipped game-INI behavior, and make inherited values explicit in layered
controls.

## Task 1: Record the current contract

- [x] Identify the ten bespoke General-tab config keys and the existing local-over-system-over-base
  display priority.
- [x] Confirm that the Graphics tab already reuses the four declarative graphics panes with a
  per-game `Config::Layer`.
- [x] Confirm the existing editor refresh, external-editor, completion, highlighting, save, and
  tab-switch behavior.

## Task 2: Define the per-game forms

- [ ] Add `GameConfigWidget.ui` with the warning row, General groups, Graphics host, Editor groups,
  tab order, slider ranges, label buddies, and narrow-window layout.
- [ ] Add `GameConfigEdit.ui` with the refresh and external-editor buttons plus the plain-text
  editor.
- [ ] Keep dynamically discovered INI tabs and the existing graphics pane as runtime children of
  layouts authored in the forms.

## Task 3: Migrate and preserve behavior

- [ ] Bind the General controls through `ConfigWidgetBinder`, including reverse disc-speed
  semantics, deterministic-dual-core values, float mappings, descriptions, value labels, and font
  mirrors.
- [ ] Preserve shipped game settings as the inherited value and italic marker, local overrides as
  bold, and right-click clearing.
- [ ] Preserve editor synchronization when entering or leaving the raw INI tab, active-game layer
  reload, and empty-local-INI cleanup.
- [ ] Add form tests for structure, authored properties, focus order, and narrow layout fit.

## Task 4: Expose inherited values

- [ ] Add layered binding support for an optional inherited game layer before falling back to the
  base setting.
- [ ] Show an inherited checkbox as partially checked and a layered combo box as
  `Use Global Setting [X]`; selecting an explicit value creates an override.
- [ ] Show inherited slider and spin-box values in place, create an override on editing, and keep
  right-click clearing.
- [ ] Cover inherited-state display, edits, clearing, reverse booleans, mapped choices, and
  no-write refresh behavior in binder tests.

## Task 5: Verify the slice

- [ ] Retain the existing Game Config and editor translated msgids, apart from the intentional
  inherited-setting strings.
- [ ] Retain the same bespoke General-tab config keys and preserve the Graphics binding registry.
- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS and run both test binaries.
- [ ] Review the page and inherited/overridden states in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows, then review wide, narrow, light, and dark states.
- [ ] Remove local and Windows validation scratch while leaving both source checkouts clean.

## Definition of done

- [ ] The forms own every permanent Game Config and INI editor widget and layout.
- [ ] Per-game controls clearly distinguish inherited, shipped game, and user values.
- [ ] Existing per-game settings, graphics behavior, raw editing, and cleanup workflows remain
  intact.
- [ ] macOS and Windows builds, tests, audits, and native review pass.
