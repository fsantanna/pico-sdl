# Pixmap auto-size w/h

Make `pixmap` accept partial Rect (omit `w`, `h`, or both) like `image` does.

## 1. Goal

Currently `pico.output.draw.pixmap(name, pixmap, rect)` requires both `w`
and `h` in `rect`. Image and text APIs already accept partial Rect and
derive the missing dimension(s) from natural size / aspect ratio.
This plan extends pixmap to follow the same convention.

## 2. Semantics

Mirrors `pico.output.draw.image`.

| Provided     | Resulting size (cols=C, rows=R)               |
|--------------|-----------------------------------------------|
| `w` and `h`  | as specified (current behavior)               |
| only `w`     | `h = w * R / C` (preserve cell aspect)        |
| only `h`     | `w = h * C / R`                               |
| neither      | natural: `w = C`, `h = R` (logical pixels)    |

Same for `pico.layer.pixmap` (creator).

## 3. Affected APIs

- `pico.output.draw.pixmap (name?, pixmap, rect)` — relaxed Rect
- `pico.layer.pixmap (mode?, up, key, dim, pixmap)` — relaxed Dim

## 4. Implementation pointers — REVISED after re-investigation

**C-side aspect derivation ALREADY WORKS.** No C fix needed.

- `src/aux.hc` `_f3` (lines 5-17): correct per-axis logic
    - both 0 → natural size
    - w==0 → w = h * ratio.w / ratio.h
    - h==0 → h = w * ratio.h / ratio.w
- `src/layers.hc` `_pico_output_draw_layer` (lines ~176-210):
    - `dp = &layer->view.dim` when ANY axis is 0
    - then `_sdl_rect(rect, NULL, dp)` → calls `_f3` for per-axis derivation
- `src/pico.c` `pico_output_draw_image` (lines 1196-1205):
    - explicit `_sdl_dim` path (writes back rect->w/h before drawing)
- `src/pico.c` `pico_output_draw_pixmap` (lines 1184-1194):
    - implicit path via `_pico_output_draw_layer` → `_sdl_rect` → `_f3`
    - both paths end up in same `_f3` logic; no divergence

**Real gap: Lua side only.**

- `lua/pico.c` `l_output_draw_pixmap` (line ~1389):
    - missing `L_dim_default_wh(L, 3)` before `C_rel_rect(L, 3)`
    - `C_checkfieldnum` errors on nil w/h
- `lua/pico.c` `l_output_draw_layer` (lines 1411-1413) already does this — copy pattern
- `pico.layer.pixmap` (creator) — layer dims fixed by design, no fix needed

## 5. Tests

- `lua/tst/pixmap_raw.lua`: existing cases switched to omit `w`/`h`
    - both blocks now omit `w` and `h` (natural size, visual preserved)
    - fails today: Lua `C_rel_rect` rejects nil w/h
- `lua/tst/pixmap_pct.lua`: same in `'%'` mode (also done)
- C aspect cases added (one axis omitted), asr key `pixmap-03`:
    - `tst/pixmap_raw.c`: `w=6, h=0` on 3x3 → expect 6x6 render
    - `tst/pixmap_pct.c`: `w=0, h=0.6` on 3x3 → expect 6x6 cells
    - exercises the C-side fix (replace `||` with per-axis logic)
- C distort cases added (regression guard), asr key `pixmap-04`:
    - `tst/pixmap_raw.c`: `w=6, h=3` on 3x3 → x-stretched (cols ×2, rows ×1)
    - `tst/pixmap_pct.c`: `w=0.6, h=0.3` on 3x3 → x-stretched
    - integer scaling, scale-mode independent
    - distort already works today (both axes nonzero); guards no regression
- Lua aspect+distort cases added in both `_raw` and `_pct` (asr `-03`, `-04`)
- All new asr to be generated after impl lands (`make gen` / `pico.gen`)
- `lua/tst/guide.lua` line 109: revert to `{'!', x=50, y=50, w=80}`
  (drop the `h=80` workaround) once feature lands

## 6. Docs

- `lua/doc/api.md` `pico.output.draw.pixmap` entry: note partial Rect
- `lua/doc/guide.md` §3.3 pixmap: example uses only `w=80`, paragraph
  describes auto-h behavior (parallel to image's "omit w/h" wording)

## 7. Open questions

- [ ] Q1: Natural size unit — is `1 cell = 1 logical pixel` correct?
      Or scale to fill view by default?
- [ ] Q2: For `pico.layer.pixmap` with omitted dim, what's the layer's
      pixel size? Same natural rule (`cols x rows`)?
- [ ] Q3: Buffer copy semantics — already `Pixmap is copied` per
      api.md. Confirm aspect derivation happens before copy.

## 8. Steps — REVISED (scope shrunk after re-investigation)

- [x] 8.1 Modify Lua tests to omit `w`/`h` (pixmap_raw.lua, pixmap_pct.lua)
- [x] 8.2 Add C aspect cases (pixmap_raw.c, pixmap_pct.c) — pass already
- [x] 8.2b Add Lua aspect + distort cases
- [x] 8.2c Add C distort cases
- [x] 8.3 Re-read `_pico_output_draw_layer` / `_sdl_rect` / `_f3`:
        C aspect derivation already works. No C fix needed.
- [x] 8.4 Lua: added `L_dim_default_wh(L, 3)` in `l_output_draw_pixmap`
        (lua/pico.c:1389, mirrors `l_output_draw_layer` pattern)
- [ ] 8.5 ~~C: fix `||`~~ NOT NEEDED (false alarm)
- [ ] 8.6 ~~Mirror in `pico_layer_pixmap`~~ NOT NEEDED (layer dims fixed)
- [ ] 8.7 Generate asr/ for new C tests (`make gen T=pixmap_raw/pct`)
- [ ] 8.8 Generate asr/ for new Lua tests (lua/tst/asr/)
- [ ] 8.9 `make tests` + `cd lua && make tests` pass
- [ ] 8.10 Update api.md (note partial Rect support for pixmap)
- [ ] 8.11 Update guide.md §3.3 + revert guide.lua `h=80` workaround
- [ ] 8.12 Regenerate `lua/tst/asr/guide-03-03-03.png` if needed

## 9. Workaround until shipped

Test and doc carry explicit `h`:

```lua
pico.output.draw.pixmap("pi", PI, {'!', x=50, y=50, w=80, h=80})
```

Track here so the workaround is removed when 8.x completes.
