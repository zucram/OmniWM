# OmniWM - Claude Context

## Overview
macOS tiling window manager written in Swift 6 using SPM. Targets macOS 15+.

## Build & Test
```bash
swift build          # Build the project
swift test           # Run tests (if available)
```

## GhosttyKit Dependency
The project depends on `GhosttyKit.xcframework` from the Ghostty terminal emulator (powers the quake-style drop-down terminal). This binary framework is gitignored and may not be present on disk. For development without the real framework, a C stub target exists at `Sources/GhosttyKit/` that can be swapped in via `Package.swift` (replace the `.binaryTarget` with a local `.target`).

## Testing Local Builds
The raw debug binary (`.build/debug/OmniWM`) has no bundle ID, so `UserDefaults.standard` won't find settings. To test properly:
1. Copy binary into the app bundle: `sudo cp .build/debug/OmniWM /Applications/OmniWM.app/Contents/MacOS/OmniWM`
2. Re-sign to preserve TCC grants: `codesign --force --sign "Apple Development: marcus@harliddavin.com (FDNBY7UMDG)" /Applications/OmniWM.app`
3. Restart OmniWM

## Architecture
- **Bundle ID**: `com.barut.OmniWM`
- **Entitlements**: `com.apple.security.automation.apple-events`
- **TCC**: Requires Accessibility and Screen Recording permissions. These are tied to code signing identity — ad-hoc signatures lose grants on every build. Use the Apple Development cert above for stable identity.
- **Upstream repo**: `BarutSRB/OmniWM` on GitHub
- **Fork**: `zucram/OmniWM` — PRs go from fork branches to upstream

## Key Source Locations
- `Sources/OmniWM/Core/Overview/` — Overview (Exposé-like) window grid
  - `OverviewController.swift` — scroll handling, thumbnail capture, layout coordination
  - `OverviewWindow.swift` — NSPanel/NSView, input events, scroll delta normalization
  - `OverviewRenderer.swift` — Core Graphics rendering, applies `scrollOffset` via `translateBy`
  - `OverviewLayoutCalculator.swift` — layout math, scroll offset bounds/clamping
- `Sources/OmniWM/QuakeTerminal/` — Drop-down terminal (uses GhosttyKit)
- `Sources/OmniWM/Core/Config/` — Settings/configuration
- `Sources/OmniWM/App/` — App entry point, UserDefaults

## Scroll System
- `normalizedScrollDelta(for:)` in `OverviewView` handles `isDirectionInvertedFromDevice` to normalize across natural/traditional scrolling
- `adjustScrollOffset(by:on:)` applies delta: `scrollOffset + delta`
- Scroll offset range is `minOffset...0` (negative = scrolled down)
- Renderer: `context.translateBy(x: 0, y: -scrollOffset)` — negative offset shifts content up, revealing content below

## In-Progress Feature Branches (as of 2026-03-20)

### Branch: `feat/per-monitor-default-column-width`
Pushed to fork (`zucram/OmniWM`). PR not yet submitted upstream.

Adds per-monitor override for default column width (e.g., 100% on MacBook, 50% on external monitor).

**Files changed:**
- `MonitorNiriSettings.swift` — added `defaultColumnWidth: Double?` to `MonitorNiriSettings` and `ResolvedNiriSettings`
- `SettingsStore.swift` — `resolvedNiriSettings()` resolves `defaultColumnWidth` with global fallback
- `NiriLayoutEngine+Monitors.swift` — added `effectiveDefaultColumnWidth(in:)` helper
- `NiriLayoutEngine.swift` — `initializeNewColumnWidth` uses per-monitor effective width
- `SettingsView.swift` — `MonitorNiriSettingsSection` has slider for "Default Column Width" (5–100%)

### Branch: `feat/workspace-in-status-bar`
Pushed to fork (`zucram/OmniWM`). PR not yet submitted upstream.

AeroSpace-style workspace indicator in the native macOS menu bar status item.

**Files changed:**
- `StatusBarController.swift` — added `refreshWorkspaces()`: shows focused workspace name next to `o.circle` icon via `button.title`/`button.imagePosition = .imageLeft`
- `StatusBarMenu.swift` — `StatusBarMenuBuilder.updateWorkspaces()`: workspace section at top of menu using `MenuActionRowView` for consistent dark styling; `checkmark` for focused, `circle` for others; app names gated by settings, truncated at 38 chars
- `WMController.swift` — added `weak var statusBarController`, `anyBarRefreshIsEnabled` computed property, calls `statusBarController?.refreshWorkspaces()` from `flushRequestedWorkspaceBarRefresh`
- `AppDelegate.swift` — wires `controller.statusBarController = statusBarController` after setup
- `SettingsStore.swift` — added `statusBarShowWorkspaceName` (default: true) and `statusBarShowAppNames` (default: true)
- `SettingsView.swift` — "Status Bar" section in GeneralSettingsTab with toggles

### Branch: `build/both-features`
Local-only combined branch merging both feature branches. Used for local testing/deployment.

## Current State & Known Issues

### GhosttyKit stub header version
The stub at `Sources/GhosttyKit/include/GhosttyKit.h` must match the code it's compiled against:
- **Pre-upstream code** (commit `1f0cac2` and earlier): `read_clipboard_cb` returns `void`
- **Post-upstream code** (after merge `f1f3657`): `read_clipboard_cb` returns `bool`
Always check and update the stub header after switching between pre/post upstream commits.

### Floating workspace bar repositioning bug
**Status:** Unresolved. The floating bar (`WorkspaceBarManager`) shifts horizontally when switching workspaces.
- Only happens switching TO workspace 2+, never when switching back to workspace 1
- Root cause: `refreshBarContent()` calls `updateBarFrameAndPosition()` which remeasures content width via `measuredWidth(for:using:)` and recenters with `x = monitor.frame.midX - width / 2`. Different workspaces have different content (icons, labels), producing different widths.
- The bug exists both pre- and post-upstream merge, but became more noticeable after upstream changes (floating windows, scratchpad features in commits `f67f58f`–`75cc44f`)
- **Attempted fixes that didn't work:**
  1. Removing `updateBarFrameAndPosition` from `refreshBarContent` — bar still moved (suspected `NSHostingView` auto-resizing the panel)
  2. Frame locking in `WorkspaceBarPanel` (override `setFrame` to block unauthorized changes) — bar disappeared entirely (too aggressive, blocked legitimate frame sets during panel ordering)
- **Likely correct fix direction:** Need both: (a) remove `updateBarFrameAndPosition` from `refreshBarContent`, AND (b) prevent `NSHostingView` from auto-resizing the panel, possibly by pinning the SwiftUI content to a fixed width or by a more targeted panel frame lock that allows initial setup but blocks subsequent auto-resize

### Currently deployed build
Detached HEAD at `1f0cac2` (pre-upstream `build/both-features`). This is the last known-good build before the upstream merge introduced the bar movement regression. The stub header has `read_clipboard_cb` returning `void` to match.

### Upstream sync status
Main branch has merged upstream up to commit `f1f3657` (includes floating windows, scratchpad, viewport fixes — 9 commits). Feature branches were rebased onto this. To return to post-upstream state: `git checkout build/both-features` and swap Package.swift + fix stub header.

### Package.swift
After every upstream merge or branch switch, check that `GhosttyKit` target is the local stub:
```swift
.target(name: "GhosttyKit", path: "Sources/GhosttyKit", publicHeadersPath: "include"),
```
NOT the binary target. Upstream always has `.binaryTarget`.
