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
