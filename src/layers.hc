#ifndef PICO_LAYERS_HC
#define PICO_LAYERS_HC

typedef enum {
    PICO_LAYER_ROOT,
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
    PICO_LAYER_SUB,
} PICO_LAYER;

typedef struct Pico_Layer {
    PICO_LAYER            type;
    char*                 name;     // NULL for main layer
    SDL_Texture*          tex;
    Pico_View             view;
    struct {
        const char* up;             // parent id; NULL = root or detached
        const char* nxt;            // next sibling under same up
        struct {
            const char* fst;        // first child (back; drawn first)
            const char* lst;        // last child  (front; drawn last)
        } dn;
    } hier;
} Pico_Layer;

typedef struct {
    Pico_Layer   base;
    Pico_Abs_Dim sup;     // snapshot of source view.dim
} Pico_Layer_Sub;

static Pico_Layer* _pico_layer_buffer (
    int mode, const char* key, Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
);
static Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
);
static Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
);
static void _pico_output_draw_layer (
    Pico_Layer* layer, Pico_Rel_Rect* rect
);

#endif // PICO_LAYERS_HC

#ifdef PICO_LAYERS_C

///////////////////////////////////////////////////////////////////////////////
// LAYER (internal)
///////////////////////////////////////////////////////////////////////////////

static Pico_Layer* _pico_layer_buffer (
    int mode,
    const char* key,
    Pico_Abs_Dim dim,
    const Pico_Color_A* pixels
) {
    assert(key!=NULL && "layer key required");
    assert(pixels!=NULL && "pixels required");
    _alloc_buffer_t ctx = { dim, pixels };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(key)+1, key,
        _free_layer, _alloc_layer_buffer, &ctx
    );
    assert(ret != NULL);
    return ret;
}

static Pico_Layer* _pico_layer_image (
    int mode, const char* key, const char* path
) {
    assert(path!=NULL && "image path required");
    const char* str = (key != NULL) ? key : path;
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, str,
        _free_layer, _alloc_layer_image, (void*)path
    );
    assert(ret != NULL);
    return ret;
}

static Pico_Layer* _pico_layer_text (
    int mode, const char* key, int height, const char* text
) {
    assert(text!=NULL && text[0]!='\0' && "text required");

    const char* str;
    char* str_buf = NULL;
    if (key == NULL) {
        const char* font = S.font;
        Pico_Color clr = S.color.draw;
        const char* font_str = font ? font : "null";
        int buflen = strlen("/text/") + strlen(font_str) + 1
            + 10 + 1 + 3+1+3+1+3 + 1 + strlen(text) + 1;
        str_buf = alloca(buflen);
        snprintf(str_buf, buflen, "/text/%s/%d/%d.%d.%d/%s",
                 font_str, height, clr.r, clr.g, clr.b, text);
        str = str_buf;
    } else {
        str = key;
    }

    _alloc_text_t ctx = { height, text };
    Pico_Layer* ret = (Pico_Layer*) realm_put (
        G.realm, mode, strlen(str)+1, str,
        _free_layer, _alloc_layer_text, &ctx
    );
    assert(ret != NULL);
    return ret;
}

static void _layer_attach (Pico_Layer* up, Pico_Layer* self) {
    self->hier.nxt = NULL;
    if (up->hier.dn.fst == NULL) {
        up->hier.dn.fst = self->name;
        up->hier.dn.lst = self->name;
    } else {
        Pico_Layer* tail = (Pico_Layer*) realm_get(G.realm, strlen(up->hier.dn.lst)+1, up->hier.dn.lst);
        tail->hier.nxt = self->name;
        up->hier.dn.lst = self->name;
    }
}

///////////////////////////////////////////////////////////////////////////////

static void _pico_output_draw_layer (
    Pico_Layer* layer, Pico_Rel_Rect* rect
) {
    // recurse: composite children onto layer->tex first
    const char* cid = layer->hier.dn.fst;
    while (cid != NULL) {
        Pico_Layer* c = (Pico_Layer*) realm_get(G.realm, strlen(cid)+1, cid);
        if (c == NULL) {
            break;
        }
        SDL_SetRenderTarget(G.ren, layer->tex);
        _pico_output_draw_layer(c, NULL);
        cid = c->hier.nxt;
    }

    // blit layer onto current render target
    if (rect == NULL) {
        rect = &layer->view.dst;
    }
    Pico_Abs_Dim* dp = NULL;
    if (rect->w == 0 || rect->h == 0) {
        dp = &layer->view.dim;
    }
    SDL_FRect rf = _sdl_rect(rect, NULL, dp);
    SDL_Rect dst = _abs_rect(&rf);

    Pico_Abs_Dim* sup = (layer->type == PICO_LAYER_SUB) ? &((Pico_Layer_Sub*)layer)->sup : &layer->view.dim;
    Pico_Abs_Rect src = pico_cv_rect_rel_abs (
        &layer->view.src,
        &(Pico_Abs_Rect){0, 0, sup->w, sup->h}
    );

    SDL_SetTextureAlphaMod(layer->tex, S.alpha);
    SDL_Point center = {
        dst.w * layer->view.rot.anchor.x,
        dst.h * layer->view.rot.anchor.y
    };
    SDL_RenderCopyEx(G.ren, layer->tex, &src, &dst,
                     layer->view.rot.angle, &center,
                     layer->view.flip);

    if (layer->view.grid) {
        _show_tile(&layer->view, dst);
    }

    _pico_output_present(0);
}

#undef PICO_LAYERS_C
#endif
