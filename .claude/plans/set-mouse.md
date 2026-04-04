# Plan: `pico_set_mouse` (issue #110)

## Goal

Add `pico_set_mouse(Pico_Rel_Pos* pos)` that warps the mouse
cursor to the position described by relative coordinates, inverse
of `pico_get_mouse`.

## Context (post-merge with main)

- `G.tgt` replaced by `G.presenting` (only for present logic)
- Mode `'w'` (window physical pixels) added to all conversion
  functions (`_sdl_pos`, `_sdl_dim`, `_sdl_rect`,
  `_cv_pos_flt_rel`)
- `'w'` falls through to `'!'` using `S.win.dim` as base
- `_sdl_pos` with mode `'w'` returns physical window coords
  directly — no log->phy conversion needed

## Conversion Chain

```
pico_get_mouse:  physical -> logical -> relative
                 _cv_phy_log  _cv_pos_flt_rel

pico_set_mouse:
  mode 'w':      _sdl_pos -> physical (direct)
  mode !/%/#:    _sdl_pos -> logical -> _cv_log_phy (new)
```

## Steps

### 1. `src/pico.c` — add `_cv_log_phy` helper (after line 359)
- [x] Inverse of `_cv_phy_log` (line 350)
- [x] Converts logical `SDL_FPoint` to physical `SDL_Point`
- [x] Math: `phy = dst.pos + ((log - src.pos) / src.dim) * dst.dim`

### 2. `src/pico.c` — add `pico_set_mouse` (after `pico_get_mouse`)
- [x] Signature: `void pico_set_mouse (Pico_Rel_Pos* pos)`
- [x] If mode `'w'`: `_sdl_pos(pos, NULL)` gives physical directly
- [x] Otherwise: `_sdl_pos(pos, NULL)` gives logical,
  then `_cv_log_phy` gives physical
- [x] Call `SDL_WarpMouseInWindow(G.win, phy.x, phy.y)`
- [x] Call `SDL_PumpEvents()`

### 3. `src/pico.h` — declare `pico_set_mouse` (after line 421)
- [x] Add declaration with doc comment

### 4. `tst/mouse.c` — replace `SDL_WarpMouseInWindow`
- [x] Replace raw SDL calls with `pico_set_mouse` using `'w'` mode
  (existing tests use physical coords)
- [x] Add roundtrip tests: `set(rel) -> get(rel) -> assert`
  (`'!'`, `'%'`, `'!' zoomed`)
- [x] Remove `extern SDL_Window* pico_win` and `#include <SDL2/SDL.h>`

### 5. `tst/mouse-rect-click.c` — replace `SDL_WarpMouseInWindow`
- [ ] Replace raw SDL calls with `pico_set_mouse`
- [ ] Remove `extern SDL_Window* pico_win`

### 6. `tst/tiles.c` — replace `SDL_WarpMouseInWindow`
- [ ] Replace raw SDL calls with `pico_set_mouse`
- [ ] Remove `extern SDL_Window* pico_win`

### 7. `lua/pico.c` — add Lua binding `l_set_mouse`
- [ ] `pico.set.mouse({'w', x=100, y=200})`
- [ ] `pico.set.mouse({'%', x=0.5, y=0.5, up=rect})`
- [ ] Register in `ll_set` table

### 8. `lua/tst/` — Lua test ports
- [ ] Port `tst/mouse.c` -> `lua/tst/mouse.lua`
- [ ] Port `tst/mouse-rect-click.c` ->
  `lua/tst/mouse-rect-click.lua`
- [ ] Port `tst/tiles.c` mouse tests ->
  `lua/tst/tiles.lua` (if applicable)

### 9. Cleanup
- [ ] Update `valgrind.supp` if `pico.c` line numbers shifted
