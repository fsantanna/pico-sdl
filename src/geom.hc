///////////////////////////////////////////////////////////////////////////////
// CV
///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Dim pico_cv_dim_rel_abs (Pico_Rel_Dim* dim, Pico_Abs_Rect* base) {
    SDL_FDim df = _sdl_dim(dim, base, NULL);
    return _abs_dim(&df);
}

void pico_cv_dim_abs_rel (
    const Pico_Abs_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    _rel_dim((SDL_FDim){fr->w, fr->h}, to, base);
}

void pico_cv_dim_rel_rel (
    Pico_Rel_Dim* fr, Pico_Rel_Dim* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Dim abs = pico_cv_dim_rel_abs(fr, base);
    pico_cv_dim_abs_rel(&abs, to, base);
}

Pico_Abs_Pos pico_cv_pos_rel_abs (const Pico_Rel_Pos* pos, Pico_Abs_Rect* base) {
    SDL_FPoint pf = _sdl_pos(pos, base);
    return (Pico_Abs_Pos) _abs_pos(&pf);
}

Pico_Abs_Rect pico_cv_rect_rel_abs (const Pico_Rel_Rect* rect, Pico_Abs_Rect* base) {
    SDL_FRect rf = _sdl_rect(rect, base, NULL);
    return (Pico_Abs_Rect) _abs_rect(&rf);
}

void pico_cv_pos_abs_rel (
    const Pico_Abs_Pos* fr, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    _rel_pos((SDL_FPoint){fr->x, fr->y}, to, base);
}

void pico_cv_pos_rel_rel (
    const Pico_Rel_Pos* fr, Pico_Rel_Pos* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Pos abs = pico_cv_pos_rel_abs(fr, base);
    pico_cv_pos_abs_rel(&abs, to, base);
}

void pico_cv_rect_abs_rel (
    const Pico_Abs_Rect* fr, Pico_Rel_Rect* to, Pico_Abs_Rect* base
) {
    _rel_rect((SDL_FRect){fr->x, fr->y, fr->w, fr->h}, to, base);
}

void pico_cv_rect_rel_rel (
    const Pico_Rel_Rect* fr, Pico_Rel_Rect* to, Pico_Abs_Rect* base
) {
    Pico_Abs_Rect abs = pico_cv_rect_rel_abs(fr, base);
    pico_cv_rect_abs_rel(&abs, to, base);
}

///////////////////////////////////////////////////////////////////////////////
// CV: named-layer projection (cv_pos_to / cv_pos_from)
///////////////////////////////////////////////////////////////////////////////

void pico_cv_pos_to (
    const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    SDL_FPoint p = _sdl_pos(fr, &L_base);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur or one of cur's ancestors"
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

void pico_cv_pos_from (
    const char* layer, const Pico_Rel_Pos* fr, Pico_Rel_Pos* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    // chain: chain[0] = cur, chain[n-1] = S
    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur or one of cur's ancestors"
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

void pico_cv_rect_to (
    const char* layer, const Pico_Rel_Rect* fr, Pico_Rel_Rect* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    SDL_FRect r = _sdl_rect(fr, &L_base, NULL);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur or one of cur's ancestors"
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

void pico_cv_rect_from (
    const char* layer, const Pico_Rel_Rect* fr, Pico_Rel_Rect* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur or one of cur's ancestors"
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

void pico_cv_dim_to (
    const char* layer, const Pico_Rel_Dim* fr, Pico_Rel_Dim* to
) {
    _pico_guard();
    Pico_Layer* T = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    Pico_Abs_Rect L_base = {0, 0, L->scene.dim.w, L->scene.dim.h};
    Pico_Rel_Dim fr_copy = *fr;
    SDL_FDim d = _sdl_dim(&fr_copy, &L_base, NULL);

    while (L != T) {
        pico_assert (
            L->hier.up != NULL
            && "cv: target must be cur or one of cur's ancestors"
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

void pico_cv_dim_from (
    const char* layer, const Pico_Rel_Dim* fr, Pico_Rel_Dim* to
) {
    _pico_guard();
    Pico_Layer* S = (layer == NULL) ? G.layer : _pico_layer_name(layer);
    Pico_Layer* L = G.layer;

    Pico_Layer* chain[64];
    int n = 0;
    Pico_Layer* M = L;
    while (M != S) {
        assert(n < (int)(sizeof(chain)/sizeof(chain[0])));
        chain[n++] = M;
        pico_assert (
            M->hier.up != NULL
            && "cv: source must be cur or one of cur's ancestors"
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

///////////////////////////////////////////////////////////////////////////////
// IN: compose child onto parent, return flat rel
///////////////////////////////////////////////////////////////////////////////

Pico_Rel_Rect pico_in_rect (const Pico_Rel_Rect* out, const Pico_Rel_Rect* in) {
    Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(out, NULL);
    Pico_Abs_Rect in_abs  = pico_cv_rect_rel_abs(in, &out_abs);
    Pico_Rel_Rect ret = { .mode = in->mode, .anchor = in->anchor };
    pico_cv_rect_abs_rel(&in_abs, &ret, NULL);
    return ret;
}

Pico_Rel_Pos pico_in_pos (const Pico_Rel_Rect* out, const Pico_Rel_Pos* in) {
    Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(out, NULL);
    Pico_Abs_Pos  in_abs  = pico_cv_pos_rel_abs(in, &out_abs);
    Pico_Rel_Pos ret = { .mode = in->mode, .anchor = in->anchor };
    pico_cv_pos_abs_rel(&in_abs, &ret, NULL);
    return ret;
}

Pico_Rel_Dim pico_in_dim (const Pico_Rel_Rect* out, const Pico_Rel_Dim* in) {
    Pico_Abs_Rect out_abs = pico_cv_rect_rel_abs(out, NULL);
    Pico_Rel_Dim  in_copy = *in;
    Pico_Abs_Dim  in_abs  = pico_cv_dim_rel_abs(&in_copy, &out_abs);
    Pico_Rel_Dim ret = { .mode = in->mode };
    pico_cv_dim_abs_rel(&in_abs, &ret, NULL);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// VS
///////////////////////////////////////////////////////////////////////////////

int pico_vs_pos_rect (Pico_Rel_Pos* pos, Pico_Rel_Rect* rect) {
    Pico_Abs_Pos  pi = pico_cv_pos_rel_abs(pos, NULL);
    Pico_Abs_Rect ri = pico_cv_rect_rel_abs(rect, NULL);
    return SDL_PointInRect(&pi, &ri);
}

int pico_vs_rect_rect (Pico_Rel_Rect* r1, Pico_Rel_Rect* r2) {
    Pico_Abs_Rect i1 = pico_cv_rect_rel_abs(r1, NULL);
    Pico_Abs_Rect i2 = pico_cv_rect_rel_abs(r2, NULL);
    return SDL_HasIntersection(&i1, &i2);
}


