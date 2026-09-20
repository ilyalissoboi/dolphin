# Declarative UI Slice 4: Settings Shell

**Status:** In progress

**Goal:** Move the shared settings-window shell into a Qt Designer form and add compact category
icons plus persistent setting help while preserving pane order, navigation, theme handling,
window sizing, and the game-properties use of the same shell.

## Task 1: Migrate the existing shell

- [x] Add `SettingsWindow.ui` for the navigation list, pane stack, help area, and footer.
- [x] Convert `StackedSettingsWindow` into the form's controller.
- [x] Preserve settings and game-properties navigation, close behavior, theme updates, and sizing.
- [x] Add a Qt form test and verify the behavior-preserving migration.

## Task 2: Add settings-shell parity

- [ ] Add category icons from Dolphin-owned resources and native Qt standard icons.
- [ ] Show category guidance in a persistent help pane.
- [ ] Show control descriptions on pointer hover and keyboard focus.
- [ ] Keep existing balloon tooltips available while exposing the same metadata to future
  binder-authored panes.

## Task 3: Preserve compatibility

- [ ] Keep the existing pane order and `SettingsWindowPaneIndex` values.
- [ ] Keep `PropertiesDialog` on the shared shell without global-settings category guidance.
- [ ] Record the `.pot` msgid diff and retain all moved shell strings.
- [ ] Keep every existing settings binding and pane implementation unchanged.

## Task 4: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Review settings and game-properties shells in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review icons, help updates, focus, keyboard traversal, and narrow-window behavior on Windows.

## Verification notes

Pending.

## Definition of done

- [ ] `SettingsWindow.ui` owns the shared settings-window layout.
- [ ] Settings categories have clear Dolphin-owned or native icons.
- [ ] Category and control help remain visible without replacing existing balloon tooltips.
- [ ] Global settings and game properties retain their existing pane behavior.
- [ ] macOS and Windows builds, tests, gettext checks, and visual review pass.
