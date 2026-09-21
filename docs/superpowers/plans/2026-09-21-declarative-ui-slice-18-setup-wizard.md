# Declarative UI Slice 18: Setup Wizard

**Status:** In progress

**Goal:** Give genuinely new Dolphin profiles a native first-run setup flow, authored in a `.ui`
form and adapted from the PCSX2 setup sequence without copying PCSX2 source or assets.

The approved design called this slice 17. The branch used slice 17 for the intervening settings
layout polish requested during implementation, so this plan continues with slice 18.

## Task 1: Define the first-run contract

- [ ] Detect whether `Dolphin.ini` existed before configuration initialization.
- [ ] Store an incomplete marker before showing the wizard so canceling or an interrupted launch
  presents it again.
- [ ] Never present the wizard to an established profile solely because the marker is absent.
- [ ] Skip modal setup in batch mode while retaining the incomplete marker for the next GUI launch.
- [ ] Cover the decision table with a pure policy test.

## Task 2: Author the setup form

- [ ] Add a native dialog form with Appearance, Game Folders, Controllers, Privacy, and Complete
  pages.
- [ ] Keep page navigation, margins, focus order, accessible names, and minimum sizing usable at a
  compact desktop window size.
- [ ] Use Dolphin's own logo, strings, settings, and implementation.
- [ ] Add a form test for page order, controls, layout ownership, and keyboard traversal.

## Task 3: Connect Dolphin settings

- [ ] Share the existing Interface language choices rather than duplicating them.
- [ ] Save language and built-in style choices, applying style changes to the wizard immediately.
- [ ] Add and remove game folders and bind recursive scanning.
- [ ] Record the usage-statistics choice and suppress the separate analytics prompt after setup.
- [ ] Offer to open Controller Settings after the main window initializes.
- [ ] Save configuration and clear the incomplete marker only when setup finishes.

## Task 4: Integrate startup

- [ ] Run setup before constructing the main window so it cannot flash behind the wizard.
- [ ] Preserve command-line boot, updater, analytics, shutdown, and established-profile behavior.
- [ ] Exit cleanly when setup is canceled.

## Task 5: Verify and clean up

- [ ] Build DolphinQt and both test binaries on macOS and run both test suites.
- [ ] Run the UI extraction regression test.
- [ ] Review every wizard page in native light and dark styles on macOS.
- [ ] Build and test on Windows and review the wizard in native light and dark styles.
- [ ] Exercise new-profile completion, cancel-and-resume, established-profile, and batch-mode paths.
- [ ] Remove temporary users, scripts, captures, and build artifacts from both hosts.

## Definition of done

- [ ] A new GUI profile sees the setup wizard before the main window.
- [ ] Existing profiles and batch launches are not interrupted.
- [ ] Canceling causes setup to return on the next GUI launch.
- [ ] Appearance, paths, analytics, and controller follow-up choices take effect.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.
