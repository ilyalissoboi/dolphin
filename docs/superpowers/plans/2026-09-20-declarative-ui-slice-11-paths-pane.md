# Declarative UI Slice 11: Paths Settings

**Status:** In progress

**Goal:** Move the Paths settings pane into a Qt Designer form while preserving game-folder
management, recursive scanning, background refresh, default-game selection, user-path editing, and
all browse actions.

## Task 1: Define the complete form

- [ ] Add `PathPane.ui` with the Game Folders list, Add and Remove buttons, scan options, and six
  path rows.
- [ ] Preserve the existing vertical arrangement, list spacing, button defaults, and stretch
  behavior.
- [ ] Add buddies for every path label and an explicit tab order covering list, buttons,
  checkboxes, line edits, and browse buttons.
- [ ] Keep the layout usable at the settings pane's narrow width without horizontal clipping.

## Task 2: Bind settings and preserve path behavior

- [ ] Bind Search Subfolders and the default ISO to their existing typed config keys.
- [ ] Bind the Wii NAND, dump, load, resource-pack, and WFS fields through `BindUserPath`.
- [ ] Preserve background-refresh initialization and writes through `Settings`.
- [ ] Preserve browse starting locations, native path separators, file filters, and immediate
  config updates.

## Task 3: Preserve game-folder behavior

- [ ] Populate the list from `Settings::GetPaths` and retain PathAdded and PathRemoved updates.
- [ ] Keep Remove disabled without a selection and remove the current path through `Settings`.
- [ ] Preserve the recursive-scan game-list refresh side effect.
- [ ] Preserve Add and Remove as non-default dialog buttons.

## Task 4: Preserve compatibility

- [ ] Keep the existing 21 translated msgids and seven explicit config symbols.
- [ ] Preserve all seven concrete config locations represented by browse and binding sites.
- [ ] Add a Qt form test for structure, row placement, buddies, tab order, button defaults, list
  spacing, and narrow layout fit.
- [ ] Verify `PathPane.cpp` constructs no permanent widget or layout.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review Paths settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, scrolling, selection state, tab traversal, and path-field
  defaults on Windows.

## Definition of done

- [ ] The Designer form owns the complete Paths pane layout and permanent controls.
- [ ] Existing configuration, browsing, game-list side effects, and settings signals remain
  intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
