# TODO

## rot-flip test failing on CI

The `rot-flip.c` test passes locally but fails on GitHub CI due to pixel-perfect
comparison with different graphics drivers.

**Problem:**
- CI uses `ubuntu-latest` with software Mesa rendering
- Local machine uses GPU driver
- Rotation rendering produces slightly different pixels

**Options:**

1. **Use CI-generated images** — Download `test-screenshots` artifact from failed
   workflow and copy to `tst/asr/`

2. **Add tolerance** — Modify `_pico_cmp_files` in `check.h` to compare
   pixel-by-pixel with a threshold instead of byte-exact `memcmp`

3. **Skip rotation tests on CI** — Not recommended

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
