# Declarative UI Slice 17: Settings Layout Polish

**Status:** Complete

**Goal:** Make the shared settings shell and its widest pages fit predictably, align the help panel
with the PCSX2 settings layout, and use a horizontal volume control.

## Task 1: Rework the shared settings shell

- [x] Keep the navigation and active page in the top row.
- [x] Move the help panel below both columns so it spans the full window width.
- [x] Give the help panel balanced left and right gutters, and place the footer below it.
- [x] Update the shell form test for the new grid structure, spans, and margins.

## Task 2: Fit controller settings at the default width

- [x] Open the global settings window at a screen-bounded preferred width that accommodates the
  controller rows.
- [x] Prevent dynamic controller and adapter names from increasing combo-box minimum widths.
- [x] Extend controller form tests to cover compact combo sizing and narrow row fit.

## Task 3: Make volume consistent with other sliders

- [x] Place the Volume group in the main Audio column.
- [x] Change the volume slider to horizontal and keep its value label aligned beside it.
- [x] Update the Audio form test for the revised structure and narrow layout.

## Task 4: Verify and clean up

- [x] Build the app and both test binaries on macOS and run both test suites.
- [x] Run the UI extraction regression test.
- [x] Review Controllers, Audio, and the full-width help panel in light and dark themes.
- [x] Build and run the app and both test binaries on Windows and review the revised layouts.
- [x] Remove validation scratch and leave both checkouts clean.

## Definition of done

- [x] Controllers are fully visible at the default settings width.
- [x] Volume uses the same horizontal interaction pattern as other sliders.
- [x] The help panel spans the complete settings window with balanced edge padding.
- [x] macOS and Windows builds, tests, and visual review pass.

## Verification

- Implementation: `96005327810f627f371fd9867f20f6c55cead8fc`
- macOS: DolphinQt and both test targets built; 130 Qt tests passed; 1,185 core tests
  passed and 2 preset-dependent tests skipped; UI extraction passed.
- Windows: DolphinQt and both test targets built; 130 Qt tests passed; 1,507 core tests
  passed and 2 preset-dependent tests skipped.
- Native light and dark renders on both platforms confirmed the controller rows fit the preferred
  width, Volume is horizontal, and the help panel spans the full window with edge gutters.
- Temporary users, bundles, scripts, captures, and the Windows-only render hook were removed.
