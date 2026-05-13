///////////////////////////////////////////////////////////////////////////////
// CV: named-layer projection (cv_pos_to / cv_pos_from)
///////////////////////////////////////////////////////////////////////////////

// Returns the root layer of L (top of the hier.up chain).
// For attached layers this is window; detached subtrees have their own root.
static Pico_Layer* _root_of (Pico_Layer* L) {
    while (L->hier.up != NULL) {
        L = _pico_layer_name(L->hier.up);
    }
    return L;
}

// Root-mediated walk: fromL → root → toL.
// fromL and toL must share a root (typically window).
static void _cv_walk_pos  (Pico_Rel_Pos*  to, const Pico_Rel_Pos*  fr,
                           Pico_Layer* fromL, Pico_Layer* toL);
static void _cv_walk_rect (Pico_Rel_Rect* to, const Pico_Rel_Rect* fr,
                           Pico_Layer* fromL, Pico_Layer* toL);
static void _cv_walk_dim  (Pico_Rel_Dim*  to, const Pico_Rel_Dim*  fr,
                           Pico_Layer* fromL, Pico_Layer* toL);

// Public unified cv: project fr (in L_fr) into to (in L_to).
// L_fr / L_to == NULL means cur. Layers must share a root.
void pico_cv_dim (
    const char* L_to, Pico_Rel_Dim* to,
    const char* L_fr, const Pico_Rel_Dim* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);
    _cv_walk_dim(to, fr, S, T);
}

void pico_cv_pos (
    const char* L_to, Pico_Rel_Pos* to,
    const char* L_fr, const Pico_Rel_Pos* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);
    _cv_walk_pos(to, fr, S, T);
}

void pico_cv_rect (
    const char* L_to, Pico_Rel_Rect* to,
    const char* L_fr, const Pico_Rel_Rect* fr
) {
    _pico_guard();
    Pico_Layer* T = (L_to == NULL) ? G.layer : _pico_layer_name(L_to);
    Pico_Layer* S = (L_fr == NULL) ? G.layer : _pico_layer_name(L_fr);
    _cv_walk_rect(to, fr, S, T);
}

static void _cv_walk_pos (
    Pico_Rel_Pos* to, const Pico_Rel_Pos* fr,
    Pico_Layer* fromL, Pico_Layer* toL
) {
    Pico_Abs_Rect base = {0, 0, fromL->scene.dim.w, fromL->scene.dim.h};
    SDL_FPoint p = _sdl_pos(fr, &base);
    if (fromL == toL) {
        _rel_pos(p, to, &base);
        return;
    }
    // 1) walk fromL up to root, accumulating src->dst per step.
    Pico_Layer* L = fromL;
    while (L->hier.up != NULL) {
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        float rx = (p.x - src.x) / src.w;
        float ry = (p.y - src.y) / src.h;
        p.x = dst.x + rx * dst.w;
        p.y = dst.y + ry * dst.h;
        L = P;
        base = Pb;
    }
    // 2) build chain toL up to root; walk back applying inverse.
    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = toL;
    while (M->hier.up != NULL) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = M;
    pico_assert(M == L && "cv: layers must share a root");
    for (int i = n-2; i >= 0; i--) {
        Pico_Layer* C = chain[i];
        Pico_Layer* P = chain[i+1];
        Pico_Abs_Rect Cb = {0, 0, C->scene.dim.w, C->scene.dim.h};
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect dst = _sdl_rect(&C->scene.dst, &Pb, NULL);
        SDL_FRect src = _sdl_rect(&C->scene.src, &Cb, NULL);
        float rx = (p.x - dst.x) / dst.w;
        float ry = (p.y - dst.y) / dst.h;
        p.x = src.x + rx * src.w;
        p.y = src.y + ry * src.h;
    }
    Pico_Abs_Rect toBase = {0, 0, toL->scene.dim.w, toL->scene.dim.h};
    _rel_pos(p, to, &toBase);
}

static void _cv_walk_rect (
    Pico_Rel_Rect* to, const Pico_Rel_Rect* fr,
    Pico_Layer* fromL, Pico_Layer* toL
) {
    Pico_Abs_Rect base = {0, 0, fromL->scene.dim.w, fromL->scene.dim.h};
    SDL_FRect r = _sdl_rect(fr, &base, NULL);
    if (fromL == toL) {
        _rel_rect(r, to, &base);
        return;
    }
    Pico_Layer* L = fromL;
    while (L->hier.up != NULL) {
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        float sx = dst.w / src.w;
        float sy = dst.h / src.h;
        r.x = dst.x + (r.x - src.x) * sx;
        r.y = dst.y + (r.y - src.y) * sy;
        r.w = r.w * sx;
        r.h = r.h * sy;
        L = P;
        base = Pb;
    }
    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = toL;
    while (M->hier.up != NULL) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = M;
    pico_assert(M == L && "cv: layers must share a root");
    for (int i = n-2; i >= 0; i--) {
        Pico_Layer* C = chain[i];
        Pico_Layer* P = chain[i+1];
        Pico_Abs_Rect Cb = {0, 0, C->scene.dim.w, C->scene.dim.h};
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect dst = _sdl_rect(&C->scene.dst, &Pb, NULL);
        SDL_FRect src = _sdl_rect(&C->scene.src, &Cb, NULL);
        float sx = src.w / dst.w;
        float sy = src.h / dst.h;
        r.x = src.x + (r.x - dst.x) * sx;
        r.y = src.y + (r.y - dst.y) * sy;
        r.w = r.w * sx;
        r.h = r.h * sy;
    }
    Pico_Abs_Rect toBase = {0, 0, toL->scene.dim.w, toL->scene.dim.h};
    _rel_rect(r, to, &toBase);
}

static void _cv_walk_dim (
    Pico_Rel_Dim* to, const Pico_Rel_Dim* fr,
    Pico_Layer* fromL, Pico_Layer* toL
) {
    Pico_Abs_Rect base = {0, 0, fromL->scene.dim.w, fromL->scene.dim.h};
    Pico_Rel_Dim fr_copy = *fr;
    SDL_FDim d = _sdl_dim(&fr_copy, &base, NULL);
    if (fromL == toL) {
        _rel_dim(d, to, &base);
        return;
    }
    Pico_Layer* L = fromL;
    while (L->hier.up != NULL) {
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        d.w = d.w * dst.w / src.w;
        d.h = d.h * dst.h / src.h;
        L = P;
        base = Pb;
    }
    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = toL;
    while (M->hier.up != NULL) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = M;
    pico_assert(M == L && "cv: layers must share a root");
    for (int i = n-2; i >= 0; i--) {
        Pico_Layer* C = chain[i];
        Pico_Layer* P = chain[i+1];
        Pico_Abs_Rect Cb = {0, 0, C->scene.dim.w, C->scene.dim.h};
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect dst = _sdl_rect(&C->scene.dst, &Pb, NULL);
        SDL_FRect src = _sdl_rect(&C->scene.src, &Cb, NULL);
        d.w = d.w * src.w / dst.w;
        d.h = d.h * src.h / dst.h;
    }
    Pico_Abs_Rect toBase = {0, 0, toL->scene.dim.w, toL->scene.dim.h};
    _rel_dim(d, to, &toBase);
}

///////////////////////////////////////////////////////////////////////////////
// IN: compose child onto parent, return flat rel
///////////////////////////////////////////////////////////////////////////////

Pico_Rel_Rect pico_in_rect (const Pico_Rel_Rect* out, const Pico_Rel_Rect* in) {
    Pico_Abs_Rect out_abs = _rnd_rect(_sdl_rect(out, NULL, NULL));
    Pico_Abs_Rect in_abs  = _rnd_rect(_sdl_rect(in, &out_abs, NULL));
    Pico_Rel_Rect ret = { .mode = in->mode, .anchor = in->anchor };
    _rel_rect((SDL_FRect){in_abs.x, in_abs.y, in_abs.w, in_abs.h}, &ret, NULL);
    return ret;
}

Pico_Rel_Pos pico_in_pos (const Pico_Rel_Rect* out, const Pico_Rel_Pos* in) {
    Pico_Abs_Rect out_abs = _rnd_rect(_sdl_rect(out, NULL, NULL));
    Pico_Abs_Pos  in_abs  = _rnd_pos(_sdl_pos(in, &out_abs));
    Pico_Rel_Pos ret = { .mode = in->mode, .anchor = in->anchor };
    _rel_pos((SDL_FPoint){in_abs.x, in_abs.y}, &ret, NULL);
    return ret;
}

Pico_Rel_Dim pico_in_dim (const Pico_Rel_Rect* out, const Pico_Rel_Dim* in) {
    Pico_Abs_Rect out_abs = _rnd_rect(_sdl_rect(out, NULL, NULL));
    Pico_Rel_Dim  in_copy = *in;
    Pico_Abs_Dim  in_abs  = _rnd_dim(_sdl_dim(&in_copy, &out_abs, NULL));
    Pico_Rel_Dim ret = { .mode = in->mode };
    _rel_dim((SDL_FDim){in_abs.w, in_abs.h}, &ret, NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// VS
///////////////////////////////////////////////////////////////////////////////

// Project p (in `layer`'s frame; NULL == cur) up to its root.
static Pico_Rel_Pos _vs_pos (const char* layer, const Pico_Rel_Pos* p) {
    const char* L_name = (layer == NULL) ? G.layer->name : layer;
    Pico_Layer* L = _pico_layer_name(L_name);
    Pico_Layer* root = _root_of(L);
    if (L == root) {
        return *p;
    }
    Pico_Rel_Pos out = {'!', {0, 0}, PICO_ANCHOR_NW};
    pico_cv_pos(root->name, &out, L_name, p);
    return out;
}

// Project r (in `layer`; NULL r == layer's scene.dst in parent frame)
// up to its root.
static Pico_Rel_Rect _vs_rect (const char* layer, const Pico_Rel_Rect* r) {
    const char* L_name = (layer == NULL) ? G.layer->name : layer;
    Pico_Layer* L = _pico_layer_name(L_name);
    Pico_Layer* root = _root_of(L);

    if (r == NULL) {
        // L's bounds = scene.dst in L's parent frame.
        Pico_Rel_Rect dst = L->scene.dst;
        if (L == root) {
            return dst;
        }
        const char* parent_name = L->hier.up;
        if (_pico_layer_name(parent_name) == root) {
            return dst;
        }
        Pico_Rel_Rect out = {'!', {0, 0, 0, 0}, PICO_ANCHOR_NW};
        pico_cv_rect(root->name, &out, parent_name, &dst);
        return out;
    }
    if (L == root) {
        return *r;
    }
    Pico_Rel_Rect out = {'!', {0, 0, 0, 0}, PICO_ANCHOR_NW};
    pico_cv_rect(root->name, &out, L_name, r);
    return out;
}

int pico_vs_pos_pos (
    const char* L1, Pico_Rel_Pos* p1,
    const char* L2, Pico_Rel_Pos* p2
) {
    _pico_guard();
    assert(p1 != NULL && p2 != NULL);
    Pico_Rel_Pos p1_cur = _vs_pos(L1, p1);
    Pico_Rel_Pos p2_cur = _vs_pos(L2, p2);
    Pico_Abs_Pos i1 = _rnd_pos(_sdl_pos(&p1_cur, NULL));
    Pico_Abs_Pos i2 = _rnd_pos(_sdl_pos(&p2_cur, NULL));
    return i1.x == i2.x && i1.y == i2.y;
}

int pico_vs_pos_rect (
    const char* L1, Pico_Rel_Pos*  p1,
    const char* L2, Pico_Rel_Rect* r2
) {
    _pico_guard();
    assert(p1 != NULL);
    Pico_Rel_Pos  p_cur = _vs_pos (L1, p1);
    Pico_Rel_Rect r_cur = _vs_rect(L2, r2);
    Pico_Abs_Pos  pi = _rnd_pos (_sdl_pos (&p_cur, NULL));
    Pico_Abs_Rect ri = _rnd_rect(_sdl_rect(&r_cur, NULL, NULL));
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
    Pico_Rel_Rect r1_cur = _vs_rect(L1, r1);
    Pico_Rel_Rect r2_cur = _vs_rect(L2, r2);
    Pico_Abs_Rect i1 = _rnd_rect(_sdl_rect(&r1_cur, NULL, NULL));
    Pico_Abs_Rect i2 = _rnd_rect(_sdl_rect(&r2_cur, NULL, NULL));
    return SDL_HasIntersection(&i1, &i2);
}
