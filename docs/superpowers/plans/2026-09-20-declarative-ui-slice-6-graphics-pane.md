# Declarative UI Slice 6: Graphics Settings

**Status:** In progress

**Goal:** Move the Graphics settings tab shell and its four permanent pages into Qt Designer
forms, replace their config-widget subclasses with stock widgets and bind-after-construction
controls, and preserve backend-dependent availability, per-game overrides, descriptions, and
translated strings.

The shader preset picker and shader-parameter editor remain dynamic standalone dialogs. Their
container migrations belong with the later standalone-dialog slices; the controls generated from
shader metadata must remain in C++.

## Task 1: Migrate the tab shell

- [x] Add `GraphicsPane.ui` for the General, Enhancements, Hacks, and Advanced tab structure.
- [x] Keep construction of the four page controllers and their scroll wrappers in C++.
- [x] Preserve backend propagation and the shared per-game config layer.
- [x] Add a Qt form test for the permanent tab structure.

## Task 2: Migrate General graphics settings

- [x] Add `GeneralWidget.ui` for Basic, Other, and Shader Compilation.
- [x] Bind backend, aspect ratio, custom dimensions, checkboxes, and shader modes through
  `ConfigWidget`.
- [x] Preserve adapter population, backend warnings, custom-aspect visibility, runtime
  restrictions, and backend-change signals.
- [x] Move every permanent description to `ConfigWidget::SetDescription`.

Checkpoint: the tab shell and General page build in `dolphin-emu` and `qt-tests`; all 105 Qt tests
pass on macOS, including the two new form-structure tests.

## Task 3: Migrate Enhancements

- [ ] Add `EnhancementsWidget.ui` for enhancement, post-processing, and stereoscopy controls.
- [ ] Replace simple, mapped, complex, text, and float-slider config subclasses with binder
  equivalents.
- [ ] Preserve backend capability gating, shader preset browsing and parameters, anti-aliasing
  population, stereoscopy behavior, value labels, and inter-control dependencies.

## Task 4: Migrate Hacks

- [ ] Add `HacksWidget.ui` for EFB, texture-cache, XFB, and other controls.
- [ ] Replace every config subclass with stock widgets and binder calls, including reversed
  boolean bindings and the mapped accuracy slider.
- [ ] Preserve backend capability descriptions and all dependent enabled states.

## Task 5: Migrate Advanced

- [ ] Add `AdvancedWidget.ui` for debugging, utility, texture dumping, frame dumping, crop, misc,
  and experimental controls.
- [ ] Replace every config subclass with stock widgets and binder calls.
- [ ] Preserve global-only controls, runtime restrictions, backend-specific availability,
  dependent enabled states, and label font mirroring.

## Task 6: Preserve compatibility

- [ ] Record an exact before/after binding-key comparison for all four pages.
- [ ] Keep every existing config key, reversed value, range, step, and default.
- [ ] Keep global-versus-per-game layer choices unchanged.
- [ ] Keep tab order, labels, group titles, descriptions, and all emitted signals.
- [ ] Record the targeted gettext msgid comparison.

## Task 7: Verify the slice

- [ ] Build `dolphin-emu`, `qt-tests`, and `tests` on macOS.
- [ ] Run the UI extraction regression suite.
- [ ] Review every tab in light and dark themes at wide and narrow sizes on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review every tab, persistent help, focus traversal, disabled controls, and narrow sizing on
  Windows.

## Definition of done

- [ ] Five Designer forms own the Graphics pane's permanent tab and page layouts.
- [ ] No Graphics page constructs a permanent control or layout in C++.
- [ ] Every former Graphics-page config subclass uses the binder.
- [ ] Existing backend, emulation-state, per-game override, shader, and dependency behavior remains
  intact.
- [ ] macOS and Windows builds, tests, gettext checks, binding audit, and visual review pass.
