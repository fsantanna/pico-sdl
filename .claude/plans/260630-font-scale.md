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

## Measured: ptsize fixes `'!'`, `'%'` still jitters

2x2 (ptsize off/on x mode). Metric: sum of changed-px in a FIXED
left region (first 8 chars) across CONSECUTIVE typewriter frames
(L vs L+1) -- the faithful typewriter case. self-diff sanity = 0.

| ptsize | `'!'` abs    | `'%'` pct    |
|--------|--------------|--------------|
| OFF    | JITTER (106) | JITTER (329) |
| ON     | STABLE (0)   | JITTER (411) |

- ptsize ON fixes `'!'` (jitter -> stable); `'%'` jitters either way.
- explains `pingus/story/intro.atm` (uses `'%'`): still flickers
  after the ptsize fix.
- (earlier far-apart-length measurement falsely showed `'!'` stable
  when OFF; consecutive-frame comparison is the correct method.)
- likely `'%'` cause: in `pico_output_draw_text_mode` the glyph
  surface renders at `dim.h = round(rect.h*scene.h)` (e.g.
  `round(12.5)=13`) but auto-width scales to the unrounded `12.5`;
  that fractional render-vs-box mismatch makes the per-string width
  rounding drift. `'!'` uses the same integer for both -> ptsize
  alignment is enough.
- the reverted float-blit fixed both modes (removed width
  re-quantisation); a scoped `'%'` fix is the alternative.

### Fix applied: native-size blit for auto-width text

`pico_output_draw_text_mode` (`src/output.c`): when `rect.w == 0`,
blit the glyph surface at its NATIVE size (`W0 x H0`) instead of
recomputing width from the (fractional) aspect. Scale is then
exactly 1 in both axes, so width never re-quantises per frame.
Mode-aware unit conversion for `'!'` / `'%'` / `'#'`.

Re-measured 2x2 (consec-frame metric), fix present:

| ptsize | `'!'`      | `'%'`      |
|--------|------------|------------|
| OFF    | STABLE (0) | STABLE (0) |
| ON     | STABLE (0) | STABLE (0) |

- jitter eliminated in all cases; ptsize now governs only size
  (native height ~= requested only when ptsize ON).
- needs a FULL text-baseline regen (auto-width text size changes).

## Fix

- [x] Inline pixel-height -> point-size resolution in `_tex_text`
      (`src/mem.c`), nested block before `TTF_RenderText_Solid`.
      Single point: drawn text and `pico.get.text` both flow
      through `_tex_text` -> layer dim, so they stay consistent.
      `_pico_font_get` stays keyed by point size.
- [ ] ~~Float-dest blit~~ (reverted): tried `SDL_RenderCopyExF`
      with unrounded float dst in `_pico_layer_output` (+
      `_pico_raw_rect`). Did not resolve the reported jitter and
      had a large blast radius (all blits float-positioned -> every
      visual baseline would churn). Reverted `src/layer.c`,
      `src/geom.c`, `src/_pico.h`. Keeping only the ptsize fix as
      the simpler solution.
- [ ] Switch glyph rendering `Solid` -> `Blended` in `_tex_text`
      (`src/mem.c`): `TTF_RenderText_Solid` -> `TTF_RenderText_Blended`.
      Anti-aliased alpha glyphs soften the sub-pixel scale step and
      blend cleaner when scaled. Requires a FULL test regen (every
      text baseline changes): `make gen` for all text tests + the
      `lua/` equivalents, then review.

Render the glyphs so the surface height *equals* the requested
pixel height; then the existing downstream math is exactly 1:1
(`round(h*W0/h) = W0`, `scale = 1`), which removes both the flicker
and the blur in one move.

Resolve a pixel height to the point size whose `TTF_FontHeight`
matches it, instead of using the pixel height as the point size.

`TTF_FontHeight(ptsize)` is the only way to learn the pixel height,
and it grows ~linearly with `ptsize` (factor `k ~= 1.25`, but
font-dependent and unknown ahead of time).
So invert it by probing once, then correcting.

### Probe algorithm

Given target pixel height `h`:

1. probe: open the font at any `p0` (e.g. `p0 = h`), read
   `fh0 = TTF_FontHeight(p0)`.
   This measures the ratio `k = fh0 / p0`.
2. estimate: `p = round(h * p0 / fh0)` (i.e. `round(h / k)`).
3. correct +/-1 (linearity is not exact, TTF rounds internally):
    - `while (TTF_FontHeight(p)   >  h) p--;`
    - `while (TTF_FontHeight(p+1) <= h) p++;`
   This yields the *largest* point size with
   `TTF_FontHeight(p) <= h`.

Worked example, `h = 30`:

| step     | value                                   |
|----------|-----------------------------------------|
| probe    | `TTF_FontHeight(30) = 40` -> `k = 1.33` |
| estimate | `round(30 * 30 / 40) = 22`              |
| check    | `FontHeight(22) = 29 <= 30`  (fits)     |
| check    | `FontHeight(23) = 31  > 30`  (overflow) |
| chosen   | `ptsize = 22`  ->  surface `H0 = 29`    |

### Notes

- "Fits" means the *typographic* line height
  (`TTF_FontHeight` = ascent + descent) `<= h`, not the visible
  ink.
  This keeps baselines consistent and guarantees text never
  spills past the requested box.
- The surface lands at/just-under `h` (`29 <= 30`), never over.
  Scale becomes a *constant* `h/H0` (`30/29 ~= 1.03`), the same
  every frame -> no wobble.
  The jitter came from the scale *changing*, not from it being
  slightly off 1.
- Do the resolution in `_tex_text` (or a small helper), keeping
  `_pico_font_get` keyed by point size, so other callers are
  unaffected.
- With `H0 ~= h` the auto-width path yields `scale ~= 1`.

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
- Resolve inside `_tex_text` and cache the text layer by pixel
  height `h` (storing the chosen point size); keep
  `_pico_font_get` keyed by point size so its other callers stay
  unchanged.
- Surface lands at/just-under `h` (e.g. 29 for `h=30`); the
  leftover 0..1 px is intended, scale stays constant.
- `pico.get.text` must use the same resolution so measured and
  drawn sizes agree.

## Test

Visual test `tst/text-sizes.c` (merged from `260624-text-sizes`).
Exercises growing text heights, a natural harness for this fix.

### Goal

- 500x500 world/window
- 10x "Hello World!" text output
- 1 per line
- small to big

### Steps

- [x] Create `tst/text-sizes.c`
    - `pico_init(1)`
    - window + world scene dim 500x500
    - loop 10 lines, NW anchor, growing height
    - stack each line below the previous

### Notes

- Follow `tst/font.c` / `tst/colors.c` style.
- Abs `'!'` rects for text.

## Out of scope

- The downstream typewriter can also avoid the issue by rendering
  the full line once and revealing it; that is a port-side
  workaround, tracked there, not this fix.
