# Declarative UI Slice 9: On-Screen Display Settings

**Status:** In progress

**Goal:** Move the On-Screen Display settings pane into a Qt Designer form while preserving
message, performance, movie, NetPlay, debug-overlay, help, and movie-window dependency behavior.

This slice covers the desktop settings pane only. The fullscreen OSD remains a separate project
as defined by the desktop UI design.

## Task 1: Define the complete form

- [ ] Add `OnScreenDisplayPane.ui` with General, Performance Statistics, Movie Window, Netplay,
  and Debug sections.
- [ ] Keep the existing two-column arrangement within each section.
- [ ] Define the font-size and performance-sample ranges and steps in the form.
- [ ] Add explicit buddies and tab order for keyboard navigation.

## Task 2: Bind settings and preserve behavior

- [ ] Bind all stock controls to the existing 21 typed config keys.
- [ ] Preserve the movie-window dependency for rerecord, lag, frame, input, and clock controls.
- [ ] Move every existing tooltip title and description to `ConfigWidget::SetDescription`.
- [ ] Preserve the settings registry entries produced by the former config-control subclasses.

## Task 3: Preserve compatibility

- [ ] Keep every existing key, label, range, step, default, and translated msgid.
- [ ] Add a Qt form test for section order, grid placement, ranges, buddies, tab order, and narrow
  layout fit.
- [ ] Verify `OnScreenDisplayPane.cpp` constructs no permanent widget or layout.

## Task 4: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config-key comparisons.
- [ ] Review On-Screen Display settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, scrolling, tab traversal, and movie-window disabled states
  on Windows.

## Definition of done

- [ ] The Designer form owns the complete On-Screen Display pane layout and permanent controls.
- [ ] Existing configuration, help, ranges, and dependency behavior remain intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
