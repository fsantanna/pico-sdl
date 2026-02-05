# Replace `_sdl_*`/`_fi_*` Pairs with `pico_cv_*` Wrappers

**Status: DONE**

## Wrappers

```c
Pico_Abs_Dim  pico_cv_dim_rel_abs  (Pico_Rel_Dim* dim, Pico_Abs_Rect* base);   // NEW
Pico_Abs_Pos  pico_cv_pos_rel_abs  (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base);
Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base);
```

All pass `NULL` as ratio (3rd arg to `_sdl_rect`/`_sdl_dim`).

---

## Replaced sites (12)

### rect (5): `_sdl_rect` + `_fi_rect` → `pico_cv_rect_rel_abs`

- `pico_vs_pos_rect` — done
- `pico_vs_rect_rect` (×2) — done
- `pico_output_draw_oval` — done
- `pico_output_draw_rect` — done
- `pico_output_screenshot` (base=`&phy`) — done

### pos (5): `_sdl_pos` + `_fi_pos` → `pico_cv_pos_rel_abs`

- `pico_vs_pos_rect` — done
- `pico_output_draw_line` (×2) — done
- `pico_output_draw_pixel` — done
- `pico_output_draw_pixels` (loop) — done
- `pico_output_draw_poly` (loop) — done
- `pico_output_draw_tri` (×3) — done

### dim (2): `_sdl_dim` + `_fi_dim` → `pico_cv_dim_rel_abs`

- `pico_set_view` — done
- `pico_set_window` (G.tgt context preserved) — done

---

## Skipped (1)

- `_pico_output_draw_layer`: uses `ratio` arg for aspect ratio
  preservation when `w==0` or `h==0`. Cannot use `pico_cv_rect_rel_abs`.

---

## Notes

- Other `_sdl_dim` calls use `_sdl_dim` alone (side-effects or ratio
  arg) — no `_fi_dim` follows, so no replacement needed.
- `pico_cv_dim_rel_abs` also exposed in Lua as `pico.cv.dim()`.
- Tests added in `tst/cv.c` and `lua/tst/cv.lua`.
