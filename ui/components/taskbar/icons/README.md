# Taskbar Icons

Monochrome (white-silhouette) taskbar icons, shipped as 96×96 PNG masks and
tinted at runtime to the active theme's `on-surface` token via
`QPainter::CompositionMode_SourceIn` (same mask-then-tint pipeline the status
bar uses). One color source per icon keeps the set themeable and crisp when
downscaled to the ~28px draw size on 1x and 2x screens.

| File | Meaning | Used by |
|------|---------|---------|
| `files.png`    | Folder      | Files app tile |
| `terminal.png` | `>_` prompt | Terminal app tile |
| `settings.png` | Gear        | Settings app tile |
| `browser.png`  | Globe       | Browser app tile |
| `start.png`    | 2×2 grid    | Start button |

The paired `*.svg` files are the editable source of each PNG; the build only
embeds the PNGs (see `../taskbar_icons.qrc`).

## Origin / License

CFDesktop-original artwork, authored for this project (no third-party license
or attribution required). Re-export a PNG with, e.g.:

```sh
rsvg-convert -w 96 -h 96 files.svg -o files.png
```
