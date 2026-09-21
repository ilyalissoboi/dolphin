# Declarative UI Slice 26: TAS Input

**Status:** In progress

**Goal:** Move the shared and controller-specific TAS input shells to Qt Designer forms without
changing input overrides, turbo behavior, shortcuts, live controller values, attachment handling,
or stick and IR interaction.

This slice covers the shared TAS scroll and settings shell plus the GameCube, Game Boy Advance, and
Wii TAS input layouts.

## Task 1: Preserve the runtime boundary

- [ ] Keep `TASCheckBox`, `TASSlider`, `TASSpinBox`, `StickWidget`, and `IRWidget` construction and
      signal wiring in C++.
- [ ] Keep input-override registration, value conversion, keyboard shortcuts, turbo state, and
      live controller updates in C++.
- [ ] Keep Wii extension and MotionPlus discovery, callbacks, visibility, resizing, and override
      selection in C++.
- [ ] Move the scroll shell, settings controls, fixed group boxes, and placement layouts to `.ui`
      files.

## Task 2: Migrate the shared shell

- [ ] Add a `TASInputWindow` form that owns the scroll area, content insertion layout, settings
      group, controller-input checkbox, turbo labels, and frame spin boxes.
- [ ] Preserve scroll-bar policies, zero outer margins, control ranges, tooltips, buddies, and focus
      order.
- [ ] Replace `SetupScrollArea` and C++ construction of the shared settings shell with form setup.

## Task 3: Migrate controller-specific layouts

- [ ] Add forms for the GameCube, Game Boy Advance, and Wii TAS input content.
- [ ] Retain named empty layouts as insertion targets for runtime-created stick, IR, slider, and
      button controls.
- [ ] Preserve each controller's group titles, button order, spacers, aspect-ratio wrappers, and
      extension-dependent containers.
- [ ] Remove C++ construction of permanent controller-specific group boxes and layouts.

## Task 4: Cover and verify

- [ ] Add Qt form tests for shell hierarchy, insertion targets, settings defaults, labels, buddies,
      focus order, stretches, and scroll behavior.
- [ ] Run XML, formatting, translation extraction, and targeted string-parity checks.
- [ ] Build the application and both test binaries and run both suites on macOS and Windows.
- [ ] Review representative GameCube, Game Boy Advance, Wii Remote, Nunchuk, and Classic Controller
      TAS layouts with native Qt rendering on both platforms.
- [ ] Remove temporary scripts, captures, harnesses, and transferred changes.

## Definition of done

- [ ] All permanent TAS input shells and fixed controller containers are form-owned.
- [ ] Runtime controls, shortcuts, attachment decisions, input overrides, strings, and translator
      comments remain intact.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.

## Verification

Pending.
