# Declarative UI Slice 24: Remaining Root-Level Surfaces

**Status:** In progress

**Goal:** Move the remaining root-level cheat, conversion, status, memory-card, resource-pack,
Riivolution, and updater surfaces to Qt Designer forms without changing their runtime behavior.

This completes the standalone root-level phase before the controller-mapping, TAS, debugger, and
final cleanup slices.

## Task 1: Preserve the behavior boundary

- [ ] Keep cheat-search sessions, enum item data, results, generated tabs, and game state in C++.
- [ ] Keep conversion choices, compression rules, progress, file operations, and validation in C++.
- [ ] Keep emulation metric formatting and visibility decisions in C++.
- [ ] Keep memory-card slot data, table contents, menus, and file operations in C++.
- [ ] Keep resource-pack discovery, selection, installation, priority, and table contents in C++.
- [ ] Keep generated Riivolution disc, section, and option rows in C++.
- [ ] Keep updater version text, release notes, button roles, settings writes, and update flow in C++.
- [ ] Move permanent widgets, properties, focus order, and shell layout positioning to `.ui` files.

## Task 2: Migrate the surfaces

- [ ] Add forms for the cheat-search factory, cheat-search session, and cheats manager.
- [ ] Add forms for conversion, emulation status, memory-card manager, and resource-pack manager.
- [ ] Add a form for the permanent Riivolution boot shell while retaining generated patch layouts.
- [ ] Add a form for the update-available dialog.
- [ ] Replace static widget construction with generated form ownership.
- [ ] Remove C++ construction of every permanent root-level layout covered by this slice.

## Task 3: Cover and verify

- [ ] Add Qt form tests for permanent structure, defaults, sizing, placeholders, and button order.
- [ ] Run XML, `uic`, formatting, translation extraction, and targeted string-parity checks.
- [ ] Build the app and both test binaries and run both suites on macOS and Windows.
- [ ] Review representative forms with native Qt rendering on both platforms.
- [ ] Remove temporary profiles, scripts, captures, harnesses, and transferred changes.

## Definition of done

- [ ] The nine controllers use forms for all permanent widgets and shell layouts.
- [ ] Runtime-generated controls, data, decisions, side effects, strings, and translator comments
      remain intact.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.

## Verification

Pending.
