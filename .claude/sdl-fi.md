# Replace `_sdl_*`/`_fi_*` Pairs with `pico_cv_*` Wrappers

## Existing wrappers

```c
// pico.c:331
Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base);

// pico.c:336
Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base);
```

Both pass `NULL` as ratio (3rd arg to `_sdl_rect`).

No `pico_cv_dim_rel_abs` exists yet — needs to be created.

---

## Sites to replace

### rect: `_sdl_rect` + `_fi_rect` → `pico_cv_rect_rel_abs`

| Line | Function | base | ratio | Replace? |
|------|----------|------|-------|----------|
| 442,444 | `pico_vs_pos_rect` | NULL | NULL | Yes |
| 449-452 | `pico_vs_rect_rect` | NULL | NULL | Yes (×2) |
| 1435-1436 | `_pico_output_draw_layer` | NULL | `dp` | **No** — ratio used |
| 1469-1470 | `pico_output_draw_oval` | NULL | NULL | Yes |
| 1516-1517 | `pico_output_draw_rect` | NULL | NULL | Yes |
| 1783-1784 | `pico_output_screenshot` | `&phy` | NULL | Yes |

### pos: `_sdl_pos` + `_fi_pos` → `pico_cv_pos_rel_abs`

| Line | Function | base | Replace? |
|------|----------|------|----------|
| 441,443 | `pico_vs_pos_rect` | NULL | Yes |
| 1460-1463 | `pico_output_draw_line` | NULL | Yes (×2) |
| 1493-1494 | `pico_output_draw_pixel` | NULL | Yes |
| 1503-1504 | `pico_output_draw_pixels` | NULL | Yes (loop) |
| 1532-1533 | `pico_output_draw_poly` | NULL | Yes (loop) |
| 1571-1576 | `pico_output_draw_tri` | NULL | Yes (×3) |

### dim: `_sdl_dim` + `_fi_dim` → `pico_cv_dim_rel_abs` (new)

New wrapper needed:
```c
Pico_Abs_Dim pico_cv_dim_rel_abs (Pico_Rel_Dim* dim, Pico_Abs_Rect* base) {
    SDL_FDim df = _sdl_dim(dim, base, NULL);
    return _fi_dim(&df);
}
```

| Line | Function | base | ratio | Replace? |
|------|----------|------|-------|----------|
| 872-873 | `pico_set_view` | NULL | NULL | Yes |
| 946-948 | `pico_set_window` | NULL | NULL | Yes |

Note: other `_sdl_dim` calls (648, 651, 734, 737, 1418, 1560, 1565)
use `_sdl_dim` alone — they rely on side-effects (auto-fill `w==0`/`h==0`)
or pass a ratio arg, so no `_fi_dim` follows.

---

## Skip

- `_pico_output_draw_layer` (line 1435): uses `ratio` arg for aspect ratio
  preservation when `w==0` or `h==0`. Cannot use `pico_cv_rect_rel_abs`.

---

## Summary

- **12 sites** replaceable (5 rect, 5 pos, 2 dim)
- **1 new wrapper**: `pico_cv_dim_rel_abs` (pico.c + pico.h)
- **1 site** skipped (ratio arg)
