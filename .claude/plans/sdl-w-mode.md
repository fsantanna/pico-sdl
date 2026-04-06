# Plan: `'w'` mode returns layer coordinates in `_sdl_*`

## Context

`'w'` mode currently resolves against window dimensions but
returns window-pixel coordinates.
All other modes (`'!'`, `'%'`, `'#'`) return layer coordinates.
This inconsistency forces every drawing function to add a
post-conversion step for `'w'`.

Fix: make `_sdl_pos`, `_sdl_rect`, `_sdl_dim` convert `'w'`
results from window pixels to layer coordinates internally.
This keeps mode switching centralized in `_sdl_*` — no changes
to any drawing function or `_pico_output_draw_layer`.

## Helpers

### Add `_cv_phy_log_rect` (pico.c, after `_cv_phy_log` line 357)

```c
static SDL_Rect _cv_phy_log_rect (SDL_Rect phy) {
    SDL_Rect dst = pico_cv_rect_rel_abs(&S.layer->view.dst, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h});
    SDL_Rect src = pico_cv_rect_rel_abs(&S.layer->view.src, NULL);
    float sx = src.w / (float)dst.w;
    float sy = src.h / (float)dst.h;
    return (SDL_Rect) {
        src.x + (phy.x - dst.x) * sx,
        src.y + (phy.y - dst.y) * sy,
        phy.w * sx,
        phy.h * sy,
    };
}
```

### Add `_cv_phy_log_flt` (float version, for `_sdl_pos`)

```c
static SDL_FPoint _cv_phy_log_flt (SDL_FPoint phy) {
    SDL_Rect dst = pico_cv_rect_rel_abs(&S.layer->view.dst, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h});
    SDL_Rect src = pico_cv_rect_rel_abs(&S.layer->view.src, NULL);
    float rx = (phy.x - dst.x) / (float)dst.w;
    float ry = (phy.y - dst.y) / (float)dst.h;
    return (SDL_FPoint) {
        src.x + rx*src.w,
        src.y + ry*src.h,
    };
}
```

## Changes to `_sdl_*` (src/pico.c)

### `_sdl_pos` (line 248) — separate `'w'` from `'!'`

```c
case 'w':
    ret = (SDL_FPoint) {
        r1.x + pos->x - pos->anchor.x,
        r1.y + pos->y - pos->anchor.y,
    };
    ret = _cv_phy_log_flt(ret);
    break;
case '!':
    ...
```

### `_sdl_rect` (line 291) — separate `'w'` from `'!'`

```c
case 'w': {
    SDL_FDim d = _f3(rect->w, rect->h, ratio);
    SDL_FRect win = {
        r1.x + rect->x - rect->anchor.x*d.w,
        r1.y + rect->y - rect->anchor.y*d.h,
        d.w,
        d.h
    };
    SDL_Rect ri = _cv_phy_log_rect(_fi_rect(&win));
    ret = (SDL_FRect) { ri.x, ri.y, ri.w, ri.h };
    break;
}
case '!': {
    ...
```

### `_sdl_dim` (line 209) — separate `'w'` from `'!'`

Write-back happens before scaling (user struct stays in window
pixels).

```c
case 'w': {
    ret = _f3(dim->w, dim->h, ratio);
    if (dim->w == 0) dim->w = ret.w;
    if (dim->h == 0) dim->h = ret.h;
    SDL_Rect dst = pico_cv_rect_rel_abs(&S.layer->view.dst, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h});
    SDL_Rect src = pico_cv_rect_rel_abs(&S.layer->view.src, NULL);
    ret.w *= src.w / (float)dst.w;
    ret.h *= src.h / (float)dst.h;
    break;
}
case '!':
    ...
```

## Caller adaptations (src/pico.c)

### `pico_set_mouse` (line 740) — simplify

All modes now return layer coords, so always use `_cv_log_phy`:

```c
void pico_set_mouse (Pico_Rel_Pos* pos) {
    _pico_guard();
    pos->anchor = PICO_ANCHOR_NW;
    SDL_FPoint fp = _sdl_pos(pos, NULL);
    SDL_Point phy = _cv_log_phy(fp);
    SDL_WarpMouseInWindow(G.win, phy.x, phy.y);
    SDL_PumpEvents();
}
```

## No changes needed

- Drawing functions (all 9)
- `_pico_output_draw_layer` (layers.hc)
- `pico_cv_*_rel_abs` public wrappers
- `pico_get_text_mode` / `pico_get_image_dim`
- `_cv_pos_flt_rel` / `pico_cv_rect_abs_rel` (inverse
  converters — 'w' mode not used for inverse)

## Verification

```bash
make tests
cd lua/ && make tests
```

## Status

- [x] Step 0: view-target test (C + Lua)
- [x] Fix `_pico_output_draw_layer` rect=NULL symmetry
- [ ] Add `_cv_phy_log_rect` + `_cv_phy_log_flt` helpers
- [ ] `_sdl_dim`: separate 'w' from '!'
- [ ] `_sdl_pos`: separate 'w' from '!'
- [ ] `_sdl_rect`: separate 'w' from '!'
- [ ] `pico_set_mouse`: simplify
- [ ] Verification
