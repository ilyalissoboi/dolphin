# Declarative UI Slice 23: Root Utility Dialogs

**Status:** In progress

**Goal:** Move the small root-level About, Discord join request, memory-card creation, NAND repair,
and NKit warning dialogs to Qt Designer forms without changing their decisions or side effects.

The approved design groups the remaining root-level dialogs into the standalone-dialog phase.
The larger conversion and manager surfaces have substantially more dynamic content, so this slice
finishes the compact utility dialogs first and leaves those managers for the next root-level batch.

## Task 1: Preserve the behavior boundary

- [ ] Keep version formatting, external links, avatar download, card creation, NAND title lookup,
  warning decisions, and configuration writes in C++.
- [ ] Keep optional avatar and NAND-removal content controlled dynamically.
- [ ] Preserve window titles, icons, button roles, defaults, text wrapping, fixed sizing, and
  accept/reject behavior.
- [ ] Move only permanent widgets, properties, focus order, and layout positioning to `.ui` files.

## Task 2: Migrate the dialogs

- [ ] Add forms for About, Discord join request, memory-card creation, NAND repair, and NKit
  warning.
- [ ] Replace static widget members with generated form ownership where needed.
- [ ] Remove all C++ layout construction from the five dialog controllers.

## Task 3: Cover and verify

- [ ] Add Qt form tests for permanent structure, defaults, button boxes, optional-content
  placeholders, and keyboard order.
- [ ] Run XML, `uic`, formatting, translation extraction, and targeted string-parity checks.
- [ ] Build the app and both test binaries and run both suites on macOS and Windows.
- [ ] Review all five dialogs with native Qt rendering on both platforms.
- [ ] Remove temporary profiles, scripts, captures, harnesses, and transferred changes.

## Definition of done

- [ ] The five forms own every permanent widget and layout.
- [ ] The existing decisions, side effects, strings, and translator comments remain intact.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.
