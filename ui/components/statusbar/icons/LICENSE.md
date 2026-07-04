# Status Bar Icons — Attribution

The status bar icons (`signal.png`, `battery.png`, `wifi.png`, `volume.png`) are
sourced from **Icons8** (https://icons8.com).

- Source set: Icons8 line-style glyphs (50px, monochrome black on transparent).
- License: Icons8 Free License — https://icons8.com/license
- Requirement: Free usage requires attribution / a link back to icons8.com.

These PNGs are used as monochrome alpha masks: at runtime `StatusBar` recolors
them to the active Material theme's `onSurfaceVariant` token, so they follow
Light/Dark theme switches with a single asset per icon.

> NOTE: If this project is distributed without an Icons8 attribution elsewhere
> (e.g. README, About dialog), keep this file in place to satisfy the license.
