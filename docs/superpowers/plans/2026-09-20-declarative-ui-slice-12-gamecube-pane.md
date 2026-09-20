# Declarative UI Slice 12: GameCube Settings

**Status:** In progress

**Goal:** Move the GameCube settings pane into a Qt Designer form while preserving IPL settings,
EXI device selection and configuration, memory-card and cartridge path validation, optional mGBA
paths, NetPlay restrictions, and live device changes.

## Task 1: Define the complete form

- [ ] Add `GameCubePane.ui` with IPL Settings, Device Settings, and GBA Settings groups.
- [ ] Define the Slot A, Slot B, and SP1 rows plus form-owned memory-card, GCI-folder, AGP, and
  override-warning rows for both memory-card slots.
- [ ] Define the BIOS, five ROM, save-location, and saves rows for builds with mGBA support.
- [ ] Add buddies, word wrapping, non-default browse/config buttons, and an explicit tab order.
- [ ] Keep the complete pane usable at the settings pane's narrow width without horizontal
  clipping.

## Task 2: Bind settings and populate dynamic choices

- [ ] Bind Skip Main Menu, System Language, the mGBA BIOS and saves paths, all five ROM paths, and
  the same-directory checkbox to their existing typed settings.
- [ ] Populate Slot A and Slot B with the existing EXI device choices and SP1 with its existing
  platform-dependent choices and values.
- [ ] Keep device selection writes grouped under the existing config-change guard and preserve
  live `ChangeDevice` calls while emulation is running.
- [ ] Keep the IPL availability description and disabled state tied to installed IPL ROMs.

## Task 3: Preserve conditional device behavior

- [ ] Keep each slot's config button enabled only for devices with a configuration action.
- [ ] Show memory-card, GCI-folder, and AGP path rows only for the selected device and only when
  custom paths are active.
- [ ] Keep GCI override warnings slot-specific, hidden by default, and visible only when the
  selected folder device has an override.
- [ ] Preserve microphone mapping, broadband adapter dialogs, and Triforce window dispatch.

## Task 4: Preserve path and mGBA behavior

- [ ] Preserve memory-card filenames, region validation, card validity checks, duplicate-slot
  rejection, and live reinsertion.
- [ ] Preserve GCI-folder default reset, region validation, duplicate-slot rejection, and live
  reinsertion.
- [ ] Preserve AGP browsing and live cartridge changes.
- [ ] Preserve mGBA browse filters, effective user paths, same-directory dependency, and NetPlay
  enablement.

## Task 5: Preserve compatibility

- [ ] Keep the existing 43 translated msgids and 11 explicit config symbols.
- [ ] Preserve all 21 concrete config locations represented by bindings, paths, and dynamic slot
  lookups.
- [ ] Add a Qt form test for group order, row placement, language choices, buddies, initial
  visibility, tab order, button defaults, and narrow layout fit.
- [ ] Verify `GameCubePane.cpp` constructs no permanent widget or layout.

## Task 6: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite and targeted gettext/config comparisons.
- [ ] Review GameCube settings in light and dark themes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review wide and narrow layouts, scrolling, dynamic device rows, tab traversal, and mGBA
  dependency states on Windows.

## Definition of done

- [ ] The Designer form owns the complete GameCube pane layout and permanent controls.
- [ ] Existing configuration, validation, dialogs, runtime side effects, and settings signals
  remain intact.
- [ ] macOS and Windows builds, tests, audits, and visual review pass.
