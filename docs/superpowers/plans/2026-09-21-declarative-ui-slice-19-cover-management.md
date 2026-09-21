# Declarative UI Slice 19: Cover Management

**Status:** In progress

**Goal:** Let users assign, replace, and remove game-library cover art while preserving Dolphin's
existing cover cache, custom-cover convention, and automatic GameTDB downloads.

The approved design called cover management slice 18. The branch used slice 17 for requested
settings layout polish and slice 18 for the setup wizard, so this plan continues with slice 19.

## Task 1: Define the managed-cover contract

- [ ] Keep Dolphin's existing adjacent `<game name>.cover.png` custom-cover convention.
- [ ] Normalize supported source images to PNG and write the result atomically.
- [ ] Treat the shared `cover.png` fallback as externally managed so removing one game's cover
  cannot affect other games in the same directory.
- [ ] Keep custom covers usable when automatic GameTDB cover downloads are disabled.

## Task 2: Refresh cached cover state

- [ ] Detect custom-cover additions, replacements, and removals during metadata refresh.
- [ ] Preserve custom-over-default cover priority and the existing banner fallback.
- [ ] Repaint the grid after cover metadata changes without restarting Dolphin.

## Task 3: Add game-library actions

- [ ] Add **Set Cover Image...** for a single selected game.
- [ ] Confirm replacement when a managed custom cover already exists.
- [ ] Add **Remove Custom Cover** only when Dolphin owns a removable per-game cover.
- [ ] Report invalid images, write failures, and removal failures with native modal errors.
- [ ] Trigger a metadata refresh after successful changes.

## Task 4: Cover the behavior

- [ ] Add Qt tests for path derivation, PNG normalization, replacement, invalid input, and removal.
- [ ] Keep the context actions out of multi-selection menus.
- [ ] Run translation extraction and review the added strings.

## Task 5: Verify and clean up

- [ ] Build DolphinQt and both test binaries on macOS and run both test suites.
- [ ] Review set, replace, remove, automatic-download-disabled, light, and dark behavior on macOS.
- [ ] Build and run both test binaries on Windows.
- [ ] Review the context actions and cover refresh behavior on Windows.
- [ ] Remove temporary games, images, scripts, captures, and transferred source changes from both
  hosts.

## Definition of done

- [ ] A user can set, replace, and remove per-game cover art from the game library.
- [ ] Grid cover changes appear without restarting Dolphin.
- [ ] Shared fallback covers are never deleted by the per-game remove action.
- [ ] Custom covers remain visible independently of automatic GameTDB downloads.
- [ ] macOS and Windows builds, tests, extraction, and native review pass.
