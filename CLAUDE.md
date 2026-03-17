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
