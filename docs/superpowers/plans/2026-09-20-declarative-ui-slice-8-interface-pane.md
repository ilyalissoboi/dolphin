# Declarative UI Slice 8: Interface Settings

**Status:** In progress

**Goal:** Move the Interface settings pane into a Qt Designer form while preserving language,
theme, style, game-library metadata, debugging, render-window, cursor, and emulation-state
behavior.

The visible PCSX2-style theme and settings-shell work already landed in Slices 2 and 4. This slice
therefore keeps Dolphin's Interface feature set and moves its permanent structure to the shared
declarative UI system.

## Task 1: Migrate the User Interface section

- [ ] Add `InterfacePane.ui` with Language, Theme, and Style rows and all user-interface
  checkboxes.
- [ ] Keep language, theme-directory, and custom-style discovery dynamic in C++.
- [ ] Bind stock form controls to the existing typed settings.
- [ ] Preserve the language restart notice, immediate style application, title-database refresh,
  and cover metadata refresh.

## Task 2: Migrate the Render Window section

- [ ] Define all render-window checkboxes and the nested Mouse Cursor Visibility group in the
  form.
- [ ] Bind the three cursor radio buttons to the existing enum values.
- [ ] Keep Lock Mouse Cursor hidden outside Windows and preserve its signal path on Windows.
- [ ] Preserve Keep Window on Top, Confirm on Stop, panic handler, active-title, and focus-loss
  behavior.

## Task 3: Preserve runtime restrictions and help

- [ ] Keep Debugging UI synchronized with the debug-mode state and disabled in Hardcore Mode.
- [ ] Keep play-time tracking disabled while emulation is active.
- [ ] Move every existing tooltip title and description to `ConfigWidget::SetDescription`.
- [ ] Preserve the settings registry entries produced by the former config-control subclasses.

## Task 4: Preserve compatibility

- [ ] Keep every existing config key, style value, cursor enum mapping, label, and dynamic option.
- [ ] Preserve the existing translated msgids and the non-Latin language display names.
- [ ] Add a Qt form test for section order, form buddies, stock controls, tab order, and narrow
  layout fit.
- [ ] Verify `InterfacePane.cpp` constructs no permanent widget or layout.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config-key comparisons.
- [ ] Review Interface settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, tab traversal, dynamic choices, disabled states, and the
  Windows-only Lock Mouse Cursor control.

## Definition of done

- [ ] The Designer form owns the complete Interface pane layout and permanent controls.
- [ ] Dynamic discovery and runtime behavior remain in C++.
- [ ] Existing configuration, help, theme, refresh, and emulation-state behavior remain intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
