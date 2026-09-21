# Declarative UI Slice 25: Controller Mapping

**Status:** In progress

**Goal:** Move the controller-mapping, control-expression, adapter, and alternate-input shells to
Qt Designer forms without changing input detection, binding, profile, device, or controller-group
behavior.

This slice covers the mapping window, its controller and hotkey pages, the advanced input/output
expression dialog, GameCube adapter settings, and DSU alternate-input configuration.

## Task 1: Preserve the runtime boundary

- [ ] Keep controller-group discovery, binding buttons, numeric settings, indicators, calibration,
      and advanced mapping dialogs generated in C++.
- [ ] Keep device enumeration, profile discovery and persistence, input detection, output testing,
      expression parsing, and controller updates in C++.
- [ ] Keep Wii extension selection, extension-dependent visibility, and runtime attachment controls
      in C++.
- [ ] Keep conditional controller-interface backends and DSU server data operations in C++.
- [ ] Move permanent widgets, properties, focus order, and placement layouts to `.ui` files.

## Task 2: Migrate the mapping shells

- [ ] Add forms for `MappingWindow`, `IOWindow`, and `GCPadWiiUConfigDialog`.
- [ ] Add forms for the GameCube, Game Boy Advance, Wii Remote, extension, Free Look, and hotkey
      mapping pages.
- [ ] Retain named empty layouts in the forms as insertion targets for runtime-generated
      controller groups.
- [ ] Remove C++ construction of each permanent mapping-page and dialog layout.

## Task 3: Migrate alternate input sources

- [ ] Add forms for `ControllerInterfaceWindow`, `DualShockUDPClientWidget`, and
      `DualShockUDPClientEditServerDialog`.
- [ ] Preserve compile-time backend availability, server validation, storage, editing, and refresh
      behavior.
- [ ] Remove C++ construction of each permanent alternate-input layout.

## Task 4: Cover and verify

- [ ] Add Qt form tests for shell hierarchy, insertion layouts, defaults, visibility, sizing, text
      interaction, and button roles.
- [ ] Run XML, `uic`, formatting, translation extraction, and targeted string-parity checks.
- [ ] Build the application and both test binaries and run both suites on macOS and Windows.
- [ ] Review representative controller, hotkey, expression, adapter, and alternate-input forms with
      native Qt rendering on both platforms.
- [ ] Remove temporary profiles, scripts, captures, harnesses, and transferred changes.

## Definition of done

- [ ] All permanent controller-mapping and alternate-input shells are form-owned.
- [ ] Runtime-generated controls, data, decisions, side effects, strings, and translator comments
      remain intact.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.

## Verification

Pending.
