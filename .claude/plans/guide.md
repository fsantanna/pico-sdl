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
| 9 | Expert Mode | OK | 9.1: two-sprite sync animation (CW fast + CCW slow) |
| 10 | Auxiliary Functions | OK | cv.pos, cv.rect, vs.pos_rect, vs.rect_rect |
| 11 | Extras | OK | output.sound, output.screenshot |

## Done

- **Flatten events**: `lua/pico.c` — `L_set_mouse_fields` /
  `L_set_keyboard_fields` helpers; event switch uses flat fields
  (`e.key`, `e.x`) instead of nested sub-tables
- **Fix rects.lua**: return-value `pico.get.mouse('!')` API
- **Fix guide §9.2**: inline example matches rects.lua (raw mode)
- **Fix guide §8.4**: `anc='C'` → `anchor='C'` (3 occurrences)
- **Typos**: "exhibit" → "exhibits" (§2.1), "resuluts" → "results" (§6.4)

## Issues (future)

- **New section**: Guide for video playback
