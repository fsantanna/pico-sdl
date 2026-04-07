# Plan: `_rel_*` helpers + `pico_cv_*_rel` refactor

## Phase 1 (done)

Create window-coordinate conversion functions that compose
`_sdl_pos` + `_cv_log_phy` (forward) and `_cv_phy_log` + inverse
(backward).
Eliminated `_cv_pos_flt_rel` and simplified
`pico_set_mouse` / `pico_get_mouse`.

## Phase 2: `_rel_*` helpers

Extract `_rel_dim`, `_rel_pos`, `_rel_rect` — inverses of
`_sdl_dim`, `_sdl_pos`, `_sdl_rect`.
Place at `// TODO: rel_*` (L331).
Each computes r0 from base+mode (same pattern as `_sdl_*`),
then switches on mode to write back relative coords.

### 8. Add `_rel_dim` (pico.c, `// TODO: rel_*`)

```c
static void _rel_dim (SDL_FDim flt, Pico_Rel_Dim* to, const Pico_Abs_Rect* base) {
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (to->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (to->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(to->up, r0, NULL);
    switch (to->mode) {
        case 'w':
        case '!':
            to->w = flt.w;
            to->h = flt.h;
            break;
        case '%':
            to->w = flt.w / r1.w;
            to->h = flt.h / r1.h;
            break;
        case '#':
            to->w = flt.w / S.layer->view.tile.w;
            to->h = flt.h / S.layer->view.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
}
```

### 9. Add `_rel_pos` (pico.c, after `_rel_dim`)

Extracted from `pico_cv_pos_abs_rel` body.

```c
static void _rel_pos (SDL_FPoint flt, Pico_Rel_Pos* to, const Pico_Abs_Rect* base) {
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (to->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (to->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    Pico_Rel_Rect* up = (Pico_Rel_Rect*) to->up;
    SDL_FRect r1 = _f1(up, r0, NULL);
    switch (to->mode) {
        case 'w':
        case '!':
            to->x = flt.x - r1.x + to->anchor.x;
            to->y = flt.y - r1.y + to->anchor.y;
            break;
        case '%':
            to->x = (flt.x - r1.x + to->anchor.x) / r1.w;
            to->y = (flt.y - r1.y + to->anchor.y) / r1.h;
            break;
        case '#':
            to->x = (flt.x - r1.x) / S.layer->view.tile.w + 1 - to->anchor.x;
            to->y = (flt.y - r1.y) / S.layer->view.tile.h + 1 - to->anchor.y;
            break;
        default:
            assert(0 && "invalid mode");
    }
}
```

### 10. Add `_rel_rect` (pico.c, after `_rel_pos`)

Extracted from `pico_cv_rect_abs_rel` body.

```c
static void _rel_rect (SDL_FRect flt, Pico_Rel_Rect* to, const Pico_Abs_Rect* base) {
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (to->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (to->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(to->up, r0, NULL);
    switch (to->mode) {
        case 'w':
        case '!':
            to->w = flt.w;
            to->h = flt.h;
            to->x = flt.x - r1.x + to->anchor.x * to->w;
            to->y = flt.y - r1.y + to->anchor.y * to->h;
            break;
        case '%':
            to->w = flt.w / r1.w;
            to->h = flt.h / r1.h;
            to->x = (flt.x - r1.x + to->anchor.x * flt.w) / r1.w;
            to->y = (flt.y - r1.y + to->anchor.y * flt.h) / r1.h;
            break;
        case '#':
            to->w = flt.w / (float)S.layer->view.tile.w;
            to->h = flt.h / (float)S.layer->view.tile.h;
            to->x = (flt.x - r1.x) / S.layer->view.tile.w + 1
                    - to->anchor.x + to->anchor.x * to->w;
            to->y = (flt.y - r1.y) / S.layer->view.tile.h + 1
                    - to->anchor.y + to->anchor.y * to->h;
            break;
        default:
            assert(0 && "invalid mode");
    }
}
```

## Phase 3: Refactor all `pico_cv_*_rel`

### 11. Add `pico_cv_dim_abs_rel` + `pico_cv_dim_rel_rel`

```c
void pico_cv_dim_abs_rel (
    const Pico_Abs_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    _rel_dim((SDL_FDim){fr->w, fr->h}, to, base);
}

void pico_cv_dim_rel_rel (
    const Pico_Rel_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Dim abs = pico_cv_dim_rel_abs(fr, base);
    pico_cv_dim_abs_rel(&abs, to, base);
}
```

### 12. Simplify `pico_cv_pos_abs_rel`

```c
void pico_cv_pos_abs_rel (
    const Pico_Abs_Pos* fr, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    _rel_pos((SDL_FPoint){fr->x, fr->y}, to, base);
}
```

### 13. Simplify `pico_cv_pos_win_rel`

```c
void pico_cv_pos_win_rel (SDL_Point phy, Pico_Rel_Pos* to, Pico_Abs_Rect* base) {
    if (to->mode == 'w') {
        _rel_pos((SDL_FPoint){phy.x, phy.y}, to, base);
    } else {
        _rel_pos(_cv_phy_log(phy), to, base);
    }
}
```

### 14. Simplify `pico_cv_rect_abs_rel`

```c
void pico_cv_rect_abs_rel (
    const Pico_Abs_Rect* fr, Pico_Rel_Rect* to, Pico_Abs_Rect* base
) {
    _rel_rect((SDL_FRect){fr->x, fr->y, fr->w, fr->h}, to, base);
}
```

### 15. Declare new functions in `pico.h`

`pico_cv_dim_abs_rel`, `pico_cv_dim_rel_rel`.

## Phase 4: Tests in `tst/cv.c`

### 16. Add `test_dim_abs_rel` roundtrip

Same pattern as `test_pos_abs_rel`: abs→rel→abs == identity.

### 17. Add `test_dim_rel_rel` roundtrip

Same pattern as `test_pos_rel_rel`.

### 18. Run roundtrip matrix for dim

Loop over modes (`!`, `%`, `#`, `w`) × anchors (N/A for dim)
× sample dims × with/without base.

## Summary

| File         | Place              | Description                    |
| ------------ | ------------------ | ------------------------------ |
| `src/pico.c` | `// TODO: rel_*`   | Add `_rel_dim`                 |
| `src/pico.c` | after `_rel_dim`   | Add `_rel_pos`                 |
| `src/pico.c` | after `_rel_pos`   | Add `_rel_rect`                |
| `src/pico.c` | after `pico_cv_dim_rel_abs` | Add `pico_cv_dim_abs_rel` + `pico_cv_dim_rel_rel` |
| `src/pico.c` | `pico_cv_pos_abs_rel` | Simplify → `_rel_pos`       |
| `src/pico.c` | `pico_cv_pos_win_rel` | Simplify → `_rel_pos`       |
| `src/pico.c` | `pico_cv_rect_abs_rel` | Simplify → `_rel_rect`     |
| `src/pico.h` | declarations       | Add `pico_cv_dim_abs_rel`, `pico_cv_dim_rel_rel` |
| `tst/cv.c`   | after existing tests | Add dim roundtrip tests      |

## Status

### Phase 1
- [x] Add `pico_cv_pos_rel_win`
- [x] Add `pico_cv_pos_win_rel`
- [x] Inline `_cv_pos_flt_rel` into `pico_cv_pos_abs_rel`
- [x] Remove `_cv_pos_flt_rel`
- [x] Simplify `pico_set_mouse`
- [x] Simplify `pico_get_mouse`
- [x] Declare in `pico.h`

### Phase 2
- [x] Add `_rel_dim`
- [x] Add `_rel_pos`
- [x] Add `_rel_rect`

### Phase 3
- [x] Add `pico_cv_dim_abs_rel` + `pico_cv_dim_rel_rel`
- [x] Simplify `pico_cv_pos_abs_rel`
- [x] Simplify `pico_cv_pos_win_rel`
- [x] Simplify `pico_cv_rect_abs_rel`
- [x] Declare in `pico.h`

### Phase 4
- [x] Add dim roundtrip tests
- [x] Add individual concrete-value tests
- [x] Inline `_cv_log_phy` / `_cv_phy_log`
- [x] Verification

### Phase 5: `_sdl_*` / `_rel_*` always in logical space

Make `'w'` mode produce/consume logical coords in all internal
helpers.
Only `pico_cv_*_rel_win` / `pico_cv_*_win_rel` end in window
coords.

#### 19. Add `_win_to_log_*` / `_log_to_win_*` helpers

```c
static SDL_FPoint _win_to_log_pos (SDL_FPoint win) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    float rx = (win.x - dst.x) / (float)dst.w;
    float ry = (win.y - dst.y) / (float)dst.h;
    return (SDL_FPoint) {
        src.x + rx*src.w, src.y + ry*src.h
    };
}

static SDL_FPoint _log_to_win_pos (SDL_FPoint log) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    float rx = (log.x - src.x) / (float)src.w;
    float ry = (log.y - src.y) / (float)src.h;
    return (SDL_FPoint) {
        dst.x + rx*dst.w, dst.y + ry*dst.h
    };
}

static SDL_FDim _win_to_log_dim (SDL_FDim win) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    return (SDL_FDim) {
        win.w * src.w / (float)dst.w,
        win.h * src.h / (float)dst.h
    };
}

static SDL_FDim _log_to_win_dim (SDL_FDim log) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    return (SDL_FDim) {
        log.w * dst.w / (float)src.w,
        log.h * dst.h / (float)src.h
    };
}

static SDL_FRect _win_to_log_rect (SDL_FRect win) {
    SDL_FPoint p = _win_to_log_pos(
        (SDL_FPoint){win.x, win.y}
    );
    SDL_FDim d = _win_to_log_dim(
        (SDL_FDim){win.w, win.h}
    );
    return (SDL_FRect) { p.x, p.y, d.w, d.h };
}

static SDL_FRect _log_to_win_rect (SDL_FRect log) {
    SDL_FPoint p = _log_to_win_pos(
        (SDL_FPoint){log.x, log.y}
    );
    SDL_FDim d = _log_to_win_dim(
        (SDL_FDim){log.w, log.h}
    );
    return (SDL_FRect) { p.x, p.y, d.w, d.h };
}
```

#### 20. `_sdl_pos`: separate `'w'` from `'!'`

```c
case 'w': {
    ret = (SDL_FPoint) {
        r1.x + pos->x - pos->anchor.x,
        r1.y + pos->y - pos->anchor.y,
    };
    ret = _win_to_log_pos(ret);
    break;
}
case '!':
    ...
```

#### 21. `_sdl_dim`: separate `'w'` from `'!'`

```c
case 'w': {
    ret = _f3(dim->w, dim->h, ratio);
    if (dim->w == 0) dim->w = ret.w;
    if (dim->h == 0) dim->h = ret.h;
    ret = _win_to_log_dim(ret);
    break;
}
case '!':
    ...
```

#### 22. `_sdl_rect`: separate `'w'` from `'!'`

```c
case 'w': {
    SDL_FDim d = _f3(rect->w, rect->h, ratio);
    ret = (SDL_FRect) {
        r1.x + rect->x - rect->anchor.x*d.w,
        r1.y + rect->y - rect->anchor.y*d.h,
        d.w, d.h
    };
    ret = _win_to_log_rect(ret);
    break;
}
case '!': {
    ...
```

#### 23. `_rel_pos`: separate `'w'` from `'!'`

```c
case 'w': {
    SDL_FPoint wflt = _log_to_win_pos(flt);
    to->x = wflt.x - r1.x + to->anchor.x;
    to->y = wflt.y - r1.y + to->anchor.y;
    break;
}
case '!':
    ...
```

#### 24. `_rel_dim`: separate `'w'` from `'!'`

```c
case 'w': {
    SDL_FDim wflt = _log_to_win_dim(flt);
    to->w = wflt.w;
    to->h = wflt.h;
    break;
}
case '!':
    ...
```

#### 25. `_rel_rect`: separate `'w'` from `'!'`

```c
case 'w': {
    SDL_FRect wflt = _log_to_win_rect(flt);
    to->w = wflt.w;
    to->h = wflt.h;
    to->x = wflt.x - r1.x + to->anchor.x * to->w;
    to->y = wflt.y - r1.y + to->anchor.y * to->h;
    break;
}
case '!':
    ...
```

#### 26. Simplify `pico_cv_pos_rel_win`

Remove `'w'` early return — `_sdl_pos` now returns logical
for all modes:

```c
SDL_Point pico_cv_pos_rel_win (
    const Pico_Rel_Pos* pos, Pico_Abs_Rect* base
) {
    SDL_FPoint fp = _sdl_pos(pos, base);
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    float rx = (fp.x - src.x) / (float)src.w;
    float ry = (fp.y - src.y) / (float)src.h;
    return (SDL_Point) {
        dst.x + rx*dst.w, dst.y + ry*dst.h
    };
}
```

#### 27. Simplify `pico_cv_pos_win_rel`

Remove `'w'` branch:

```c
void pico_cv_pos_win_rel (
    SDL_Point phy, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst,
        &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(
        &S.layer->view.src, NULL
    );
    float rx = (phy.x - dst.x) / (float)dst.w;
    float ry = (phy.y - dst.y) / (float)dst.h;
    SDL_FPoint log = {
        src.x + rx*src.w, src.y + ry*src.h
    };
    _rel_pos(log, to, base);
}
```

#### 28. Update tests

- `tst/cv.c`: `'w'` rel_abs tests now expect logical coords
  (e.g., pos 320,240 on 500×500 win / 100×100 log → 64,48)
- `tst/cv.c`: win conversion tests unchanged
- `tst/todo/w-mouse.c`: should now work (draw with `'w'`)
- `lua/tst/cv.lua`: update `'w'` mode expectations

### Phase 5 status
- [ ] Add `_win_to_log_*` / `_log_to_win_*` helpers
- [ ] `_sdl_pos`: separate `'w'`, convert to logical
- [ ] `_sdl_dim`: separate `'w'`, convert to logical
- [ ] `_sdl_rect`: separate `'w'`, convert to logical
- [ ] `_rel_pos`: convert logical→window for `'w'`
- [ ] `_rel_dim`: convert logical→window for `'w'`
- [ ] `_rel_rect`: convert logical→window for `'w'`
- [ ] Simplify `pico_cv_pos_rel_win`
- [ ] Simplify `pico_cv_pos_win_rel`
- [ ] Update tests
- [ ] Verification
