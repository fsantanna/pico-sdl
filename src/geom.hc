///////////////////////////////////////////////////////////////////////////////
// CV: named-layer projection (cv_pos_to / cv_pos_from)
///////////////////////////////////////////////////////////////////////////////

// Returns 1 if A is a strict descendant of B (via hier.up chain).
// Returns 0 when A == B or when A is not below B.
static int _cv_is_descendant_of (Pico_Layer* A, Pico_Layer* B) {
    Pico_Layer* M = A;
    while (M->hier.up != NULL) {
        M = _pico_layer_name(M->hier.up);
        if (M == B) {
            return 1;
        }
    }
    return 0;
}

void pico_cv_dim_from (
    const char* layer, const Pico_Rel_Dim* fr, Pico_Rel_Dim* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // S is descendant of cur: swap, delegate to _to with cur=S.
    if (_cv_is_descendant_of(S, L)) {
        G.layer = S;
        pico_cv_dim_to(L->name, fr, to);
        G.layer = L;
        return;
    }

    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur, ancestor, or descendant of cur"
        );
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = S;

    Pico_Abs_Rect S_base = {0, 0, S->scene.dim.w, S->scene.dim.h};
    Pico_Rel_Dim fr_copy = *fr;
    SDL_FDim d = _sdl_dim(&fr_copy, &S_base, NULL);

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

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    _rel_dim(d, to, &L_base);
}

void pico_cv_dim_to (
    const char* layer, const Pico_Rel_Dim* fr, Pico_Rel_Dim* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // T is descendant of cur: swap, delegate to _from with cur=T.
    if (_cv_is_descendant_of(T, L)) {
        G.layer = T;
        pico_cv_dim_from(L->name, fr, to);
        G.layer = L;
        return;
    }

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Rel_Dim fr_copy = *fr;
    SDL_FDim d = _sdl_dim(&fr_copy, &L_base, NULL);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur, ancestor, or descendant of cur"
        );
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &L_base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        d.w = d.w * dst.w / src.w;
        d.h = d.h * dst.h / src.h;
        L = P;
        L_base = Pb;
    }

    _rel_dim(d, to, &L_base);
}

void pico_cv_pos_from (
    const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // S is descendant of cur: swap, delegate to _to with cur=S.
    if (_cv_is_descendant_of(S, L)) {
        G.layer = S;
        pico_cv_pos_to(L->name, fr, to);
        G.layer = L;
        return;
    }

    // chain: chain[0] = cur, chain[n-1] = S
    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur, ancestor, or descendant of cur"
        );
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = S;

    Pico_Abs_Rect S_base = {0, 0, S->scene.dim.w, S->scene.dim.h};
    SDL_FPoint p = _sdl_pos(fr, &S_base);

    // walk S -> cur, applying inverse of each step
    for (int i = n-2; i >= 0; i--) {
        Pico_Layer* C = chain[i];      // child
        Pico_Layer* P = chain[i+1];    // parent
        Pico_Abs_Rect Cb = {0, 0, C->scene.dim.w, C->scene.dim.h};
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect dst = _sdl_rect(&C->scene.dst, &Pb, NULL);
        SDL_FRect src = _sdl_rect(&C->scene.src, &Cb, NULL);
        float rx = (p.x - dst.x) / dst.w;
        float ry = (p.y - dst.y) / dst.h;
        p.x = src.x + rx * src.w;
        p.y = src.y + ry * src.h;
    }

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    _rel_pos(p, to, &L_base);
}

void pico_cv_pos_to (
    const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // T is descendant of cur: swap, delegate to _from with cur=T.
    if (_cv_is_descendant_of(T, L)) {
        G.layer = T;
        pico_cv_pos_from(L->name, fr, to);
        G.layer = L;
        return;
    }

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    SDL_FPoint p = _sdl_pos(fr, &L_base);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur, ancestor, or descendant of cur"
        );
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &L_base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        float rx = (p.x - src.x) / src.w;
        float ry = (p.y - src.y) / src.h;
        p.x = dst.x + rx * dst.w;
        p.y = dst.y + ry * dst.h;
        L = P;
        L_base = Pb;
    }

    _rel_pos(p, to, &L_base);
}

void pico_cv_rect_from (
    const char* layer, const Pico_Rel_Rect* fr, Pico_Rel_Rect* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // S is descendant of cur: swap, delegate to _to with cur=S.
    if (_cv_is_descendant_of(S, L)) {
        G.layer = S;
        pico_cv_rect_to(L->name, fr, to);
        G.layer = L;
        return;
    }

    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur, ancestor, or descendant of cur"
        );
        M = _pico_layer_name(M->hier.up);
    }
    chain[n++] = S;

    Pico_Abs_Rect S_base = {0, 0, S->scene.dim.w, S->scene.dim.h};
    SDL_FRect r = _sdl_rect(fr, &S_base, NULL);

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

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    _rel_rect(r, to, &L_base);
}

void pico_cv_rect_to (
    const char* layer, const Pico_Rel_Rect* fr, Pico_Rel_Rect* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // T is descendant of cur: swap, delegate to _from with cur=T.
    if (_cv_is_descendant_of(T, L)) {
        G.layer = T;
        pico_cv_rect_from(L->name, fr, to);
        G.layer = L;
        return;
    }

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    SDL_FRect r = _sdl_rect(fr, &L_base, NULL);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur, ancestor, or descendant of cur"
        );
        Pico_Layer* P = _pico_layer_name(L->hier.up);
        Pico_Abs_Rect Pb = {0, 0, P->scene.dim.w, P->scene.dim.h};
        SDL_FRect src = _sdl_rect(&L->scene.src, &L_base, NULL);
        SDL_FRect dst = _sdl_rect(&L->scene.dst, &Pb, NULL);
        float sx = dst.w / src.w;
        float sy = dst.h / src.h;
        r.x = dst.x + (r.x - src.x) * sx;
        r.y = dst.y + (r.y - src.y) * sy;
        r.w = r.w * sx;
        r.h = r.h * sy;
        L = P;
        L_base = Pb;
    }

    _rel_rect(r, to, &L_base);
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

static Pico_Rel_Pos _vs_pos (const char* layer, const Pico_Rel_Pos* p) {
    if (layer == NULL) return *p;
    Pico_Layer* old = G.layer;
    G.layer = _pico_layer_name(layer);
    if (G.layer == old) {
        return *p;
    }
    Pico_Rel_Pos out = {'!', {0, 0}, PICO_ANCHOR_NW};
    pico_cv_pos_to(old->name, p, &out);
    G.layer = old;
    return out;
}

static Pico_Rel_Rect _vs_rect (const char* layer, const Pico_Rel_Rect* r) {
    if (layer == NULL) {
        return (r == NULL) ? G.layer->scene.dst : *r;
    }

    Pico_Layer* old = G.layer;
    G.layer = _pico_layer_name(layer);
    if (G.layer == old) {
        return (r == NULL) ? old->scene.dst : *r;
    }

    Pico_Rel_Rect out;
    if (r == NULL) {
        out = G.layer->scene.dst;
    } else {
        out = (Pico_Rel_Rect){'!', {0, 0, 0, 0}, PICO_ANCHOR_NW};
        pico_cv_rect_to(old->name, r, &out);
    }
    G.layer = old;
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
