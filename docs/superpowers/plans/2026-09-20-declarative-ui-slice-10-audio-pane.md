# Declarative UI Slice 10: Audio Settings

**Status:** In progress

**Goal:** Move the Audio settings pane into a Qt Designer form while preserving DSP selection,
backend and device discovery, volume and latency controls, playback behavior, and Wii Remote audio
routing.

## Task 1: Define the complete form

- [ ] Add `AudioPane.ui` with DSP Options, Backend Settings, Audio Playback Settings, Wii Remote
  Audio Routing, and Volume sections.
- [ ] Keep the existing main-column and vertical-volume arrangement.
- [ ] Define slider ranges, steps, labels, buddies, and explicit tab order in the form.
- [ ] Keep platform- and backend-dependent rows form-owned and hide them when unavailable.

## Task 2: Bind settings and populate dynamic choices

- [ ] Bind the DSP engine combo to its existing HLE/JIT setting pair.
- [ ] Populate and bind the audio backend, WASAPI device, and Wii Remote device combos at runtime.
- [ ] Bind Dolby quality through its existing enum values and bind all stock checkboxes and
  sliders to the existing settings.
- [ ] Preserve the volume, latency, and audio-buffer value labels and the buffer's 8 ms stepping.

## Task 3: Preserve runtime behavior

- [ ] Preserve backend-dependent Dolby, latency, volume, and WASAPI states.
- [ ] Preserve controls disabled while emulation is active.
- [ ] Preserve Wii Remote routing requirements for Cubeb, speaker data, Bluetooth passthrough,
  emulated remotes, and per-remote enablement.
- [ ] Move all existing help titles and descriptions to `ConfigWidget::SetDescription`.

## Task 4: Preserve compatibility

- [ ] Keep the existing 48 translated msgids and 17 explicit config symbols.
- [ ] Preserve all 21 concrete setting locations represented by the platform and per-remote
  bindings.
- [ ] Add a Qt form test for section order, placement, ranges, steps, buddies, tab order, and
  narrow layout fit.
- [ ] Verify `AudioPane.cpp` constructs no permanent widget or layout.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review Audio settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, dynamic choices, scrolling, tab traversal, and dependency
  states on Windows.

## Definition of done

- [ ] The Designer form owns the complete Audio pane layout and permanent controls.
- [ ] Existing discovery, configuration, help, side effects, and runtime restrictions remain
  intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
