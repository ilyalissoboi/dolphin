# Declarative UI Slice 14: Triforce Settings

**Status:** In progress

**Goal:** Move the Triforce settings pane into a Qt Designer form while preserving controller
mapping dispatch and the existing IP-redirection editor and config behavior.

## Task 1: Define the complete pane form

- [ ] Add `TriforcePane.ui` with Controllers and IP Address Redirections groups plus the trailing
  stretch.
- [ ] Use stock Configure buttons with non-default behavior and an explicit tab order.
- [ ] Keep both groups aligned and usable at the settings pane's narrow width.

## Task 2: Preserve action dispatch

- [ ] Keep the controller action opening the AM Baseboard mapping window for device 0.
- [ ] Preserve delete-on-close, window-modal behavior, and non-blocking display for the mapping
  window.
- [ ] Keep the IP Address Redirections action opening its editor with delete-on-close behavior.

## Task 3: Preserve the IP-redirection editor

- [ ] Leave the existing dialog layout and table behavior unchanged for the later standalone
  dialog migration.
- [ ] Preserve the Emulated, Real, and Description columns, stretch sizing, edit triggers, and
  final empty row.
- [ ] Preserve automatic row insertion and removal, whitespace handling, incomplete-row skipping,
  descriptions, and comma-separated serialization.
- [ ] Preserve Default's immediate config reset, Clear's empty-row state, and OK-only saving.

## Task 4: Preserve compatibility

- [ ] Keep the existing eight translated msgids.
- [ ] Preserve `MAIN_TRIFORCE_IP_REDIRECTIONS` at its three existing read, save, and reset sites.
- [ ] Add a Qt form test for group order, button placement, button defaults, tab order, and narrow
  layout fit.
- [ ] Verify the `TriforcePane` constructor constructs no permanent widget or layout; the nested
  dialog remains outside this slice's form boundary.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review the pane and IP-redirection editor in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow pane layouts, tab traversal, mapping-window dispatch, and
  IP-redirection dialog actions on Windows.

## Definition of done

- [ ] The Designer form owns the complete Triforce settings pane layout and permanent controls.
- [ ] Both Configure actions and the IP-redirection editor retain their existing behavior.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
