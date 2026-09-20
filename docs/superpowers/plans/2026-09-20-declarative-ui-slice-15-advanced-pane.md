# Declarative UI Slice 15: Advanced Settings

**Status:** In progress

**Goal:** Move the Advanced settings pane into a Qt Designer form while preserving its dynamic
CPU-core choices, config binding, runtime value labels, emulation-state restrictions, and reset
workflow.

## Task 1: Define the complete pane form

- [ ] Add `AdvancedPane.ui` with CPU Options, Timing, Clock Override, VBI Frequency Override,
  Memory Override, Custom RTC Options, and Reset Dolphin Settings groups plus the trailing stretch.
- [ ] Author the CPU-engine label and combo, all checkboxes, both float sliders, both scaled memory
  sliders, their value labels, the UTC date-time editor, and the reset button as stock widgets.
- [ ] Preserve the group and control order, slider ranges, non-default reset button behavior,
  explicit tab order, and narrow-pane fit.

## Task 2: Bind the declarative controls

- [ ] Populate the platform-dependent CPU engine choices from `PowerPC::AvailableCPUCores()` and
  bind their enum values with the runtime mapped-combo overload.
- [ ] Bind the boolean settings, 1–500% clock sliders, and 24–64 MiB and 64–128 MiB memory sliders
  with the stock-widget config binder.
- [ ] Preserve the existing descriptions and translator context for the timing controls.

## Task 3: Preserve dynamic behavior

- [ ] Keep the CPU and VBI percentage labels synchronized with their sliders and current emulated
  clock or refresh rate.
- [ ] Keep CPU-engine, MMU, and panic controls locked while emulation runs; preserve the existing
  write-back-cache behavior.
- [ ] Enable clock and VBI sliders only with their respective overrides, and enable memory sliders,
  custom RTC editing, and reset only when their existing state rules allow it.
- [ ] Preserve the custom RTC display format, four-digit year, UTC interpretation, 2000–2099 range,
  and epoch-seconds storage.

## Task 4: Preserve reset and compatibility behavior

- [ ] Keep the reset confirmation text, Yes/No buttons, default No choice, window modality, settings
  reset, user-directory refresh, config-change notification, and optional analytics prompt.
- [ ] Retain all 38 translated msgids and the existing 16 config keys.
- [ ] Add a Qt form test for group order, control placement, slider ranges, label buddies, button
  defaults, tab order, and narrow layout fit.
- [ ] Verify `AdvancedPane.cpp` constructs no permanent widget or layout.

## Task 5: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review the full pane, dependent enabled states, and reset confirmation in light and dark
  themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, scrolling, tab traversal, dependent enabled states, RTC
  editing, and reset cancellation on Windows.

## Definition of done

- [ ] The Designer form owns the complete Advanced settings pane layout and permanent controls.
- [ ] Dynamic labels, config writes, state restrictions, RTC handling, and reset behavior match the
  prior pane.
- [ ] macOS and Windows builds, tests, audits, and native review pass.
