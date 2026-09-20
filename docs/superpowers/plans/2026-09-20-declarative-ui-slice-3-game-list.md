# Declarative UI Slice 3: Game Library

**Status:** In progress

**Goal:** Move the game-library shell into a Qt Designer form and add compact, always-available
view and filtering controls while preserving the existing list, grid, menu, launch, and empty
library behavior.

## Task 1: Migrate the existing shell

- [x] Add `GameListWidget.ui` for the view stack and current search controls.
- [x] Convert `GameList` from a `QStackedWidget` into the form's controller.
- [x] Remove the separate C++-authored `SearchBar` layout.
- [x] Preserve `Ctrl+F`, Escape, Close, search filtering, and the empty-library double-click action.
- [x] Add a Qt form test and verify the behavior-preserving migration.

## Task 2: Add the library control row

- [ ] Keep search visible and make `Ctrl+F` focus and select it.
- [ ] Add accessible List and Grid buttons synchronized with the View menu.
- [ ] Add transient platform and region quick filters without replacing the existing multi-select
  View menu filters.
- [ ] Add a grid-scale slider synchronized with the existing zoom shortcuts and saved scale.
- [ ] Keep fixed view controls visible while search and filter fields compress at narrow widths.

## Task 3: Preserve compatibility

- [ ] Keep saved preferred-view, grid-scale, table-header, and column-visibility settings.
- [ ] Keep all existing platform and country visibility config keys in the filtering path.
- [ ] Record the `.pot` msgid diff and retain moved search strings.
- [ ] Keep list/grid selection, sorting, context menus, keyboard launch, and game-count updates.

## Task 4: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Review light and dark list, grid, empty, filtered, and narrow-window states on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review light and dark controls, keyboard traversal, focus, and narrow-window behavior on
  Windows.

## Definition of done

- [ ] `GameListWidget.ui` owns the game-library layout.
- [ ] Search, platform, and region controls are always available.
- [ ] View buttons, View menu actions, slider, and zoom shortcuts stay synchronized.
- [ ] Existing saved settings and multi-select menu filters retain their meaning.
- [ ] macOS and Windows builds, tests, gettext checks, and visual review pass.
