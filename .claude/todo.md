# TODO

## Accept colors as strings in buffer and other calls

Allow color strings (e.g., `'red'`, `'blue'`) in buffer operations and other
functions that accept colors, not just `{r, g, b}` tables.

## Accept empty text string

`pico_output_draw_text("")` currently asserts. Fix to ignore empty strings
instead of failing.

## Lua: support `'transparent'` as color string

The color registry stores `Pico_Color*` via lightuserdata. `PICO_COLOR_TRANSPARENT`
is `Pico_Color_A` (4 bytes), so `c_color_s` casting to `Pico_Color*` reads only
`{0,0,0}` (black). Need to decide how `'transparent'` should behave:

Only makes sense after color strings are supported in buffer calls (see "Accept
colors as strings in buffer and other calls" above). Without that, `'transparent'`
has no RGBA consumer — `set.color.draw` and `set.color.clear` use `Pico_Color`
(RGB only).

## Fonts should be resources

Fonts are always opened and closed (or remain open forever?). They should become
resources with keys like `"/font/tiny/10"` to be GC'ed via the hash table TTL.

## Guide: `pico.get.ticks` never introduced

In `lua/doc/guide.md` section 9.2, `pico.get.ticks` is used but never formally
introduced. Add documentation or an earlier mention.

## Independent grid per layer

Currently grid is only rendered for the main layer. The `view.grid` flag exists
in `Pico_View` but is ignored for non-main layers. Render grid overlay for each
layer with `view.grid` enabled.

See: `.claude/plans/view.md` section 2.

## Fix `tst/todo_*.c` files

Several test files use deprecated APIs and need updating:

| File | Main Issues |
|------|-------------|
| `todo_rain.c` | Deprecated types, `pico_output_draw_rect_pct()` |
| `todo_rotate.c` | `pico_set_view_raw()`, old types |
| `todo_hide.c` | Missing window/view setup |
| `todo_scale.c` | `pico_set_scale()`, `pico_set_anchor_pos()` |
| `todo_control.c` | Wrong text drawing API |
| `todo_main.c` | Deprecated types throughout |

See: `.claude/plans/view.md` section 7.

## Create `extra/` directory

Create an `extra/` directory to hold auxiliary tools and utilities:
- `check.h` — visual regression testing header (currently in `tst/`)
- Video generator (yet to come)

## Support percentage mode for alpha

`pico.set.alpha('%', 0.5)` — allow setting alpha as a normalized value (0.0–1.0)
instead of only raw 0–255.

## Rename alpha to transparency (invert values)

`pico.set.alpha` uses SDL convention where 255=opaque, 0=transparent.
Rename to `pico.set.transparency` (or similar) and invert so that
0=opaque, 255=fully transparent — more intuitive for users.

## Add ttl-GC to history and guide

Document the TTL-based garbage collection mechanism (hash table with TTL
eviction for resources) in the project history and in `lua/doc/guide.md`.

## `pico.set.draw.*` — group all drawing state

Move all drawing-related setters under `pico.set.draw.*` (color, style,
alpha, font, etc.). Consider: should `pico.push`/`pico.pop` become
scoped to `pico.set.draw`? E.g., `pico.draw { ... }` as a block that
auto-pushes/pops.

## `pico_set_view` present in non-expert mode

Already calls `_pico_output_present(0)` (line 976). Pros: navigation
works without input loop, guide nav would work. Cons: flicker on setup
(blank texture after dim change), multiple presents per logical setup.
Investigate if navigation isn't updating visually — the present is
there, issue might be elsewhere.

In guide: why pixels in 5.3 need clear? Why 6.1 doesn't work?

## Review and complete guide

Review `lua/doc/guide.md` for completeness, accuracy, and missing sections.
