# Guide Plan

Roadmap for `lua/doc/guide.md`.

The guide is not exhaustive: it walks through key concepts with
interactive examples. Consult `lua/doc/api.md` for the full API.

## Current Sections

| # | Section | Status | Notes |
|---|---------|--------|-------|
| 1 | Introduction | OK | |
| 2 | Initialization | OK | mention `size` shorthand |
| 3 | Basic Drawing | OK | not exhaustive by design |
| 4 | Internal State | OK | not exhaustive by design |
| 5 | Positioning | Good | raw, pct, anchors, tiles |
| 6 | Advanced View | OK | added size, target, clip, fullscreen |
| 7 | Events | OK | added mouse fields, timeout nil, quit |
| 8 | Layers | OK | added screenshot for 8.2 |
| 9 | Expert Mode | OK | |
| 10 | Auxiliary Functions | OK | cv.pos, cv.rect, vs.pos_rect, vs.rect_rect |
| 11 | Extras | OK | output.sound, output.screenshot |

## Issues (future)

- **9.2**: `pico.get.ticks` used but never formally introduced
- **New section**: Guide for video playback
