# Declarative UI Slice 17: Settings Layout Polish

**Status:** In Progress

**Goal:** Make the shared settings shell and its widest pages fit predictably, align the help panel
with the PCSX2 settings layout, and use a horizontal volume control.

## Task 1: Rework the shared settings shell

- [ ] Keep the navigation and active page in the top row.
- [ ] Move the help panel below both columns so it spans the full window width.
- [ ] Give the help panel balanced left and right gutters, and place the footer below it.
- [ ] Update the shell form test for the new grid structure, spans, and margins.

## Task 2: Fit controller settings at the default width

- [ ] Open the global settings window at a screen-bounded preferred width that accommodates the
  controller rows.
- [ ] Prevent dynamic controller and adapter names from increasing combo-box minimum widths.
- [ ] Extend controller form tests to cover compact combo sizing and narrow row fit.

## Task 3: Make volume consistent with other sliders

- [ ] Place the Volume group in the main Audio column.
- [ ] Change the volume slider to horizontal and keep its value label aligned beside it.
- [ ] Update the Audio form test for the revised structure and narrow layout.

## Task 4: Verify and clean up

- [ ] Build the app and both test binaries on macOS and run both test suites.
- [ ] Run the UI extraction regression test.
- [ ] Review Controllers, Audio, and the full-width help panel in light and dark themes.
- [ ] Build and run the app and both test binaries on Windows and review the revised layouts.
- [ ] Remove validation scratch and leave both checkouts clean.

## Definition of done

- [ ] Controllers are fully visible at the default settings width.
- [ ] Volume uses the same horizontal interaction pattern as other sliders.
- [ ] The help panel spans the complete settings window with balanced edge padding.
- [ ] macOS and Windows builds, tests, and visual review pass.
