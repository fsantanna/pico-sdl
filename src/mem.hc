#ifndef PICO_MEM_HC
#define PICO_MEM_HC

///////////////////////////////////////////////////////////////////////////////
// Context structs for realm alloc callbacks
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    Pico_Abs_Dim dim;
    const Pico_Color_A* pixels;
} _Ctx_Buffer;

typedef struct {
    Pico_Layer* par;
    Pico_Rel_Rect crop;
} _Ctx_Sub;

typedef struct {
    int height;
    const char* text;
} _Ctx_Text;

typedef struct {
    const char* path;
    int h;
} _Ctx_Font;

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
        _pico_hash_clean_video((Pico_Layer_Video*)data);
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

static Pico_View _layer_new (Pico_Abs_Dim dim) {
    return (Pico_View) {
        .grid = 0,
        .dim  = dim,
        .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
        .src  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
        .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
        .tile = {0, 0},
        .rot  = {0, PICO_ANCHOR_C},
        .flip = PICO_FLIP_NONE,
    };
}

///////////////////////////////////////////////////////////////////////////////
// Alloc callbacks
///////////////////////////////////////////////////////////////////////////////

static void* _alloc_layer_buffer (int n, const void* key, void* ctx) {
    _Ctx_Buffer* c = (_Ctx_Buffer*)ctx;
    SDL_Surface* sfc = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)c->pixels, c->dim.w, c->dim.h,
        32, 4 * c->dim.w, SDL_PIXELFORMAT_RGBA32
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(G.ren, sfc);
    pico_assert(tex != NULL);
    SDL_FreeSurface(sfc);
    Pico_Layer* data = malloc(sizeof(Pico_Layer));
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = PICO_LAYER_PLAIN,
        .name = strdup((const char*)key),
        .tex  = tex,
        .view = _layer_new(c->dim),
    };
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
}

static void* _alloc_layer_empty (int n, const void* key, void* ctx) {
    Pico_Abs_Dim* dim = (Pico_Abs_Dim*)ctx;
    Pico_Layer* data = malloc(sizeof(Pico_Layer));
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = PICO_LAYER_PLAIN,
        .name = strdup((const char*)key),
        .tex  = _tex_create(*dim),
        .view = _layer_new(*dim),
    };
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(data->tex, SDL_BLENDMODE_BLEND);
    return data;
}

static void* _alloc_layer_image (int n, const void* key, void* ctx) {
    const char* path = (const char*)ctx;
    SDL_Texture* tex = IMG_LoadTexture(G.ren, path);
    pico_assert(tex != NULL);
    Pico_Abs_Dim dim;
    SDL_QueryTexture(tex, NULL, NULL, &dim.w, &dim.h);
    Pico_Layer* data = malloc(sizeof(Pico_Layer));
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = PICO_LAYER_PLAIN,
        .name = strdup((const char*)key),
        .tex  = tex,
        .view = _layer_new(dim),
    };
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
}

static void* _alloc_layer_sub (int n, const void* key, void* ctx) {
    _Ctx_Sub* c = (_Ctx_Sub*)ctx;
    Pico_Abs_Rect abs = pico_cv_rect_rel_abs(
        &c->crop,
        &(Pico_Abs_Rect){0, 0, c->par->view.dim.w, c->par->view.dim.h}
    );
    Pico_Layer* data = malloc(sizeof(Pico_Layer));
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type   = PICO_LAYER_SUB,
        .name   = strdup((const char*)key),
        .tex    = c->par->tex,
        .view   = {
            .grid = 0,
            .dim  = {abs.w, abs.h},
            .dst  = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .src  = c->crop,
            .clip = {'%', {.5,.5,1,1}, PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
            .rot  = {0, PICO_ANCHOR_C},
            .flip = PICO_FLIP_NONE,
        },
        .parent = c->par,
    };
    assert(data->name != NULL);
    data->view.src.up = &c->par->view.src;
    return data;
}

static void* _alloc_layer_text (int n, const void* key, void* ctx) {
    _Ctx_Text* c = (_Ctx_Text*)ctx;
    Pico_Abs_Dim dim;
    SDL_Texture* tex = _tex_text(c->height, c->text, &dim);
    Pico_Layer* data = malloc(sizeof(Pico_Layer));
    assert(data != NULL);
    *data = (Pico_Layer) {
        .type = PICO_LAYER_PLAIN,
        .name = strdup((const char*)key),
        .tex  = tex,
        .view = _layer_new(dim),
    };
    assert(data->name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return data;
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

    Pico_Layer_Video* vs = calloc(1, sizeof(Pico_Layer_Video));
    assert(vs != NULL);
    vs->base = (Pico_Layer) {
        .type = PICO_LAYER_VIDEO,
        .name = strdup((const char*)key),
        .tex  = tex,
        .view = _layer_new((Pico_Abs_Dim){w, h}),
    };
    assert(vs->base.name != NULL);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

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
    _Ctx_Font* c = (_Ctx_Font*)ctx;
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
