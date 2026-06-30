# font-scale

`pico_output_draw_text` scales the rendered glyph surface instead of
rendering at the target size.
With auto-width (`rect.w == 0`) this produces a non-uniform,
per-call-rounded stretch, so incrementally-grown text (a typewriter
reveal) flickers: every added character slides the earlier glyphs by
~1px.

## Symptom

Typewriter reveal in a downstream app (Pingus port): drawing
`"For"`, `"Fo r"`, `"For a"`, ... into the same `h`-only rect makes
the already-typed prefix jitter horizontally on each frame.

Measured: same shared prefix, only trailing chars differ ->
23-63 changed pixels; the per-char width drifts 7.65..7.75 px.

## Root cause

`h` is used as the font *point size*, not the on-screen pixel height.

1. `_tex_text` (`src/mem.c`) opens the font at `ptsize = height`
   via `_pico_font_get(font, height)`, then
   `TTF_RenderText_Solid`.
   The surface height is `TTF_FontHeight(ptsize) ~= 1.25 * ptsize`,
   *not* `height`.
2. `pico_output_draw_text_mode` (`src/output.c`) then fits that
   surface into a box of height `height` and width
   `round(height * W0 / H0)` (auto-width via `_f_rat` aspect).
3. Source height `H0 != ` dest height -> a permanent down-scale, and
   the width is rounded *per string*.
   So `x_scale = round(h*W0/H0)/W0` differs from
   `y_scale = h/H0` and drifts with `W0`.

The drift is purely horizontal (independent of ascenders/
descenders) and is the flicker.
It also blurs every text draw, since nothing is ever 1:1.

## Affected code

| file          | place                          | role                         |
|---------------|--------------------------------|------------------------------|
| `src/mem.c`   | `_tex_text`                    | renders surface at ptsize=h  |
| `src/mem.c`   | `_pico_mem_alloc_layer_text`   | wraps surface into a layer    |
| `src/pico.c`  | `_pico_font_get`               | caches font by ptsize         |
| `src/output.c`| `pico_output_draw_text_mode`   | aspect-width + scaled output  |
| `src/geom.c`  | `_raw_dim` / `_f_rat`          | rounded aspect math           |

## Fix

Render the glyphs so the surface height *equals* the requested
pixel height; then the existing downstream math is exactly 1:1
(`round(h*W0/h) = W0`, `scale = 1`), which removes both the flicker
and the blur in one move.

Resolve a pixel height to the point size whose `TTF_FontHeight`
matches it, instead of using the pixel height as the point size:

- in `_tex_text` (or `_pico_font_get`), given target pixel height
  `h`:
    - open a probe at some `p0`, read `fh0 = TTF_FontHeight`
    - pick `p = round(h * p0 / fh0)`; adjust +/-1 until
      `TTF_FontHeight(p) <= h` (largest that fits)
    - cache the font by the *pixel height* key, storing the chosen
      point size
- the rendered surface is then `<= h` tall; with `H0 == h` the
  auto-width path yields `scale = 1`.

Auto-width (`rect.w == 0`) is the case to make exact.
Explicit `rect.w` keeps today's deliberate stretch.

### Alternative (smaller, weaker)

Keep ptsize=h but make the output uniform: drop the independent
width rounding and blit at `(round(W0 * h/H0), h)` derived from a
single scale, or use `SDL_RenderCopyF` (float dest) so the width is
not re-quantised per call.
Removes the flicker but keeps the constant down-scale blur.

## Risks / compat

- Text gets slightly smaller for a given `h`
  (`h` now means pixel height, as documented, not point size).
  Apps that tuned `h` as point size will see ~0.8x text.
- Font cache key changes from point size to pixel height; check
  every `_pico_font_get` caller.
- `pico.get.text` must use the same resolution so measured and
  drawn sizes agree.

## Out of scope

- The downstream typewriter can also avoid the issue by rendering
  the full line once and revealing it; that is a port-side
  workaround, tracked there, not this fix.
