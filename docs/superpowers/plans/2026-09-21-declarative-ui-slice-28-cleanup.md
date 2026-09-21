# Declarative UI Slice 28: Cleanup

**Status:** In progress

**Goal:** Remove the widget-subclass compatibility layer used before bind-after-construction,
retain the shared settings-help balloon through the binder module, and make the form/C++ layout
boundary enforceable for future changes.

## Task 1: Migrate the remaining compatibility-control users

- [ ] Replace Free Look's `ConfigBool` and `ConfigChoice` instances with stock `QCheckBox` and
      `QComboBox` controls bound through `ConfigWidget`.
- [ ] Replace Logitech microphone `ConfigBool` instances with stock bound checkboxes.
- [ ] Replace SDL hint `ToolTipCheckBox` instances with stock checkboxes and binder-owned help
      metadata while preserving its string-valued SDL settings.
- [ ] Preserve labels, option order, setting values, enablement, descriptions, signals, and
      emulation-state restrictions.

## Task 2: Remove the legacy control directories

- [ ] Move `BalloonTip` and `ToolTipStyle`, which remain active binder infrastructure, into
      `Config/Binder`.
- [ ] Update application and test includes, comments, and CMake source lists to use the binder
      location.
- [ ] Remove the obsolete `ConfigControls` and tooltip widget-subclass sources.
- [ ] Remove the legacy-only settings-help test while retaining coverage for help metadata on stock
      controls.

## Task 3: Enforce the declarative boundary

- [ ] Add a configure-time check that rejects `new Q*Layout` construction in a controller's C++
      file when a same-name `.ui` form owns its permanent shell.
- [ ] Document explicit exceptions for runtime-generated Riivolution option groups, the Triforce
      IP-redirection dialog, and TAS input groups.
- [ ] Make newly added same-name form/controller pairs participate in the check automatically.

## Task 4: Verify and clean

- [ ] Confirm no application or test source references `ConfigControls` or `ToolTipControls`.
- [ ] Run formatting, diff checks, translation extraction, and targeted catalog parity.
- [ ] Configure and build the application and both test binaries and run both suites on macOS and
      Windows.
- [ ] Review Free Look, Logitech microphone, SDL hints, and settings-help behavior with native Qt
      rendering where available.
- [ ] Remove temporary scripts, captures, profiles, catalogs, patches, and transferred changes.

## Definition of done

- [ ] The legacy control directories are gone and the remaining users rely on stock Qt widgets.
- [ ] Balloon help remains functional through `ConfigWidget::SetDescription`.
- [ ] Form-backed controllers cannot silently reintroduce permanent C++ layouts.
- [ ] macOS and Windows builds, tests, extraction, and representative visual review pass.

## Verification

Pending.
