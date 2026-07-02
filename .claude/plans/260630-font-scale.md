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

### FIX (applied + all tests pass): native-size blit for auto-width text

`pico_output_draw_text_mode` (`src/output.c`): when `rect.w == 0`, set
the destination to the glyph layer's NATIVE `W0 x H0` (mode-aware)
instead of a stretched width + requested height. Scale becomes exactly
1 -> no per-char width re-quantisation (jitter) and no stretch (blur).

- root cause was NOT mode ('!' vs '%') nor only fractional input:
  jitter occurs whenever the requested pixel height has no exact
  ptsize match, so the glyph surface `H0 != requested H` and the blit
  stretches `H0 -> H` with a per-char-rounded width.
  e.g. H=22 -> ptsize 18 -> H0=21 -> stretch 21->22 wobbles.
- verified (W anchor, ptsize ON), consec-frame jitter -> 0:
  H = 14, 20, 22, 29 all STABLE (was JITTER at 14/22/29).
- `pico_get_text` unchanged: still reports the requested (logical)
  size; the native raster size is an internal, unobservable detail.
- heights where `H0 == H` (e.g. 20) are byte-identical to before;
  only the mismatched heights re-render -> partial text baseline regen.
- explicit-width text and non-text layers untouched.

(superseded earlier note below)

### (earlier) native-size blit notes

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

## Fix (applied in `e7e8887`)

The landed fix is THREE coordinated changes, not the ptsize probe
alone -- the measurements above show ptsize alone left `'%'`
jittering; the native-size blit is what actually removes it.

- [x] Native-size blit for auto-width text in
      `pico_output_draw_text_mode` (`src/output.c`): when
      `rect.w == 0`, set dest to the glyph layer's NATIVE `W0 x H0`
      (mode-aware `'!'` / `'%'` / `'#'`). Scale becomes exactly 1 ->
      no per-char width re-quantisation (jitter) and no stretch
      (blur). THE jitter fix.
- [x] Pixel-height -> point-size resolution in `_tex_text`
      (`src/mem.c`), probe block before render (guarded `#if 1`).
      Single point: drawn text and `pico.get.text` both flow through
      `_tex_text` -> layer dim, so they stay consistent.
      `_pico_font_get` stays keyed by point size.
- [x] ~~`Solid` -> `Blended`~~ (REVERTED to `Solid`): Blended was
      tried to soften the residual scale step, but with the native-blit
      fix scale is exactly 1, so its only effect was anti-aliasing --
      which BLURS tiny text into illegibility (`anchor-01` metric
      labels, embedded pixel font ~5px, became gray blobs). The
      native-blit already removes the jitter/blur it was meant to fix,
      so `Solid` (crisp) is the correct render. `_tex_text` uses
      `TTF_RenderText_Solid`.
- [ ] ~~Float-dest blit~~ (reverted): `SDL_RenderCopyExF` with
      unrounded float dst in `_pico_layer_output` (+ `_pico_raw_rect`).
      Did not resolve the jitter and churned every blit baseline.
      Reverted `src/layer.c`, `src/geom.c`, `src/_pico.h`.

### Point-size resolution (detail of the `_tex_text` change)

Render the glyphs so the surface height *equals* the requested pixel
height; then the auto-width math is ~1:1 (`round(h*W0/h) = W0`,
`scale ~= 1`). On its own this was NOT enough for `'%'` (see the 2x2
tables) -- it aligns `'!'`; the native-size blit above is what
removes the residual `'%'` jitter. Both shipped together.

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

## Follow-up: residual VERTICAL snap on reveal (anchor-coupled)

The horizontal jitter is gone, but the downstream typewriter
(`pingus/story/intro.atm`, `'%'`, `anchor=:W`) still shows a 1px
*vertical* snap: as the reveal grows, the already-typed prefix jumps
up ~1px the moment a descender first enters the string.
The native-size blit fixed the X axis and exposed a Y-axis coupling.

### Measured (port, fixed lib installed)

Growing substring of
`"For a long time, the Pingus have lived happily in peace"` into the
SAME `'%'` rect (`x=0.15, h=0.025`); only the anchor differs.

anchor `:W` (vertical center), `y=0.60`, ink bbox:

| len | first new glyph | top | height | bottom |
|-----|-----------------|-----|--------|--------|
| 6   | caps / x-height | 406 | 9      | 415    |
| 8   | ...             | 406 | 9      | 415    |
| 10  | `g` (descender) | 405 | 13     | 418    |
| 20  | ...             | 405 | 13     | 418    |

top 406 -> 405 exactly when `g` (in "lon**g**") appears.

anchor `:NW` (top), same sweep:

| len | top | height | bottom |
|-----|-----|--------|--------|
| 6   | 413 | 9      | 422    |
| 8   | 413 | 9      | 422    |
| 10  | 413 | 13     | 426    |
| 20  | 413 | 13     | 426    |

top CONSTANT (413); the descender grows DOWNWARD only -> no snap.

### Why appending in X moves the line in Y

Not obvious, so spelled out:

1. `TTF_RenderText_*` renders the whole string to one surface whose
   vertical extent depends on CONTENT: it grows when the revealed
   substring first includes a descender (`g p y j q`) or a tall
   ascender.
2. The auto-width fix blits that surface at its NATIVE `W0 x H0`, so
   the destination box HEIGHT = the (content-dependent) glyph height.
3. An anchor positions the box by a reference point:
    - `NW` / top   -> box TOP is fixed (height-invariant).
    - `W` / `C`    -> box CENTER is fixed; `top = center - H0/2`.
    - `SW` / `SE`  -> box BOTTOM is fixed; `top = bottom - H0`.
   Only the top anchor is invariant to `H0`.
4. So when appending a char grows `H0` (a descender entered), any
   non-top anchor recomputes the top edge -> the whole already-typed
   prefix shifts. A center anchor splits the growth, so a
   bottom-growing descender pushes the top UP ~half the delta = the
   observed 1px snap-up.

So the chain is: X edit changes the string's *content* -> content
changes the surface's *height* -> a non-top anchor turns a height
change into a *vertical position* change.
`content -> H0 -> (anchor) -> Y`.

Open question the anchor tests settle: is `H0` genuinely
content-dependent, or is the surface a constant `FontHeight` and the
motion is anchor-rounding of the center?
Either way the cure is the same: place text vertically from a
content-INDEPENDENT height.

### `output_text` adjustment (proposed)

For auto-width text, decouple vertical placement from the tight glyph
surface:

- keep native WIDTH (kills the horizontal wobble), but
- derive the box HEIGHT (and the anchor math) from a
  content-independent metric -- the font line-height
  (`TTF_FontHeight`) or the requested `h` -- so descenders extend
  WITHIN a fixed box instead of resizing it.

Equivalent framing: make the glyph layer's height the font
line-height (constant per size), padding the tight raster, so `H0`
never changes with the string and every anchor is stable.

### Refactor: share the native-dim -> rel-mode conversion

The auto-width native->mode switch (`'!'`/`'%'`/`'#'`) is duplicated
in `pico_output_draw_text_mode` (`src/output.c`) and
`pico_get_text_mode` (`src/get-set.c`).
It is exactly the inverse of `_pico_abs_dim`, and geom.c already has
it as the `static _rel_dim`.

- [x] Promote `_rel_dim` -> `_pico_rel_dim (Pico_Abs_Dim, char mode)`
      (public wrapper in `geom.c`, declared in `src/_pico.h` next to
      `_pico_abs_dim`).
- [x] `get-set.c`: `pico_get_text_mode` auto-width branch uses
      `_pico_rel_dim(nat, rel->mode)` -> switch gone.
- [x] `output.c`: `pico_output_draw_text_mode` auto-width block uses
      `_pico_rel_dim(layer->scene.dim, rect.mode)` -> switch gone.
- Orthogonal to the vertical fix: it changes WHICH dim is fed in, not
  the mode conversion -> safe to land first.

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

### Extend: NW / C / SE anchor reveal (vertical-snap harness)

Add a block that runs the SAME typewriter reveal three times, once
per anchor (`NW`, `C`, `SE`), stacked or in columns, with red boxes
sized to `pico_get_text` and start/mid/end captures.
This is the harness that pins down the `output_text` vertical fix.

Expected BEFORE the fix (settled prefix during the reveal):

| anchor | settled prefix        |
|--------|-----------------------|
| NW     | stable (top pinned)   |
| C      | snaps ~1px (center)   |
| SE     | snaps ~full delta (bottom pinned) |

- [x] `tst/text-sizes.c`: added the NW/C/SE reveal block +
      captures (`text-sizes-05` len 6, `-06` len 10 = descender in,
      `-07` full). Each anchor has a fixed GREEN guide at its
      reference y + a RED measured box; comparing `-05` vs `-06`
      shows whether C/SE text drifts off the guide when `g` enters
      (= content-dependent box height) while NW stays glued. Settles
      the open question by inspection: `make int T=text-sizes`.
- [x] `tst/text-sizes.c`: added Y-snap REPRO (`text-sizes-08`) in
      port conditions -- '%' mode, `h=0.025`, `W` (center-y) anchor.
      Overlays len-8 "For a lo" (RED, no descender) and len-10
      "For a long" (CYAN, 'g') at the SAME anchor. Coincident caps =
      no in-lib snap (surface height = constant `TTF_FontHeight`, so
      likely port-side); a 1px RED fringe = the snap reproduced.
      Sharper than the `'!'` NW/C/SE harness, which cannot snap
      (integer height, constant surface).
- [x] `lua/tst/text-sizes.lua`: full mirror of `text-sizes.c` (all 9
      captures `-01..-09`), added after the design settled to exercise
      the lua binding through the same paths. Passes.
- [x] fix the box height: pin the glyph raster to `TTF_FontHeight` in
      `_tex_text` (`src/mem.c`) so the layer dim is content-independent
      -> a non-top anchor can no longer snap. Blit the rendered surface
      onto an `sfc->w x TTF_FontHeight` box; report `dim.h = fh`.
- [ ] regen text baselines after the fix IF any change. On a machine
      where `sfc->h == FontHeight` (e.g. this one) the padded raster is
      byte-identical -> no churn; on a machine where they differ, the
      pinned height re-rasters -> regen those text baselines.

## Status

ALL TESTS PASS (C + lua). Only the downstream port re-trace remains
(external to this repo). Ready to move to `done/`.

Horizontal-jitter fix (native-blit + ptsize; `Solid` render) landed. The residual VERTICAL snap DID reproduce --
but only where SDL_ttf returns a content-varying surface height. On
this machine `sfc->h == TTF_FontHeight` (constant), so `text-sizes-08/
09` showed no snap and an earlier note wrongly called it a no-op. The
PORT's SDL_ttf returns a tighter surface: instrumenting the placement
rect (`fprintf` of the abs text rect in `pico_output_draw_text_mode`)
caught it live --

    alon  -> h=15 y=329
    along -> h=16 y=328      # 'g' enters: box +1 -> W anchor -1px

The box height was tracking `sfc->h`, which varied with content;
`y = y_ref - 0.5*h` turned +1 height into -1 y.

### Fix (simplified -- final)

Two concerns were separated:
- SNAP is fixed by a content-independent box height (pad raster to
  `TTF_FontHeight`), NOT by ptsize resolution.
- EXACT `h`-as-pixels was the ONLY thing the probe/estimate/correction
  machinery bought, against SDL_ttf's point-size-only API.

Dropped the exactness. `_tex_text` (`src/mem.c`) now:
1. `ttf = _pico_font_get(font, height)` -- open at the requested size
   directly (no probe, no correction loop, no intermediate-ptsize
   font opens). `h` behaves as a POINT size.
2. content-independent box height
   `H = max(TTF_SizeText("|gjpqy"), TTF_FontLineSkip(ttf))`; render
   Blended; blit the tight raster top-aligned onto an `sfc->w x H`
   surface; `assert(sfc->h <= H)`; report `dim = {sfc->w, H}`.

Why `H` this way (see the long investigation):
- NO font metric bounds the raster: `TTF_RenderText` surfaces exceed
  `FontHeight`/`LineSkip`/glyph-metrics by up to +1px (FreeType
  hinting/rounding; SDL_ttf #118). Measured: render > FontHeight in
  28/43 sizes; `LineSkip == FontHeight` for both bundled fonts.
- `TTF_SizeText` == the actual render height (per-string, content-dep).
  So `SizeText` over a reference cell "|gjpqy" (tallest glyph `|` +
  deepest descenders) == the max render over ALL strings, exactly, in
  every (font,size) tested -> content-INDEPENDENT constant, no render.
- `max(.., LineSkip)` keeps a full line box for fonts with leading.
- `assert(sfc->h <= H)` (>=, not ==: most strings are shorter than the
  cell) catches any glyph beyond the reference instead of clipping.
- top-align is proven pixel-stable: the shared prefix of a growing
  string is byte-identical across frames (SDL extends downward only),
  so a constant box => no snap for ANY anchor.

`pico_get_text` (`src/get-set.c`) updated to report the same native
drawn size `{nat.w, H}` (mirrors the draw switch) -> measured ==
drawn. `tst/font.c` get_text asserts updated (10 -> 11 = the cell).

Fixes all three: horizontal jitter (native-width blit, `src/output.c`
unchanged), vertical snap (constant `H`), and descender clipping
(`H` bounds the raster; earlier `FontHeight` pad clipped `font-01`).

Verified: compiles `-Wall -Werror`; `font.c` passes (was asserting),
`font-01` `g` descenders intact; `text-sizes` renders clean.

Remaining:
- [x] FULL text-baseline regen (Solid + new sizing): C `make gen` for
      all text tests + `lua/` equivalents, reviewed. Suite green
      (C + lua).
- [x] debug `fprintf` in `src/output.c` removed.
- [ ] confirm no calling app hard-depends on text being exactly `h` px
      tall; S/SW/SE anchors now float text up by the constant descender
      reserve (line-box bottom, not ink bottom). (port re-trace)

Test-translation fixes surfaced during regen (auto vs explicit width
diverged once auto-width height became the cell `H`, not the requested
`h`):
- `lua/tst/font.lua`: get_text asserts 10 -> 11 (cell); line 13
  switched `*100//1` (floor) -> `(*100+0.5)//1` (round) so `0.11f`
  doesn't truncate to 10.
- `lua/tst/blend_raw.lua`: block-2 rect `w=d.w` -> `w=0` to match C's
  auto-width (`blend_raw.c` uses `w=0`); explicit width was stretching
  the 11px cell to 10px and mismatching the C-generated baseline.

## TODO

- [x] Fix lua tests: text baselines stale after the Blended +
      native-size change (`guide-07-04-01`, ...). Regenerated and
      verified; suite green. The earlier `colors-0X` note was a
      pre-existing alpha / screenshot-timing flake unrelated to
      font-scale; no longer failing on re-run.

## Follow-up: reference cell under-bounds decorative fonts

The constant `H = max(SizeText("|gjpqy"), LineSkip)` was verified only
against the two bundled fonts (`DejaVuSans`, `tiny`).
A downstream app (SNKRX, font `PixulBrush.ttf`) aborts on the safety
net `assert(sfc->h <= H)` (`_tex_text`, `src/mem.c`): some glyph
rasters taller than the `"|gjpqy"` cell, so the cell is NOT a
universal bound.

    lua5.4: src/mem.c:262: _tex_text: Assertion
    `sfc->h <= H && "text raster exceeds reference cell"' failed.
    Aborted (core dumped)

### Root cause

`"|gjpqy"` assumes `|` is the tallest glyph and `gjpqy` the deepest.
That holds for the bundled fonts but not for decorative / brush
fonts, whose glyphs (flourishes, tall caps, deep tails) can exceed
both extremes.
The bound is font-specific, yet the code hardcodes one literal cell.

Discovered (probe): the culprit glyph is a PLAIN capital `A`, not a
decorative outlier -- in PixulBrush `A` renders 1px taller than the
`"|gjpqy"` cell (`10 -> 11` at pt6, `36 -> 37` at pt22). `SizeText`
and `RenderText` AGREE (both 11 for `A`), so it is a wrong-reference
string, not a metric under-report. Nearly all caps/digits overshoot.

### Fix (applied -- minimal): add `A` to the reference cell

`_tex_text` (`src/mem.c`): reference `"|gjpqy"` -> `"|gjpqyA"`.
- [x] validated: over pt 6..120, ALL printable ASCII of PixulBrush
      now has `render <= H` (0 fails).
- [x] no baseline churn: `SizeText("|gjpqyA") == SizeText("|gjpqy")`
      for both bundled fonts (DejaVu, tiny) at every size -> `H`
      unchanged there; existing C + lua baselines stay valid.
- [x] `tst/font-brush.c`: minimal repro (draw `"A"` @h=6 with
      `PixulBrush.ttf`) -- aborted before, renders now.
- still a fixed literal: a future font could exceed even `A`; the
  glyph-scan below stays the robust fallback if that recurs.

### Fix direction (fallback, if a fixed cell is ever defeated again)

Keep `H` content-INDEPENDENT (the anchor-stability invariant), but
make it a true per-`(font,size)` bound instead of a fixed cell:

- compute `H` as the max raster height over the font's printable
  glyph set (`0x20..0x7E`), measured once per `(font,size)` and
  cached alongside the font entry;
- a single-line string's raster height is bounded by its glyphs'
  max ascent + max descent, so scanning the glyphs bounds every
  string while staying content-independent;
- keep `assert(sfc->h <= H)` as the safety net, now against the
  computed bound rather than the literal cell.

Weaker alternative: just widen the reference string -- but any fixed
literal is defeated by the next font, so prefer the scan.

### Font (how to copy)

Copy the offending font into `res/` (the test-font dir, alongside
`DejaVuSans.ttf`):

    cp /x/x/SNKRX/assets/fonts/PixulBrush.ttf \
       /x/pico-sdl/res/PixulBrush.ttf

Source: SNKRX `../assets/fonts/PixulBrush.ttf` (~15 KB).

### Test case to add

New case in `tst/font.c` (following the `../res/DejaVuSans.ttf`
pattern), or a dedicated `tst/font-brush.c`:

- [ ] `pico_set_pencil_font("../res/PixulBrush.ttf")`.
- [ ] draw the full printable-ASCII line at the SNKRX sizes
      (button `h`, plus the hover-scaled `1.3x`) -- the exact case
      that aborts today.
- [ ] regression asserts: renders WITHOUT aborting (`sfc->h <= H`),
      and `pico_get_text` height is CONSTANT across strings of
      different content (proves `H` stayed content-independent).
- [ ] capture a baseline (`font-brush-01`).

## Out of scope

- The downstream typewriter can also avoid the issue by rendering
  the full line once and revealing it; that is a port-side
  workaround, tracked there, not this fix.
