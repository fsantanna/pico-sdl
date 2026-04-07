# w-bug: collision rect != visible rect (cross-layer struct reuse)

## The bug

A `Pico_Rel_Rect` literal with `up = NULL` does not represent a fixed
rectangle - it represents "evaluate me later against whatever
`S.layer` happens to be". The same struct can therefore mean two
completely different rectangles depending on when it is used.

`tst/mouse-rect-click.c` fell into this. `btn1/2/3` are declared with
`up = NULL` and used in two roles:

1. While layer `"A"` is current, `pico_output_draw_rect(&btn)` resolves
   `'%'` against layer A's `120x50`. The button is physically drawn at
   layer-A coords `(51..69, 17.5..32.5)` and later composited via `r`
   into a region of the screen.

2. After `pico_set_layer(NULL)`, `pico_vs_pos_rect(&pos, &btn)`
   resolves the **same** struct against the screen layer's `100x100`.
   That gives a phantom rectangle in screen-`%` space, which has
   nothing to do with the visible button.

The collision passes against the phantom rectangle by coincidence
(the phantom happens to overlap with the mouse position). Meanwhile
the red pixel is drawn at the mouse window position, which lands at
the boundary of the *real* (layer-A composited) button. The two
rectangles disagree, the assertion is satisfied against the wrong
one, and the user sees a passing test with a pixel rendered just
outside the white button.

A second, related bug in the same test:
`pct = pico_get_mouse('%', &r)` is a fraction *of `r`*, but `pos`
was constructed with `up = NULL`, so the same fraction is reinterpreted
as a fraction of the screen. `pos` then refers to a different point
than the actual mouse.

## Minimal repro

`tst/mouse-w-click.c` reproduces both bugs with a single button and
two clicks:

- Click `267` (one window pixel inside the visible button): collision
  passes, both pixels render inside.
- Click `268` (one window pixel past the visible button): collision
  fails, both pixels render outside.

Both clicks use `pos.up = &r` and (after `pico_output_draw_layer`)
`btn.up = &r`. With these the collision agrees with what is drawn,
including at the 1-pixel boundary.

## Fix pattern (applied to both tests)

The API is correct. Tests must opt out of late-binding by giving
`'%'` rects/positions an explicit `up` whenever they cross a
`set_layer` boundary.

For `mouse-rect-click.c` and `lua/tst/mouse-rect-click.lua`:

1. After `pico_output_draw_layer("A", &r)`, set
   `btn1.up = btn2.up = btn3.up = &r`. From here on, the buttons mean
   "`'%'` of `r`" - i.e. the visible region - regardless of which
   layer is current.
2. In each click block, declare `pos` with `up = &r`. Now `pct`
   (fraction of `r`) is interpreted as fraction of `r`, and `pos`
   refers to the actual mouse window pixel.
3. Click 2's coordinate moved from `(418, 392)` to `(419, 392)` so
   the integer rounding (with anchor C) lands one log pixel inside
   `btn2`'s right edge instead of one past it. The original `(418, …)`
   only "worked" against the buggy phantom rectangle.

## Color convention in the fixed test

The hit clicks (1, 2, 3) draw their pixel in green. The no-collision
click stays red. After the fix, every green pixel sits inside the
visible button it claims to hit.

## Dead-end investigations

Earlier hypotheses that turned out **not** to be the cause of the
visible bug:

- Float drift in `_pos_win_to_wld` / `_pos_wld_to_win`: real but
  sub-ulp, invisible after `floorf(x+0.5f)`.
- Anchor `0.5` non-invertibility on `Pico_Rel_Pos`: real but
  sub-pixel.
- `'w'` mode quantization to the screen layer's `100x100` log grid:
  real (5 win px per log px) but internally consistent. In a
  single-layer setup `pico_vs_pos_rect` and `pico_output_draw_pixel`
  go through the same `_abs_rect`/`_abs_pos` pipeline and **cannot**
  disagree.

The visible bug only appears when a test reasons in multiple
coordinate systems, here layer A's `120x50`, the screen layer's
`100x100`, and `r`'s continuous fraction of the screen.

## Files touched

- `tst/mouse-w-click.c` - new minimal repro / regression test.
- `tst/mouse-rect-click.c` - fix applied (`up = &r` on btn1/2/3 and
  pos, click 2 nudge, RED -> GREEN on hit clicks).
- `lua/tst/mouse-rect-click.lua` - same fix mirrored.

## Open follow-ups

- Regenerate `tst/asr/mouse-rect-click-*.png` (and the Lua mirror)
  to match the fixed output.
- Decide whether `pico_set_mouse`'s old `pos->anchor = PICO_ANCHOR_NW;
  // TODO` line (now removed) needs any compatibility note.
- The screen layer's `5x` quantization (`PICO_DIM_LOG` -> `PICO_DIM_PHY`)
  is a real architectural property that limits `'w'` precision to
  5-window-pixel cells. Worth a separate note in user docs but not a
  bug.

- **`'w'` vs `r`-relative divergence (isolated in `tst/mouse-w-click.c`):**
  even after `btn.up = &r`, a `'w'` pixel draw and a collision against
  the same `btn` can disagree near edges. Collision goes
  `win -> pct of r -> r-%` (continuous) while
  `pico_output_draw_pixel({'w', ...})` snaps through the screen log grid
  (5 win px / log px). When `r` is distorted or non-grid-aligned, the
  snapped cell can land outside the visible btn even though the
  continuous `r`-% point is inside.
    - Repro: `tst/mouse-w-click.c`, single btn, `r = SE 0.99/0.99/0.35/0.35`,
      mouse `'w' (400, 382)`. `pico_vs_pos_rect` passes; the green pixel
      renders one row above btn's top edge (win 380 < 381.25).
    - Options:
        1. Route `'w'` draws through the same `_abs_pos` pipeline used by
           collision against `r` (no screen-log snap when an `up` chain
           exists or when called outside any explicit layer).
        2. Document the 5-win-px quantization as a hard limit; require
           edge-precision tests to draw via `up = &r` instead of `'w'`.
