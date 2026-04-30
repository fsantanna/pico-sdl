# Rect/Dim default w=h=0 in Lua bindings

## Goal

Allow Lua callers to omit `w` and/or `h` from `Rect` and `Dim` tables passed
to `pico.set.view`, `pico.output.draw.*`, etc.  Missing fields default to `0`,
which downstream C code already interprets as "use the layer's intrinsic
dimension" via this fallback in `_pico_output_draw_layer`:

    if (rect->w == 0 || rect->h == 0) {
        dp = &layer->view.dim;
    }

Currently only `pico.layer.sub`'s `crop` arg defaults via `L_dim_default_wh`
(see `lua/pico.c:1282`).  Other rect-parsing sites use `C_checkfieldnum`
which errors when the field is missing.

## Affected files

- `lua/pico.c`
    - `C_checkfieldnum` (~lines 23-34): keep as-is — used for required fields
    - `C_abs_dim`, `C_abs_rect`, `C_abs_pos` (~38-65): factor in default-wh
      logic OR add a new helper `C_optfieldnum` returning 0 when missing
    - `C_rel_rect` (location TBD): apply default-wh
    - All sites parsing rect/dim arguments — audit list:
        - `l_set_view`     (target / source / clip / dim / tile)
        - `l_output_draw_image`, `_rect`, `_oval`, `_layer`, `_pixmap`, `_text`
        - `l_layer_empty`, `_pixmap`, `_sub` (already partially handled)
        - `l_get_image`, `l_get_text`, `l_get_video`
        - `l_set_window` (dim)

## Steps

1. Add helper `static float C_optfieldnum(L, i, k, default)` that returns
   `default` when field is missing/nil/non-numeric.
2. Refactor `C_abs_dim`, `C_abs_rect`, `C_rel_rect`, `C_rel_dim` to use
   `C_optfieldnum(.., 0.0)` for `w`/`h` (keep `x`/`y` required).
3. Remove now-redundant explicit `L_dim_default_wh` calls.
4. Verify all callers still error appropriately when truly required fields
   (`x`, `y`) are missing.

## Test plan

- `cd lua && make tests` — full visual regression suite.
- Specifically test `pico.set.view { target = {'%', x=0.5, y=0.3, h=0.4} }`
  (no `w`) — should auto-scale width to text's intrinsic ratio.
- `tst/guide.lua` §7.4 — once binding accepts missing `w`, drop the explicit
  `w=0` from hello/world targets.

## Reference

- Issue surfaced when adapting `lua/doc/guide.md` §7.4 (hierarchical
  compositing demo).
- See conversation 2026-04-29.

## Pending follow-ups

- After this lands: revert `lua/doc/guide.md` and `lua/tst/guide.lua` §7.4
  hello/world targets to omit `w` (cleaner doc).
