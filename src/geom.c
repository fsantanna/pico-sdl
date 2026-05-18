#include <math.h>
#include <assert.h>

#include "_pico.h"

// forward decls for static helpers defined at the bottom of this file
static SDL_FDim      _pico_raw_dim  (Pico_Rel_Dim* dim, const Pico_Abs_Rect* base, const Pico_Abs_Dim* ratio);
static SDL_FPoint    _pico_raw_pos  (Pico_Rel_Pos pos, const Pico_Abs_Rect* base);
static SDL_FRect     _pico_raw_rect (Pico_Rel_Rect rect, const Pico_Abs_Rect* base, const Pico_Abs_Dim* ratio);
static void          _pico_rel_dim  (SDL_FDim flt,   Pico_Rel_Dim*  to, const Pico_Abs_Rect* base);
static void          _pico_rel_pos  (SDL_FPoint flt, Pico_Rel_Pos*  to, const Pico_Abs_Rect* base);
static void          _pico_rel_rect (SDL_FRect flt,  Pico_Rel_Rect* to, const Pico_Abs_Rect* base);
static Pico_Abs_Dim  _pico_rnd_dim  (SDL_FDim   f);
static Pico_Abs_Pos  _pico_rnd_pos  (SDL_FPoint f);
static Pico_Abs_Rect _pico_rnd_rect (SDL_FRect  f);

///////////////////////////////////////////////////////////////////////////////
// CV: named-layer projection (cv_pos_to / cv_pos_from)
///////////////////////////////////////////////////////////////////////////////

// Root-mediated walk: fromL → root → toL.
// fromL and toL must share a root (typically window).

// up: walk d (in L's frame) to root; *out is d in root's frame.
static Pico_Layer* _dim_root_to (Pico_Layer* L, SDL_FDim d, SDL_FDim* out) {
    if (L->hier.up == NULL) {
        *out = d;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    d.w = d.w * dst.w / src.w;
    d.h = d.h * dst.h / src.h;
    return _dim_root_to(P, d, out);
}

// down: project d (in L's root's frame) into L's frame.
static Pico_Layer* _dim_root_fr (Pico_Layer* L, SDL_FDim d, SDL_FDim* out) {
    if (L->hier.up == NULL) {
        *out = d;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Layer* R = _dim_root_fr(P, d, &d);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    d.w = d.w * src.w / dst.w;
    d.h = d.h * src.h / dst.h;
    *out = d;
    return R;
}

// up: walk p (in L's frame) to root; *out is p in root's frame.
static Pico_Layer* _pos_root_to (Pico_Layer* L, SDL_FPoint p, SDL_FPoint* out) {
    if (L->hier.up == NULL) {
        *out = p;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    float rx = (p.x - src.x) / src.w;
    float ry = (p.y - src.y) / src.h;
    p.x = dst.x + rx * dst.w;
    p.y = dst.y + ry * dst.h;
    return _pos_root_to(P, p, out);
}

// down: project p (in L's root's frame) into L's frame.
static Pico_Layer* _pos_root_fr (Pico_Layer* L, SDL_FPoint p, SDL_FPoint* out) {
    if (L->hier.up == NULL) {
        *out = p;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Layer* R = _pos_root_fr(P, p, &p);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    float rx = (p.x - dst.x) / dst.w;
    float ry = (p.y - dst.y) / dst.h;
    p.x = src.x + rx * src.w;
    p.y = src.y + ry * src.h;
    *out = p;
    return R;
}

// up: walk r (in L's frame) to root; *out is r in root's frame.
static Pico_Layer* _rect_root_to (Pico_Layer* L, SDL_FRect r, SDL_FRect* out) {
    if (L->hier.up == NULL) {
        *out = r;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    float sx = dst.w / src.w;
    float sy = dst.h / src.h;
    r.x = dst.x + (r.x - src.x) * sx;
    r.y = dst.y + (r.y - src.y) * sy;
    r.w = r.w * sx;
    r.h = r.h * sy;
    return _rect_root_to(P, r, out);
}

// down: project r (in L's root's frame) into L's frame.
static Pico_Layer* _rect_root_fr (Pico_Layer* L, SDL_FRect r, SDL_FRect* out) {
    if (L->hier.up == NULL) {
        *out = r;
        return L;
    }
    Pico_Layer* P = _pico_layer_name(L->hier.up);
    Pico_Layer* R = _rect_root_fr(P, r, &r);
    Pico_Abs_Rect Lb = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
    SDL_FRect dst = _pico_raw_rect(L->scene.dst, &Pb, NULL);
    SDL_FRect src = _pico_raw_rect(L->scene.src, &Lb, NULL);
    float sx = src.w / dst.w;
    float sy = src.h / dst.h;
    r.x = src.x + (r.x - dst.x) * sx;
    r.y = src.y + (r.y - dst.y) * sy;
    r.w = r.w * sx;
    r.h = r.h * sy;
    *out = r;
    return R;
}

// Public unified cv: project fr (in L_fr) into to (in L_to).
// L_fr / L_to == NULL means cur. Layers must share a root.
void pico_cv_dim (
    const char* L_to, Pico_Rel_Dim* to,
    const char* L_fr, const Pico_Rel_Dim* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);

    SDL_FDim d1,d2,d3;
    Pico_Rel_Dim fr_copy = *fr;
    d1 = _pico_raw_dim(&fr_copy, &(Pico_Abs_Rect){0, 0, S->scene.dim.w, S->scene.dim.h}, NULL);

    Pico_Layer* R_fr = _dim_root_to(S, d1, &d2);
    Pico_Layer* R_to = _dim_root_fr(T, d2, &d3);
    pico_assert(R_fr == R_to && "cv: layers must share a root");

    _pico_rel_dim(d3, to, &(Pico_Abs_Rect){0, 0, T->scene.dim.w, T->scene.dim.h});
}

void pico_cv_pos (
    const char* L_to, Pico_Rel_Pos* to,
    const char* L_fr, const Pico_Rel_Pos* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);

    SDL_FPoint p1,p2,p3;
    p1 = _pico_raw_pos(*fr, &(Pico_Abs_Rect){0, 0, S->scene.dim.w, S->scene.dim.h});

    Pico_Layer* R_fr = _pos_root_to(S, p1, &p2);
    Pico_Layer* R_to = _pos_root_fr(T, p2, &p3);
    pico_assert(R_fr == R_to && "cv: layers must share a root");

    _pico_rel_pos(p3, to, &(Pico_Abs_Rect){0, 0, T->scene.dim.w, T->scene.dim.h});
}

void pico_cv_rect (
    const char* L_to, Pico_Rel_Rect* to,
    const char* L_fr, const Pico_Rel_Rect* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);

    SDL_FRect r1,r2,r3;
    r1 = _pico_raw_rect(*fr, &(Pico_Abs_Rect){0, 0, S->scene.dim.w, S->scene.dim.h}, NULL);

    Pico_Layer* R_fr = _rect_root_to(S, r1, &r2);
    Pico_Layer* R_to = _rect_root_fr(T, r2, &r3);
    pico_assert(R_fr == R_to && "cv: layers must share a root");

    _pico_rel_rect(r3, to, &(Pico_Abs_Rect){0, 0, T->scene.dim.w, T->scene.dim.h});
}

///////////////////////////////////////////////////////////////////////////////
// IN: compose child onto parent, return flat rel
///////////////////////////////////////////////////////////////////////////////

Pico_Rel_Rect pico_in_rect (Pico_Rel_Rect out, Pico_Rel_Rect in) {
    Pico_Abs_Rect out_abs = _pico_abs_rect(out, NULL, NULL);
    Pico_Abs_Rect in_abs  = _pico_abs_rect(in, &out_abs, NULL);
    Pico_Rel_Rect ret = { .mode = in.mode, .anchor = in.anchor };
    _pico_rel_rect((SDL_FRect){in_abs.x, in_abs.y, in_abs.w, in_abs.h}, &ret, NULL);
    return ret;
}

Pico_Rel_Pos pico_in_pos (Pico_Rel_Rect out, Pico_Rel_Pos in) {
    Pico_Abs_Rect out_abs = _pico_abs_rect(out, NULL, NULL);
    Pico_Abs_Pos  in_abs  = _pico_abs_pos(in, &out_abs);
    Pico_Rel_Pos ret = { .mode = in.mode, .anchor = in.anchor };
    _pico_rel_pos((SDL_FPoint){in_abs.x, in_abs.y}, &ret, NULL);
    return ret;
}

Pico_Rel_Dim pico_in_dim (Pico_Rel_Rect out, Pico_Rel_Dim in) {
    Pico_Abs_Rect out_abs = _pico_abs_rect(out, NULL, NULL);
    Pico_Abs_Dim  in_abs  = _pico_abs_dim(&in, &out_abs, NULL);
    Pico_Rel_Dim ret = { .mode = in.mode };
    _pico_rel_dim((SDL_FDim){in_abs.w, in_abs.h}, &ret, NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// VS
///////////////////////////////////////////////////////////////////////////////

// Returns the root layer of L (top of the hier.up chain).
// For attached layers this is window; detached subtrees have their own root.
static Pico_Layer* _root_of (Pico_Layer* L) {
    while (L->hier.up != NULL) {
        L = _pico_layer_name(L->hier.up);
    }
    return L;
}

// Project p (in `layer`'s frame; NULL == cur) up to its root, as '!'.
static Pico_Rel_Pos _root_pos (const char* layer, const Pico_Rel_Pos* p) {
    Pico_Layer* L = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* R = _root_of(L);
    Pico_Rel_Pos ret = {'!', {}, PICO_ANCHOR_C};
    pico_cv_pos(R->name, &ret, L->name, p);
    return ret;
}

// Project r (in `layer`; NULL r == layer's scene.dst in parent frame)
// up to its root, as '!'.
static Pico_Rel_Rect _root_rect (const char* layer, const Pico_Rel_Rect* r) {
    Pico_Layer* L = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* R = _root_of(L);

    Pico_Rel_Rect ret = {'!', {}, PICO_ANCHOR_C};

    if (r != NULL) {
        pico_cv_rect(R->name, &ret, L->name, r);
    } else if (L == R) {
        // L is R: no parent placement; use L's own interior.
        ret.x = 0;
        ret.y = 0;
        ret.w = L->scene.dim.w;
        ret.h = L->scene.dim.h;
    } else {
        pico_cv_rect(R->name, &ret, L->hier.up, &L->scene.dst);
    }

    return ret;
}

int pico_vs_pos_pos (
    const char* L1, Pico_Rel_Pos* p1,
    const char* L2, Pico_Rel_Pos* p2
) {
    _pico_guard();
    assert(p1 != NULL && p2 != NULL);
    Pico_Rel_Pos p1_cur = _root_pos(L1, p1);
    Pico_Rel_Pos p2_cur = _root_pos(L2, p2);
    Pico_Abs_Pos i1 = _pico_abs_pos(p1_cur, NULL);
    Pico_Abs_Pos i2 = _pico_abs_pos(p2_cur, NULL);
    return i1.x == i2.x && i1.y == i2.y;
}

int pico_vs_pos_rect (
    const char* L1, Pico_Rel_Pos*  p1,
    const char* L2, Pico_Rel_Rect* r2
) {
    _pico_guard();
    assert(p1 != NULL);
    Pico_Rel_Pos  p_cur = _root_pos (L1, p1);
    Pico_Rel_Rect r_cur = _root_rect(L2, r2);
    Pico_Abs_Pos  pi = _pico_rnd_pos (_pico_raw_pos(p_cur, NULL));
    Pico_Abs_Rect ri = _pico_abs_rect(r_cur, NULL, NULL);
    return SDL_PointInRect(&pi, &ri);
}

int pico_vs_rect_pos (
    const char* L1, Pico_Rel_Rect* r1,
    const char* L2, Pico_Rel_Pos*  p2
) {
    return pico_vs_pos_rect(L2, p2, L1, r1);
}

int pico_vs_rect_rect (
    const char* L1, Pico_Rel_Rect* r1,
    const char* L2, Pico_Rel_Rect* r2
) {
    _pico_guard();
    Pico_Rel_Rect r1_cur = _root_rect(L1, r1);
    Pico_Rel_Rect r2_cur = _root_rect(L2, r2);
    Pico_Abs_Rect i1 = _pico_abs_rect(r1_cur, NULL, NULL);
    Pico_Abs_Rect i2 = _pico_abs_rect(r2_cur, NULL, NULL);
    return SDL_HasIntersection(&i1, &i2);
}



///////////////////////////////////////////////////////////////////////////////
// _f_rat: fill missing w/h from aspect ratio
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

///////////////////////////////////////////////////////////////////////////////
// _raw_*: rel -> float (logical coords)
///////////////////////////////////////////////////////////////////////////////

static SDL_FDim _pico_raw_dim (
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

static SDL_FPoint _pico_raw_pos (Pico_Rel_Pos pos, const Pico_Abs_Rect* base) {
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

static SDL_FRect _pico_raw_rect (
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
            SDL_FDim wh = _f_rat(rect.w*d.w, rect.h*d.h, ratio);
            ret = (SDL_FRect) {
                p.x + rect.x*d.w - rect.anchor.x*wh.w,
                p.y + rect.y*d.h - rect.anchor.y*wh.h,
                wh.w, wh.h
            };
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

static void _pico_rel_dim (
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

static void _pico_rel_pos (
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

static void _pico_rel_rect (
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

static Pico_Abs_Dim _pico_rnd_dim (SDL_FDim f) {
    return (Pico_Abs_Dim) {
        floorf(f.w+0.5f), floorf(f.h+0.5f)
    };
}

static Pico_Abs_Pos _pico_rnd_pos (SDL_FPoint f) {
    return (Pico_Abs_Pos) {
        floorf(f.x+0.5f), floorf(f.y+0.5f)
    };
}

static Pico_Abs_Rect _pico_rnd_rect (SDL_FRect f) {
    return (Pico_Abs_Rect) {
        floorf(f.x+0.5f), floorf(f.y+0.5f),
        floorf(f.w+0.5f), floorf(f.h+0.5f)
    };
}

///////////////////////////////////////////////////////////////////////////////
// _pico_abs_*: rel -> abs (rounded)
///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Dim _pico_abs_dim (
    Pico_Rel_Dim* dim, const Pico_Abs_Rect* base, const Pico_Abs_Dim* ratio
) {
    return _pico_rnd_dim(_pico_raw_dim(dim, base, ratio));
}

Pico_Abs_Pos _pico_abs_pos (Pico_Rel_Pos pos, const Pico_Abs_Rect* base) {
    return _pico_rnd_pos(_pico_raw_pos(pos, base));
}

Pico_Abs_Rect _pico_abs_rect (
    Pico_Rel_Rect rect, const Pico_Abs_Rect* base, const Pico_Abs_Dim* ratio
) {
    return _pico_rnd_rect(_pico_raw_rect(rect, base, ratio));
}

///////////////////////////////////////////////////////////////////////////////
// _pico_mode_*: mode conversion (in.mode -> out.mode)
///////////////////////////////////////////////////////////////////////////////

void _pico_mode_rect (Pico_Rel_Rect in, Pico_Rel_Rect* out) {
    _pico_rel_rect(_pico_raw_rect(in, NULL, NULL), out, NULL);
}
