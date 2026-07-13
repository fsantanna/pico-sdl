# 260712-rect-circle

Allow omitting `w` or `h` in `draw_rect` / `draw_oval`:
the missing dimension copies the other one in *absolute
pixels*, producing a square / circle in any mode
(`!`, `%`, `#`).

## Rationale

- `_f_rat()` (`src/geom.c:343`) already completes a missing
  `w`/`h` (0) from a `ratio`, and it runs *after* mode scaling,
  i.e. on absolute pixels.
- `draw_rect` / `draw_oval` currently call
  `_pico_abs_rect(rect, NULL, NULL)` with `ratio=NULL`, so
  `w=0`/`h=0` pass through and draw nothing.
- Passing `ratio = {1,1}` makes the absolute missing dimension
  equal the given one: square / circle for free.
- Lua side needs no changes: `C_rel_rect` (`lua/pico.c:233`)
  already defaults missing `w`/`h` to `0`.

## Semantics

| given          | result                                   |
|----------------|------------------------------------------|
| `w` and `h`    | unchanged (rect / oval)                  |
| only `w`       | `h` = same absolute size (square/circle) |
| only `h`       | `w` = same absolute size (square/circle) |
| neither        | unchanged (draws nothing; keeps `dim.c`  |
|                | regression images valid)                 |

Note: in `%` and `#` modes the two axes have different scales;
"square" means equal *pixels*, not equal relative values.

## Steps

- [x] `src/output.c` : `pico_output_draw_rect`
    - pass ratio `{1,1}` only when exactly one of `w`/`h` is 0
- [x] `src/output.c` : `pico_output_draw_oval`
    - same change
- [x] `src/pico.h` : doc comments of `pico_output_draw_rect`
  and `pico_output_draw_oval`
    - document omitted `w`/`h` -> square / circle
- [x] `lua/doc/api.md` : document optional `w`/`h` in
  `draw.rect` / `draw.oval`
- [x] `tst/rect-circle.c` + Makefile entry (after `rect.c`)
- [x] `lua/tst/rect-circle.lua` + lua/Makefile entry
    - same check names as C test (shared `tst/asr/`)

## Won't do

- `neither w nor h` completion: kept as no-op to preserve
  `dim.c` regression images (`dim-01` draws w=h=0)
- `#` mode square-in-tiles variant: pixel-square keeps circles
  round on non-square tiles

## Progress

- code, docs, tests written; syntax-checked with gcc
- 0/0 case settled as no-op (user confirmed)
- asr images generated; all tests pass (C and Lua)
- plan complete
