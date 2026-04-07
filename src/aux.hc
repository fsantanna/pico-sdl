///////////////////////////////////////////////////////////////////////////////
// rel hierarchy resolution: _f1 (recursive up), _f2 (%), _f3 (aspect ratio)
///////////////////////////////////////////////////////////////////////////////

static SDL_FDim _f3 (float w, float h, const Pico_Abs_Dim* ratio) {
    if (ratio!=NULL && (w==0 || h==0)) {
        if (w == 0 && h == 0) {
            w = ratio->w;
            h = ratio->h;
        } else if (w == 0) {
            w = h * ratio->w / ratio->h;
        } else {
            h = w * ratio->h / ratio->w;
        }
    }
    return (SDL_FDim) { w, h };
}

static SDL_FRect _f2 (
    SDL_FRect   dn,
    Pico_Anchor anc,
    SDL_FRect   up,
    const Pico_Abs_Dim* ratio
) {
    SDL_FDim d = _f3(dn.w*up.w, dn.h*up.h, ratio);
    return (SDL_FRect) {
        up.x + dn.x*up.w - anc.x*d.w,
        up.y + dn.y*up.h - anc.y*d.h,
        d.w, d.h,
    };
}

static SDL_FRect _f1 (
    const Pico_Rel_Rect* r,
    SDL_FRect base,
    const Pico_Abs_Dim* ratio
) {
    if (r == NULL) {
        return base;
    } else {
        SDL_FRect abs = { r->x, r->y, r->w, r->h };
        SDL_FRect tmp = _f1(r->up, base, NULL);
        return _f2(abs, r->anchor, tmp, ratio);
    }
}

///////////////////////////////////////////////////////////////////////////////
// _sdl_*: rel -> float (logical coords)
///////////////////////////////////////////////////////////////////////////////

static SDL_FDim _sdl_dim (
    Pico_Rel_Dim*        dim,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (dim->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (dim->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(dim->up, r0, NULL);
    SDL_FDim ret;
    switch (dim->mode) {
        case 'w':
        case '!':
            ret = _f3(dim->w, dim->h, ratio);
            if (dim->w == 0) dim->w = ret.w;
            if (dim->h == 0) dim->h = ret.h;
            break;
        case '%':
            ret = _f3(dim->w * r1.w, dim->h * r1.h, ratio);
            if (dim->w == 0) dim->w = ret.w / r1.w;
            if (dim->h == 0) dim->h = ret.h / r1.h;
            break;
        case '#':
            ret = _f3(dim->w * S.layer->view.tile.w, dim->h * S.layer->view.tile.h, ratio);
            if (dim->w == 0) dim->w = ret.w / S.layer->view.tile.w;
            if (dim->h == 0) dim->h = ret.h / S.layer->view.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FPoint _sdl_pos (
    const Pico_Rel_Pos*  pos,
    const Pico_Abs_Rect* base
) {
    SDL_FPoint ret;
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (pos->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (pos->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(pos->up, r0, NULL);
    switch (pos->mode) {
        case 'w':
        case '!':
            ret = (SDL_FPoint) {
                r1.x + pos->x - pos->anchor.x,
                r1.y + pos->y - pos->anchor.y,
            };
            break;
        case '%':
            ret = (SDL_FPoint) {
                r1.x + pos->x*r1.w - pos->anchor.x,
                r1.y + pos->y*r1.h - pos->anchor.y,
            };
            break;
        case '#':
            ret = (SDL_FPoint) {
                r1.x + (pos->x - 1 + pos->anchor.x)*S.layer->view.tile.w,
                r1.y + (pos->y - 1 + pos->anchor.y)*S.layer->view.tile.h,
            };
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FRect _sdl_rect (
    const Pico_Rel_Rect* rect,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FRect ret;
    SDL_FRect r0;
    if (base == NULL) {
        r0 = (SDL_FRect) {
            0, 0,
            (rect->mode == 'w') ? S.win.dim.w : S.layer->view.dim.w,
            (rect->mode == 'w') ? S.win.dim.h : S.layer->view.dim.h,
        };
    } else {
        r0 = (SDL_FRect) { base->x, base->y, base->w, base->h };
    }
    SDL_FRect r1 = _f1(rect->up, r0, NULL);
    switch (rect->mode) {
        case 'w':
        case '!': {
            SDL_FDim d = _f3(rect->w, rect->h, ratio);
            ret = (SDL_FRect) {
                r1.x + rect->x - rect->anchor.x*d.w,
                r1.y + rect->y - rect->anchor.y*d.h,
                d.w,
                d.h
            };
            break;
        }
        case '%':
            ret = _f1(rect, r0, ratio);
            break;
        case '#': {
            SDL_FDim d = _f3 (
                rect->w * S.layer->view.tile.w,
                rect->h * S.layer->view.tile.h,
                ratio
            );
            ret = (SDL_FRect) {
                r1.x + (rect->x - 1 + rect->anchor.x)*S.layer->view.tile.w - rect->anchor.x*d.w,
                r1.y + (rect->y - 1 + rect->anchor.y)*S.layer->view.tile.h - rect->anchor.y*d.h,
                d.w,
                d.h
            };
            break;
        }
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// _rel_*: float (logical coords) -> rel
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// _abs_*: float -> abs (rounding)
///////////////////////////////////////////////////////////////////////////////

// use floorf(. + 0.5f) toward +inf: makes NW/C behave the same

static Pico_Abs_Dim _abs_dim (const SDL_FDim* f) {
    return (Pico_Abs_Dim) {
        floorf(f->w+0.5f), floorf(f->h+0.5f)
    };
}

static Pico_Abs_Pos _abs_pos (const SDL_FPoint* f) {
    return (Pico_Abs_Pos) {
        floorf(f->x+0.5f), floorf(f->y+0.5f)
    };
}

static Pico_Abs_Rect _abs_rect (const SDL_FRect* f) {
    return (Pico_Abs_Rect) {
        floorf(f->x+0.5f), floorf(f->y+0.5f),
        floorf(f->w+0.5f), floorf(f->h+0.5f)
    };
}

