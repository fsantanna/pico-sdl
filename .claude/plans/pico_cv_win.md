# Plan: `pico_cv_pos_rel_win` / `pico_cv_pos_win_rel`

## Context

Create window-coordinate conversion functions that compose
`_sdl_pos` + `_cv_log_phy` (forward) and `_cv_phy_log` + inverse
(backward).
This eliminates `_cv_pos_flt_rel` and simplifies
`pico_set_mouse` / `pico_get_mouse`.

## Changes

### 1. Add `pico_cv_pos_rel_win` (pico.c, after `_cv_log_phy`)

```c
SDL_Point pico_cv_pos_rel_win (
    const Pico_Rel_Pos* pos, Pico_Abs_Rect* base
) {
    SDL_FPoint fp = _sdl_pos(pos, base);
    if (pos->mode == 'w') {
        return (SDL_Point) { fp.x, fp.y };
    } else {
        return _cv_log_phy(fp);
    }
}
```

### 2. Add `pico_cv_pos_win_rel` (pico.c, after above)

```c
void pico_cv_pos_win_rel (
    SDL_Point phy, Pico_Rel_Pos* to,
    Pico_Abs_Rect* base
) {
    SDL_FPoint fr = _cv_phy_log(phy);
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            S.layer->view.dim.w, S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) {
            base->x, base->y, base->w, base->h
        };
    }
    Pico_Rel_Rect* up = (Pico_Rel_Rect*) to->up;
    SDL_FRect r1 = _f1(up, r0, NULL);
    switch (to->mode) {
        case 'w':
        case '!':
            to->x = fr.x - r1.x + to->anchor.x;
            to->y = fr.y - r1.y + to->anchor.y;
            break;
        case '%':
            to->x = (fr.x - r1.x + to->anchor.x) / r1.w;
            to->y = (fr.y - r1.y + to->anchor.y) / r1.h;
            break;
        case '#':
            to->x = (fr.x - r1.x) / S.layer->view.tile.w + 1 - to->anchor.x;
            to->y = (fr.y - r1.y) / S.layer->view.tile.h + 1 - to->anchor.y;
            break;
        default:
            assert(0 && "invalid mode");
    }
}
```

Note: r0 always uses layer dims because `fr` is already in
layer coords (from `_cv_phy_log`). No `'w'` special case for r0.

### 3. Inline `_cv_pos_flt_rel` into `pico_cv_pos_abs_rel`

```c
void pico_cv_pos_abs_rel (
    const Pico_Abs_Pos* fr, Pico_Rel_Pos* to,
    Pico_Abs_Rect* base
) {
    SDL_FPoint flt = { fr->x, fr->y };
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (to->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (to->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) {
            base->x, base->y, base->w, base->h
        };
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

### 4. Remove `_cv_pos_flt_rel`

### 5. Simplify `pico_set_mouse`

```c
void pico_set_mouse (Pico_Rel_Pos* pos) {
    _pico_guard();
    pos->anchor = PICO_ANCHOR_NW;   // TODO
    SDL_Point phy = pico_cv_pos_rel_win(pos, NULL);
    SDL_WarpMouseInWindow(G.win, phy.x, phy.y);
    SDL_PumpEvents();
}
```

### 6. Simplify `pico_get_mouse`

```c
if (mode == 'w') {
    m.x = phy.x;
    m.y = phy.y;
} else {
    Pico_Rel_Pos rel = {
        .mode=mode, .anchor=PICO_ANCHOR_NW, .up=rect
    };
    pico_cv_pos_win_rel(phy, &rel, NULL);
    m.x = rel.x;
    m.y = rel.y;
}
```

### 7. Declare in `src/pico.h`

## Summary

| File         | Place                | Description                        |
| ------------ | -------------------- | ---------------------------------- |
| `src/pico.c` | after `_cv_log_phy`  | Add `pico_cv_pos_rel_win`          |
| `src/pico.c` | after above          | Add `pico_cv_pos_win_rel`          |
| `src/pico.c` | `pico_cv_pos_abs_rel`| Inline `_cv_pos_flt_rel`           |
| `src/pico.c` | `_cv_pos_flt_rel`    | Remove                             |
| `src/pico.c` | `pico_set_mouse`     | Use `pico_cv_pos_rel_win`          |
| `src/pico.c` | `pico_get_mouse`     | Use `pico_cv_pos_win_rel`          |
| `src/pico.h` | declarations         | Add 2 new public functions         |

## Status

- [ ] Add `pico_cv_pos_rel_win`
- [ ] Add `pico_cv_pos_win_rel`
- [ ] Inline `_cv_pos_flt_rel` into `pico_cv_pos_abs_rel`
- [ ] Remove `_cv_pos_flt_rel`
- [ ] Simplify `pico_set_mouse`
- [ ] Simplify `pico_get_mouse`
- [ ] Declare in `pico.h`
- [ ] Verification
