///////////////////////////////////////////////////////////////////////////////
// _f_rat: fill missing w/h from aspect ratio
// _f_pct: resolve a '%' rect onto a base SDL_FRect
///////////////////////////////////////////////////////////////////////////////

static SDL_FDim _f_rat (float w, float h, const Pico_Abs_Dim* ratio) {
    if (ratio == NULL) {
        // no ratio to use
    } else if (w!=0 && h!=0) {
        // forced w,h
    } else {
        // calculate w,h based on h,w & ratio
        assert(ratio!=NULL && (w==0 || h==0));
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

static SDL_FRect _f_pct (
    Pico_Rel_Rect r,
    SDL_FRect base,
    const Pico_Abs_Dim* ratio
) {
    SDL_FDim d = _f_rat(r.w*base.w, r.h*base.h, ratio);
    return (SDL_FRect) {
        base.x + r.x*base.w - r.anchor.x*d.w,
        base.y + r.y*base.h - r.anchor.y*d.h,
        d.w, d.h,
    };
}

///////////////////////////////////////////////////////////////////////////////
// _sdl_*: rel -> float (logical coords)
///////////////////////////////////////////////////////////////////////////////

static SDL_FDim _sdl_dim (
    Pico_Rel_Dim*        dim,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FDim ret;
    switch (dim->mode) {
        case '!':
            ret = _f_rat(dim->w, dim->h, ratio);
            if (dim->w == 0) dim->w = ret.w;
            if (dim->h == 0) dim->h = ret.h;
            break;
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            ret = _f_rat(dim->w * d.w, dim->h * d.h, ratio);
            if (dim->w == 0) dim->w = ret.w / d.w;
            if (dim->h == 0) dim->h = ret.h / d.h;
            break;
        }
        case '#':
            ret = _f_rat(dim->w * G.layer->scene.tile.w, dim->h * G.layer->scene.tile.h, ratio);
            if (dim->w == 0) dim->w = ret.w / G.layer->scene.tile.w;
            if (dim->h == 0) dim->h = ret.h / G.layer->scene.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FPoint _sdl_pos (Pico_Rel_Pos pos, const Pico_Abs_Rect* base) {
    SDL_FPoint p;
    if (base == NULL) {
        p = (SDL_FPoint) { 0, 0 };
    } else {
        p = (SDL_FPoint) { base->x, base->y };
    }
    SDL_FPoint ret;
    switch (pos.mode) {
        case '!':
            ret = (SDL_FPoint) {
                p.x + pos.x - pos.anchor.x,
                p.y + pos.y - pos.anchor.y,
            };
            break;
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            ret = (SDL_FPoint) {
                p.x + pos.x*d.w - pos.anchor.x,
                p.y + pos.y*d.h - pos.anchor.y,
            };
            break;
        }
        case '#':
            ret = (SDL_FPoint) {
                p.x + (pos.x - 1 + pos.anchor.x)*G.layer->scene.tile.w,
                p.y + (pos.y - 1 + pos.anchor.y)*G.layer->scene.tile.h,
            };
            break;
        default:
            assert(0 && "invalid mode");
    }
    return ret;
}

static SDL_FRect _sdl_rect (
    Pico_Rel_Rect        rect,
    const Pico_Abs_Rect* base,
    const Pico_Abs_Dim*  ratio
) {
    SDL_FPoint p;
    if (base == NULL) {
        p = (SDL_FPoint) { 0, 0 };
    } else {
        p = (SDL_FPoint) { base->x, base->y };
    }
    SDL_FRect ret;
    switch (rect.mode) {
        case '!': {
            SDL_FDim d = _f_rat(rect.w, rect.h, ratio);
            ret = (SDL_FRect) {
                p.x + rect.x - rect.anchor.x*d.w,
                p.y + rect.y - rect.anchor.y*d.h,
                d.w, d.h
            };
            break;
        }
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            ret = _f_pct(rect, (SDL_FRect){ p.x, p.y, d.w, d.h }, ratio);
            break;
        }
        case '#': {
            SDL_FDim d = _f_rat (
                rect.w * G.layer->scene.tile.w,
                rect.h * G.layer->scene.tile.h,
                ratio
            );
            ret = (SDL_FRect) {
                p.x + (rect.x - 1 + rect.anchor.x)*G.layer->scene.tile.w - rect.anchor.x*d.w,
                p.y + (rect.y - 1 + rect.anchor.y)*G.layer->scene.tile.h - rect.anchor.y*d.h,
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

static void _rel_dim (
    SDL_FDim flt,
    Pico_Rel_Dim* to,
    const Pico_Abs_Rect* base
) {
    switch (to->mode) {
        case '!':
            to->w = flt.w;
            to->h = flt.h;
            break;
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            to->w = flt.w / d.w;
            to->h = flt.h / d.h;
            break;
        }
        case '#':
            to->w = flt.w / G.layer->scene.tile.w;
            to->h = flt.h / G.layer->scene.tile.h;
            break;
        default:
            assert(0 && "invalid mode");
    }
}

static void _rel_pos (
    SDL_FPoint flt,
    Pico_Rel_Pos* to,
    const Pico_Abs_Rect* base
) {
    SDL_FPoint p;
    if (base == NULL) {
        p = (SDL_FPoint) { 0, 0 };
    } else {
        p = (SDL_FPoint) { base->x, base->y };
    }
    switch (to->mode) {
        case '!':
            to->x = flt.x - p.x + to->anchor.x;
            to->y = flt.y - p.y + to->anchor.y;
            break;
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            to->x = (flt.x - p.x + to->anchor.x) / d.w;
            to->y = (flt.y - p.y + to->anchor.y) / d.h;
            break;
        }
        case '#':
            to->x = (flt.x - p.x) / G.layer->scene.tile.w + 1 - to->anchor.x;
            to->y = (flt.y - p.y) / G.layer->scene.tile.h + 1 - to->anchor.y;
            break;
        default:
            assert(0 && "invalid mode");
    }
}

static void _rel_rect (
    SDL_FRect flt,
    Pico_Rel_Rect* to,
    const Pico_Abs_Rect* base
) {
    SDL_FPoint p;
    if (base == NULL) {
        p = (SDL_FPoint) { 0, 0 };
    } else {
        p = (SDL_FPoint) { base->x, base->y };
    }
    switch (to->mode) {
        case '!':
            to->w = flt.w;
            to->h = flt.h;
            to->x = flt.x - p.x + to->anchor.x * to->w;
            to->y = flt.y - p.y + to->anchor.y * to->h;
            break;
        case '%': {
            SDL_FDim d;
            if (base == NULL) {
                d = (SDL_FDim) { G.layer->scene.dim.w, G.layer->scene.dim.h };
            } else {
                d = (SDL_FDim) { base->w, base->h };
            }
            to->w = flt.w / d.w;
            to->h = flt.h / d.h;
            to->x = (flt.x - p.x + to->anchor.x * flt.w) / d.w;
            to->y = (flt.y - p.y + to->anchor.y * flt.h) / d.h;
            break;
        }
        case '#':
            to->w = flt.w / (float)G.layer->scene.tile.w;
            to->h = flt.h / (float)G.layer->scene.tile.h;
            to->x = (flt.x - p.x) / G.layer->scene.tile.w + 1
                    - to->anchor.x + to->anchor.x * to->w;
            to->y = (flt.y - p.y) / G.layer->scene.tile.h + 1
                    - to->anchor.y + to->anchor.y * to->h;
            break;
        default:
            assert(0 && "invalid mode");
    }
}

///////////////////////////////////////////////////////////////////////////////
// _rnd_*: float -> abs (rounding)
///////////////////////////////////////////////////////////////////////////////

// use floorf(. + 0.5f) toward +inf: makes NW/C behave the same

static Pico_Abs_Dim _rnd_dim (SDL_FDim f) {
    return (Pico_Abs_Dim) {
        floorf(f.w+0.5f), floorf(f.h+0.5f)
    };
}

static Pico_Abs_Pos _rnd_pos (SDL_FPoint f) {
    return (Pico_Abs_Pos) {
        floorf(f.x+0.5f), floorf(f.y+0.5f)
    };
}

static Pico_Abs_Rect _rnd_rect (SDL_FRect f) {
    return (Pico_Abs_Rect) {
        floorf(f.x+0.5f), floorf(f.y+0.5f),
        floorf(f.w+0.5f), floorf(f.h+0.5f)
    };
}
