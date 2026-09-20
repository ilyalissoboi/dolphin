# Declarative UI Slice 13: Wii Settings

**Status:** In progress

**Goal:** Move the Wii settings pane into a Qt Designer form while preserving SYSCONF choices,
SD-card paths and conversion actions, USB passthrough whitelisting, Wii Remote settings, and
emulation-state restrictions.

## Task 1: Define the complete form

- [ ] Add `WiiPane.ui` with Misc Settings, SD Card Settings, Whitelisted USB Passthrough Devices,
  and Wii Remote Settings groups.
- [ ] Author the static aspect-ratio, system-language, sound-mode, SD-size, and sensor-position
  choices in the form.
- [ ] Use stock checkboxes, combo boxes, sliders, line edits, and buttons plus the existing
  minimum-size list wrapper, with buddies, non-default action buttons, and an explicit tab order.
- [ ] Keep the complete pane usable at the settings pane's narrow width without horizontal
  clipping.

## Task 2: Bind settings and descriptions

- [ ] Bind the four Misc checkboxes and the three mapped or indexed choices to their existing
  typed settings.
- [ ] Bind both SD paths through `BindUserPath`, the three SD checkboxes, and the mapped SD-size
  choice.
- [ ] Bind the Wii Remote rumble checkbox, mapped sensor position, IR sensitivity, and speaker
  volume.
- [ ] Preserve all existing setting descriptions and label font mirroring.

## Task 3: Preserve SD-card and USB behavior

- [ ] Preserve SD image and sync-folder browse starting locations, filters, native separators,
  effective user paths, and immediate config updates.
- [ ] Preserve the pack and unpack confirmation text, asynchronous conversion, cancellation,
  progress dialog, and failure reporting.
- [ ] Preserve USB whitelist population, display strings, selection-dependent Remove state,
  duplicate rejection, and Add and Remove writes.
- [ ] Keep every browse, conversion, Add, and Remove button non-default.

## Task 4: Preserve runtime restrictions

- [ ] Keep the same SYSCONF, WiiLink, SD conversion, and Wii Remote controls disabled while
  emulation is running.
- [ ] Keep Insert SD Card, Allow Writes, folder sync, SD paths, SD size, USB whitelist, and USB
  keyboard behavior unchanged while emulation is running.
- [ ] Preserve config-change refreshes and initial state setup.

## Task 5: Preserve compatibility

- [ ] Keep the existing 70 translated msgids and 17 explicit config symbols.
- [ ] Preserve all 18 concrete config locations plus the USB whitelist read and write paths.
- [ ] Add a Qt form test for section order, authored choices, row placement, buddies, ranges,
  initial button state, tab order, and narrow layout fit.
- [ ] Verify `WiiPane.cpp` constructs no permanent widget or layout.

## Task 6: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review Wii settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, scrolling, USB selection state, tab traversal, and setting
  dependencies on Windows.

## Definition of done

- [ ] The Designer form owns the complete Wii pane layout and permanent controls.
- [ ] Existing bindings, browsing, conversion, whitelist behavior, help, and runtime restrictions
  remain intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
