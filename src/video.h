#ifndef PICO_VIDEO_H
#define PICO_VIDEO_H

typedef struct {
    Pico_Layer base;
    FILE*      fp;
    int        fps;
    struct {
        unsigned char* y;
        unsigned char* u;
        unsigned char* v;
    } plane;
    struct {
        int y;
        int uv;
        int frame;
    } size;
    long       data_offset;
    struct {
        int total;
        int cur;
        int done;
    } frame;
    Uint32     t0;
} Pico_Layer_Video;

static void _pico_hash_clean_video (Pico_Layer_Video*);

#endif // PICO_VIDEO_H

#ifdef PICO_VIDEO_C

///////////////////////////////////////////////////////////////////////////////
// VIDEO
///////////////////////////////////////////////////////////////////////////////

/* Parse Y4M file header.
 * Format: YUV4MPEG2 W<w> H<h> F<num>:<den> ...
 * Returns 1 on success, 0 on failure.
 */
static int _y4m_parse_header (FILE* fp, int* w, int* h, int* fps) {
    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        return 0;
    }
    if (strncmp(line, "YUV4MPEG2", 9) != 0) {
        return 0;
    }

    *w = 0;
    *h = 0;
    *fps = 0;
    int fps_num = 0;
    int fps_den = 1;

    char* p = line + 9;
    while (*p != '\0' && *p != '\n') {
        while (*p == ' ') p++;
        if (*p == 'W') {
            *w = atoi(p + 1);
        } else if (*p == 'H') {
            *h = atoi(p + 1);
        } else if (*p == 'F') {
            sscanf(p + 1, "%d:%d", &fps_num, &fps_den);
        }
        while (*p != ' ' && *p != '\0' && *p != '\n') {
            p++;
        }
    }

    if (fps_den > 0) {
        *fps = fps_num / fps_den;
    }

    return (*w > 0 && *h > 0 && *fps > 0);
}

/* Free video-specific resources (called from hash cleanup) */
static void _pico_hash_clean_video (Pico_Layer_Video* vs) {
    free(vs->plane.y);
    free(vs->plane.u);
    free(vs->plane.v);
    fclose(vs->fp);
}

/* Get or create video layer by name */
static Pico_Layer_Video* _pico_layer_video (const char* name, const char* path) {
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);

    Pico_Layer_Video* vs =
        (Pico_Layer_Video*)ttl_hash_get(
            G.hash, n, key);
    if (vs != NULL) {
        assert(vs->base.type == PICO_LAYER_VIDEO);
        return vs;
    }

    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr,
            "Could not open video: %s\n", path);
        return NULL;
    }

    int w, h, fps;
    if (!_y4m_parse_header(fp, &w, &h, &fps)) {
        fprintf(stderr,
            "Invalid Y4M header: %s\n", path);
        fclose(fp);
        return NULL;
    }

    SDL_Texture* tex = SDL_CreateTexture(
        G.ren, SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
    );
    pico_assert(tex != NULL);

    vs = calloc(1, sizeof(Pico_Layer_Video));
    assert(vs != NULL);
    vs->base = (Pico_Layer) {
        .type = PICO_LAYER_VIDEO,
        .key  = ttl_hash_put(G.hash, n, key, vs),
        .tex  = tex,
        .view = {
            .grid = 0,
            .dim  = {w, h},
            .dst  = {'%', {.5,.5,1,1},
                      PICO_ANCHOR_C, NULL},
            .src  = {'%', {.5,.5,1,1},
                      PICO_ANCHOR_C, NULL},
            .clip = {'%', {.5,.5,1,1},
                      PICO_ANCHOR_C, NULL},
            .tile = {0, 0},
            .rot  = {0, PICO_ANCHOR_C},
            .flip = PICO_FLIP_NONE,
        },
    };
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

    /* Calculate total frames from file size */
    long cur = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long end = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    vs->frame.total =
        (int)((end - vs->data_offset) / vs->size.frame);

    return vs;
}

const char* pico_layer_video (const char* name, const char* path) {
    assert(name != NULL && "layer name required");
    assert(path != NULL && "video path required");

    Pico_Layer_Video* vs =
        _pico_layer_video(name, path);
    pico_assert(vs != NULL);
    return vs->base.key->key;
}

/* Read a single Y4M frame at current file position */
static int _y4m_read_frame (Pico_Layer_Video* vs) {
    char marker[6];
    if (fread(marker, 1, 6, vs->fp) != 6) {
        return 0;
    }
    if (strncmp(marker, "FRAME\n", 6) != 0) {
        return 0;
    }
    if (fread(vs->plane.y, 1, vs->size.y, vs->fp)
            != (size_t)vs->size.y ||
        fread(vs->plane.u, 1, vs->size.uv, vs->fp)
            != (size_t)vs->size.uv ||
        fread(vs->plane.v, 1, vs->size.uv, vs->fp)
            != (size_t)vs->size.uv) {
        return 0;
    }
    return 1;
}

/* Update the layer's YUV texture from video planes */
static void _y4m_update_texture (Pico_Layer_Video* vs) {
    SDL_UpdateYUVTexture(
        vs->base.tex, NULL,
        vs->plane.y, vs->base.view.dim.w,
        vs->plane.u, vs->base.view.dim.w / 2,
        vs->plane.v, vs->base.view.dim.w / 2
    );
}

int pico_video_sync (const char* name, int frame) {
    assert(name != NULL && "layer name required");

    /* Find layer by name */
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);
    Pico_Layer* layer =
        (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    pico_assert(layer != NULL && "layer does not exist");

    /* Get video state from layer */
    Pico_Layer_Video* vs = (Pico_Layer_Video*)layer;
    assert(vs->base.type == PICO_LAYER_VIDEO
        && "not a video layer");

    /* Clamp frame */
    if (frame < 0) {
        frame = 0;
    }
    if (frame >= vs->frame.total) {
        vs->frame.done = 1;
        return 0;
    }

    vs->frame.done = 0;

    /* Already at this frame */
    if (frame == vs->frame.cur) {
        return 1;
    }

    /* Seek to target frame */
    if (frame < vs->frame.cur || vs->frame.cur < 0) {
        /* Backward or initial: seek from start */
        long offset = vs->data_offset
            + (long)frame * vs->size.frame;
        fseek(vs->fp, offset, SEEK_SET);
        if (!_y4m_read_frame(vs)) {
            vs->frame.done = 1;
            return 0;
        }
        vs->frame.cur = frame;
    } else {
        /* Forward: skip intermediate frames */
        while (vs->frame.cur < frame) {
            long offset = vs->data_offset
                + (long)(vs->frame.cur + 1)
                * vs->size.frame;
            fseek(vs->fp, offset, SEEK_SET);
            if (!_y4m_read_frame(vs)) {
                vs->frame.done = 1;
                return 0;
            }
            vs->frame.cur++;
        }
    }

    _y4m_update_texture(vs);
    return 1;
}

int pico_output_draw_video (const char* path, Pico_Rel_Rect* rect) {
    Pico_Layer_Video* vs = _pico_layer_video(path, path);
    pico_assert(vs != NULL);

    /* Auto-sync: calculate frame from elapsed time */
    if (vs->t0 == 0) {
        vs->t0 = SDL_GetTicks();
    }
    int dt = SDL_GetTicks() - vs->t0;
    int frame = dt * vs->fps / 1000;

    if (!pico_video_sync(path, frame)) {
        return 0;
    }

    /* Draw */
    _pico_output_draw_layer(&vs->base, rect);
    return 1;
}

Pico_Video pico_get_video (const char* path, Pico_Rel_Rect* rect) {
    Pico_Layer_Video* vs =
        _pico_layer_video(path, path);
    pico_assert(vs != NULL);

    Pico_Video info = {
        .dim   = vs->base.view.dim,
        .fps   = vs->fps,
        .frame = (vs->frame.cur < 0) ? 0
                                      : vs->frame.cur,
        .done  = vs->frame.done,
    };

    /* Fill rect dimensions if provided */
    if (rect != NULL
            && (rect->w == 0 || rect->h == 0)) {
        Pico_Rel_Dim rel = {
            rect->mode, {rect->w, rect->h}, rect->up
        };
        _sdl_dim(&rel, NULL, &info.dim);
        rect->w = rel.w;
        rect->h = rel.h;
    }

    return info;
}

#endif // PICO_VIDEO_C
