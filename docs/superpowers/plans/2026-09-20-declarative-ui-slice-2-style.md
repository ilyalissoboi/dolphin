# Declarative UI Slice 2: Application Style

**Status:** In progress

**Goal:** Replace the overlapping native and Fusion style choices with one cross-platform,
palette-driven set while preserving saved preferences and custom styles.

## Task 1: Extract testable theme definitions

- [x] Move the built-in palettes out of `Settings.cpp`.
- [x] Define the canonical System, Light, Dark Gray, Dark, and User style behavior in one helper.
- [x] Map the old Windows Light/Dark integer values to the equivalent palette themes.
- [x] Add Qt tests for compatibility mapping, style selection, color schemes, and palette contrast.

## Task 2: Apply themes consistently

- [x] Offer the same built-in style choices on every desktop platform.
- [x] Set Qt's application color scheme for fixed light and dark palettes.
- [x] Preserve the initial platform style and palette for System and custom user styles.
- [x] Clear Qt's pixmap cache after a theme change so tinted icons are regenerated.
- [x] Update Windows title bars when switching in either direction.

## Task 3: Retire the Windows dark stylesheet

- [x] Use the Dark Gray Fusion palette when Windows System mode needs a dark fallback.
- [x] Remove `dark.qss`, its image resources, and the Windows resource target.
- [x] Confirm custom `.qss` files still load and retain their tooltip override behavior.

## Task 4: Verify the slice

- [x] Run `qt-tests`, `tests`, and build `dolphin-emu` on macOS.
- [x] Record the intentional style-label gettext changes.
- [x] Review System, Light, Dark Gray, and Dark on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review light and dark main-window and settings controls on Windows.

## Progress recorded September 20, 2026

- macOS `dolphin-emu`, `qt-tests`, and `tests` built successfully.
- `qt-tests` passed all 98 tests in 11 suites.
- `tests` passed 1,183 tests in 77 suites, with the two environment-dependent shader preset
  tests skipped as expected.
- The `.ui` gettext fixture passed. The built-in labels `(Fusion Light)`,
  `(Fusion Dark Gray)`, and `(Fusion Dark)` were replaced by `(Light)`, `(Dark Gray)`,
  and `(Dark)`; `(Light)` and `(Dark)` retain their existing msgids.
- System, Light, Dark Gray, and Dark were reviewed in the main window and settings on macOS.
- An isolated custom stylesheet was loaded visibly in the main window. Its `QToolTip` selector
  remains authoritative because the fallback tooltip rule is only appended when that selector is
  absent.

## Definition of done

- [ ] One palette-driven built-in style set is available on macOS and Windows.
- [x] Existing stored style values continue to select their nearest equivalent.
- [ ] System mode follows Windows dark mode without `dark.qss`.
- [ ] Theme changes refresh icons and Windows title-bar state.
- [ ] macOS and Windows builds, tests, gettext checks, and visual review pass.
