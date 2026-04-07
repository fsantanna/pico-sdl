# w-bug: collision rect != visible rect (cross-layer struct reuse)

## Symptom

In `tst/mouse-rect-click.c` (and the minimal repro `tst/w-bug.c`), the
"click 2" assertion `pico_vs_pos_rect(&pos, &btn2)` passes, but the red
pixel drawn at the same mouse position renders just outside the
visually-drawn `btn2`.

## Root cause

`btn1/2/3` are reused as **two different rectangles** depending on
which layer is current at the moment they are interpreted:

1. While layer `"A"` is current (`pico_set_layer("A")`), the literal
   `btn = { '%', {.5, .5, .15, .30}, C, NULL }` is `'%'` of
   **layer A's `120x50`** -> physically drawn at layer-A coords
   `(51..69, 17.5..32.5)`. After `pico_output_draw_layer("A", &r)`
   composites layer A into `r` (35% bottom-right of the screen),
   the visible button covers approximately win `(394..420, ...)`.

2. After `pico_set_layer(NULL)` and during the collision check, the
   *same* literal is `'%'` of the **screen layer's `100x100`** ->
   a phantom rect at win `(212..287, 175..325)`. Different rectangle,
   same `Pico_Rel_Rect` struct.

## Why the collision still passes

`pct = pico_get_mouse('%', &r)` returns roughly `(0.56, 0.40)` for mouse
window pixel `(418, 392)` (i.e. mouse position as a fraction of `r`).

The phantom screen-`%` interpretation of `btn2` covers
`x in [0.425, 0.575]`, which includes `0.56` by **coincidence**.
The assertion is satisfied against the *wrong* rect.

## Why the pixel is "outside"

`pico_output_draw_pixel(&{'w', {418, 392}, ...})` resolves to a
window pixel near `(420, 390)` (5x quantization through the screen
layer's `100x100` log grid). That pixel lands on the boundary of the
*real* (layer-A composited) `btn2`, just past its right edge -
visually outside the white rect.

## One-line conclusion

`btn2` is interpreted in two coordinate systems. The collision passes
against one (screen-`%` phantom); the pixel is rendered against the
other (layer-A composited). They are independent rectangles that
happen to share a struct literal.

## Minimal repro

`tst/w-bug.c` reproduces the same pattern with a single button and a
single click. Run:

```bash
make int T=w-bug
```

Expected: assertion passes, but the red pixel in
`tst/out/w-bug.png` lies outside the visible white rect inside the
blue layer-A composited region.

## Notes on dead-end investigations

Earlier hypotheses that turned out **not** to be the cause:

- Float drift in `_pos_win_to_wld` / `_pos_wld_to_win`: real but
  sub-ulp, invisible after `floorf(x+0.5f)`.
- Anchor `0.5` non-invertibility on `Pico_Rel_Pos`: real but sub-pixel.
- `'w'` mode quantization to the screen layer's `100x100` grid: real
  (5 win px per log px) but internally consistent - in a single-layer
  setup, `pico_vs_pos_rect` and `pico_output_draw_pixel` go through
  the same `_abs_rect`/`_abs_pos` pipeline and **cannot** disagree.

The visible bug only appears when the test reasons in multiple
coordinate systems - here, layer A's `120x50`, the screen layer's
`100x100`, and `r`'s continuous fraction of the screen.

## Fix options (not implemented)

1. Re-set the layer to `"A"` before the collision checks so `'%'`
   resolves against layer A.
2. Give `btn1/2/3` `up = &r` so they live in `r`-space regardless of
   the current layer (but then drawing them into layer A becomes
   awkward, since `r` does not exist when layer A is current).
3. Compute collision in `'w'` against layer-A-composited window
   pixels - most explicit, least convenient.
