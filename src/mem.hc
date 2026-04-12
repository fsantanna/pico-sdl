#ifndef PICO_MEM_HC
#define PICO_MEM_HC

///////////////////////////////////////////////////////////////////////////////
// Context structs for realm alloc callbacks
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    Pico_Abs_Dim dim;
    const Pico_Color* pixels;
} _alloc_buffer_t;

typedef struct {
    Pico_Abs_Dim  dim;
    Pico_Abs_Dim* tile;
} _alloc_empty_t;

typedef struct {
    Pico_Layer* par;
    Pico_Rel_Rect crop;
} _alloc_sub_t;

typedef struct {
    int height;
    const char* text;
} _alloc_text_t;

typedef struct {
    const char* path;
    int h;
} _alloc_font_t;

///////////////////////////////////////////////////////////////////////////////
// Free callbacks
///////////////////////////////////////////////////////////////////////////////

static void _free_font (int, const void*, void*);
static void _free_layer (int, const void*, void*);
static void _free_sound (int, const void*, void*);

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

static void* _alloc_layer_buffer (int, const void*, void*);
static void* _alloc_layer_empty (int, const void*, void*);
static void* _alloc_layer_image (int, const void*, void*);
static void* _alloc_layer_sub (int, const void*, void*);
static void* _alloc_layer_text (int, const void*, void*);
static void* _alloc_layer_video (int, const void*, void*);
static void* _alloc_font (int, const void*, void*);
static void* _alloc_sound (int, const void*, void*);

#endif // PICO_MEM_HC

///////////////////////////////////////////////////////////////////////////////

#ifdef PICO_MEM_C

///////////////////////////////////////////////////////////////////////////////
// Free callbacks
///////////////////////////////////////////////////////////////////////////////

static void _free_font (int n, const void* key, void* value) {
    TTF_CloseFont((TTF_Font*)value);
}

static void _free_layer (int n, const void* key, void* value) {
    Pico_Layer* data = (Pico_Layer*)value;
    if (data->type == PICO_LAYER_VIDEO) {
        _free_layer_video((Pico_Layer_Video*)data);
    }
    if (data->type != PICO_LAYER_SUB) {
        SDL_DestroyTexture(data->tex);
    }
    free(data->name);
    free(data);
}

static void _free_sound (int n, const void* key, void* value) {
    Mix_FreeChunk((Mix_Chunk*)value);
}

///////////////////////////////////////////////////////////////////////////////
// Alloc helpers
///////////////////////////////////////////////////////////////////////////////

static Pico_Layer* _layer_new (
    int type, size_t size, const char* key, SDL_Texture* tex, Pico_Abs_Dim dim
) {
    Pico_Layer* data = calloc(1, size);
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = type,
        .name = strdup(key),
        .tex  = tex,
        .draw = {
            {0xFF, 0xFF, 0xFF, 0xFF}, PICO_STYLE_FILL, NULL
        },
        .show = {
            {0, 0, 0, 0xFF}, 0, {0, PICO_ANCHOR_C}, PICO_FLIP_NONE, 0xFF
        },
        .view = {
            dim, {0, 0},
            {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
        },
    };
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

static void* _alloc_layer_buffer (int n, const void* key, void* ctx) {
    _alloc_buffer_t* c = (_alloc_buffer_t*)ctx;
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)c->pixels, c->dim.w, c->dim.h,
        32, 4 * c->dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    return _layer_new (
        PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, c->dim
    );
}

static void* _alloc_layer_empty (int n, const void* key, void* ctx) {
    _alloc_empty_t* arg = (_alloc_empty_t*)ctx;
    Pico_Abs_Dim dim = arg->dim;
    if (arg->tile != NULL) {
        dim.w *= arg->tile->w;
        dim.h *= arg->tile->h;
    }
    Pico_Layer* lay = _layer_new (
        PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, _tex_create(dim), dim
    );
    if (arg->tile != NULL) {
        lay->view.tile = *arg->tile;
    } else {
    }
    return lay;
}

static void* _alloc_layer_image (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    SDL_Texture* tex = IMG_LoadTexture(G.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    return _layer_new (
        PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

static void* _alloc_layer_sub (int n, const void* key, void* ctx) {
    _alloc_sub_t* c = (_alloc_sub_t*)ctx;
    Pico_Abs_Rect abs = pico_cv_rect_rel_abs(
        &c->crop,
        &(Pico_Abs_Rect){0, 0, c->par->view.dim.w, c->par->view.dim.h}
    );
    Pico_Layer* data = _layer_new (
        PICO_LAYER_SUB, sizeof(Pico_Layer_Sub),
        (const char*)key, c->par->tex,
        (Pico_Abs_Dim){abs.w, abs.h}
    );
    data->view.src = c->crop;
    data->view.src.up = &c->par->view.src;
    ((Pico_Layer_Sub*)data)->sup = c->par->view.dim;
    return data;
}

static SDL_Texture* _tex_text (int height, const char* text, Pico_Abs_Dim* dim) {
    SDL_Color c = { S.layer->draw.color.r, S.layer->draw.color.g, S.layer->draw.color.b, 0xFF };
    TTF_Font* ttf = _font_get(S.layer->draw.font, height);
    SDL_Surface* sfc = TTF_RenderText_Solid(ttf, text, c);
    pico_assert(sfc != NULL);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    *dim = (Pico_Abs_Dim){ sfc->w, sfc->h };
    SDL_FreeSurface(sfc);
    return tex;
}

static void* _alloc_layer_text (int n, const void* key, void* ctx) {
    _alloc_text_t* c = (_alloc_text_t*)ctx;
    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(c->height, c->text, &dim);
    return _layer_new (
        PICO_LAYER_PLAIN, sizeof(Pico_Layer),
        (const char*)key, tex, dim
    );
}

static void* _alloc_layer_video (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;

    FILE* fp = fopen(path, "rb");
    pico_assert(fp != NULL);

    int w, h, fps;
    if (!_y4m_parse_header(fp, &w, &h, &fps)) {
        fclose(fp);
        assert(0 && "invalid Y4M header");
    }

    SDL_Texture* tex = SDL_CreateTexture(
        G.ren, SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING, w, h
    );
    pico_assert(tex != NULL);

    Pico_Layer_Video* vs = (Pico_Layer_Video*)_layer_new (
        PICO_LAYER_VIDEO, sizeof(Pico_Layer_Video),
        (const char*)key, tex, (Pico_Abs_Dim){w, h}
    );

    vs->fp = fp;
    vs->fps = fps;
    vs->size.y = w * h;
    vs->size.uv = (w / 2) * (h / 2);
    vs->size.frame = 6 + vs->size.y + vs->size.uv * 2;
    vs->data_offset = ftell(fp);
    vs->frame.cur = -1;
    vs->frame.done = 0;
    vs->t0 = 0;

    vs->plane.y = malloc(vs->size.y);
    vs->plane.u = malloc(vs->size.uv);
    vs->plane.v = malloc(vs->size.uv);
    assert(vs->plane.y && vs->plane.u && vs->plane.v);

    long cur = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long end = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    vs->frame.total =
        (int)((end - vs->data_offset) / vs->size.frame);

    return vs;
}

static void* _alloc_font (int n, const void* key, void* ctx) {
    _alloc_font_t* c = (_alloc_font_t*)ctx;
    TTF_Font* ttf;
    if (c->path == NULL) {
        SDL_RWops* rw = SDL_RWFromConstMem(pico_tiny_ttf, pico_tiny_ttf_len);
        ttf = TTF_OpenFontRW(rw, 1, c->h);
    } else {
        ttf = TTF_OpenFont(c->path, c->h);
    }
    pico_assert(ttf != NULL);
    return ttf;
}

static void* _alloc_sound (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    return Mix_LoadWAV(path);
}

#undef PICO_MEM_C
#endif
