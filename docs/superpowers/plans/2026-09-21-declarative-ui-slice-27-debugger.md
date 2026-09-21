# Declarative UI Slice 27: Debugger

**Status:** In progress

**Goal:** Move the permanent layouts and containers of DolphinQt's debugger tools to Qt Designer
forms without changing debugger state, models, disassembly, memory access, breakpoint behavior,
custom views, toolbar actions, or settings persistence.

This slice covers the 15 debugger controllers that still construct layouts in C++:
`AssembleInstructionDialog`, `AssemblerWidget`, `BranchWatchDialog`, `BreakpointDialog`,
`BreakpointWidget`, `CodeWidget`, `EditSymbolDialog`, `JITWidget`, `MemoryViewWidget`,
`MemoryWidget`, `NetworkWidget`, `PatchInstructionDialog`, `RegisterWidget`, `ThreadWidget`, and
`WatchWidget`.

## Task 1: Preserve the runtime boundary

- [ ] Keep debugger models, table contents, proxy filtering, selection, and context menus in C++.
- [ ] Keep assembly, disassembly, symbol, memory, breakpoint, watch, thread, network, and JIT
      operations in C++.
- [ ] Keep custom-painted code and memory views, runtime toolbars and actions, generated rows, and
      settings persistence in C++.
- [ ] Move permanent dialog and dock shells, group boxes, splitters, tabs, labels, button boxes,
      scroll areas, and placement layouts to `.ui` files.

## Task 2: Migrate compact dialogs and docks

- [ ] Add forms for the assemble, patch-instruction, edit-symbol, and breakpoint dialogs.
- [ ] Add forms for the breakpoint, register, and watch dock widgets.
- [ ] Add a form for `MemoryViewWidget` while retaining its runtime table model/delegate and custom
      memory behavior.
- [ ] Preserve button roles, tab order, table properties, toolbar behavior, and minimum sizing.

## Task 3: Migrate compound debugger surfaces

- [ ] Add forms for the assembler, code, JIT, memory, network, thread, and branch-watch tools.
- [ ] Retain named insertion layouts for runtime editors, custom views, toolbar groups, generated
      controls, tables, and status widgets.
- [ ] Preserve splitter orientation and stretch behavior, group order, scroll behavior, filters,
      disabled states, and visibility decisions.
- [ ] Remove C++ construction of every permanent layout covered by the slice.

## Task 4: Cover and verify

- [ ] Add Qt form tests for hierarchy, orientation, insertion targets, defaults, buddy links,
      button roles, table properties, and focus order.
- [ ] Run XML, formatting, translation extraction, targeted catalog parity, and a debugger
      `new Q*Layout` boundary check.
- [ ] Build the application and both test binaries and run both suites on macOS and Windows.
- [ ] Review representative compact and compound debugger forms with native Qt rendering on both
      platforms.
- [ ] Remove temporary scripts, captures, harnesses, catalogs, and transferred changes.

## Definition of done

- [ ] All 15 debugger controllers use forms for permanent shells and layout containers.
- [ ] Runtime debugger logic, custom views, generated content, strings, shortcuts, and side effects
      remain intact.
- [ ] No debugger C++ file constructs a Qt layout.
- [ ] macOS and Windows builds, tests, extraction, and visual review pass.

## Verification

Pending.
