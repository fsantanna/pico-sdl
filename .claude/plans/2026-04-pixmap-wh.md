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

## 4. Implementation pointers (C)

- `src/pico.c`:
    - `pico_output_draw_pixmap` (or `_buffer` if not yet renamed)
    - `pico_layer_pixmap`
    - mirror the natural-size logic from `pico_output_draw_image`
- `src/pico.h`:
    - check signature of `pico_get_image` for Dim mutation pattern
- Lua side (`lua/pico.c`) — likely no change if it just forwards

## 5. Tests

- `lua/tst/pixmap_raw.lua`: add cases
    - omit `h` → derived from `w` and rows/cols
    - omit `w` → derived from `h`
    - omit both → natural `cols x rows` raw pixels
- `lua/tst/pixmap_pct.lua`: same in `'%'` mode
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

## 8. Steps

- [ ] 8.1 Read `pico_output_draw_image` natural-size code
- [ ] 8.2 Mirror in `pico_output_draw_pixmap`
- [ ] 8.3 Mirror in `pico_layer_pixmap`
- [ ] 8.4 Add Lua-side tests
- [ ] 8.5 `make tests` + `cd lua && make tests` pass
- [ ] 8.6 Update api.md
- [ ] 8.7 Update guide.md §3.3 + revert guide.lua workaround
- [ ] 8.8 Regenerate `lua/tst/asr/guide-03-03-03.png`

## 9. Workaround until shipped

Test and doc carry explicit `h`:

```lua
pico.output.draw.pixmap("pi", PI, {'!', x=50, y=50, w=80, h=80})
```

Track here so the workaround is removed when 8.x completes.
