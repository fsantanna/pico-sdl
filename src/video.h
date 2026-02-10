#ifndef PICO_VIDEO_H
#define PICO_VIDEO_H

#endif // PICO_VIDEO_H

#ifdef PICO_VIDEO_C

///////////////////////////////////////////////////////////////////////////////
// VIDEO
///////////////////////////////////////////////////////////////////////////////

/* Parse Y4M file header.
 * Format: YUV4MPEG2 W<w> H<h> F<num>:<den> ...
 * Returns 1 on success, 0 on failure.
 */
static int _y4m_parse_header (
    FILE* fp, int* w, int* h, int* fps
) {
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

/* Get or create video state by file path */
static Pico_Video_State* _pico_video_open (
    const char* path
) {
    int n = sizeof(Pico_Key) + strlen(path) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_VIDEO;
    strcpy(key->key, path);

    Pico_Video_State* vs =
        (Pico_Video_State*)ttl_hash_get(G.hash, n, key);
    if (vs != NULL) {
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

    vs = calloc(1, sizeof(Pico_Video_State));
    assert(vs != NULL);
    vs->fp = fp;
    vs->width = w;
    vs->height = h;
    vs->fps = fps;
    vs->size.y = w * h;
    vs->size.uv = (w / 2) * (h / 2);
    vs->size.frame = 6 + vs->size.y + vs->size.uv * 2;
    vs->data_offset = ftell(fp);
    vs->frame.cur = -1;
    vs->frame.done = 0;
    vs->t0 = 0;
    vs->layer = NULL;

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

    ttl_hash_put(G.hash, n, key, vs);
    return vs;
}

/* Read a single Y4M frame at current file position */
static int _y4m_read_frame (Pico_Video_State* vs) {
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

/* Update the layer's YUV texture from video state */
static void _y4m_update_texture (Pico_Video_State* vs) {
    if (vs->layer == NULL) {
        return;
    }
    SDL_UpdateYUVTexture(
        vs->layer->tex, NULL,
        vs->plane.y, vs->width,
        vs->plane.u, vs->width / 2,
        vs->plane.v, vs->width / 2
    );
}

const char* pico_layer_video (
    const char* name, const char* path
) {
    assert(name != NULL && "layer name required");
    assert(path != NULL && "video path required");

    Pico_Video_State* vs = _pico_video_open(path);
    pico_assert(vs != NULL);

    /* Reuse existing layer */
    int n = sizeof(Pico_Key) + strlen(name) + 1;
    Pico_Key* key = alloca(n);
    key->type = PICO_KEY_LAYER;
    strcpy(key->key, name);

    Pico_Layer* layer =
        (Pico_Layer*)ttl_hash_get(G.hash, n, key);
    if (layer != NULL) {
        layer->extra = vs;
        vs->layer = layer;
        return layer->key->key;
    }

    SDL_Texture* tex = SDL_CreateTexture(
        G.ren, SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        vs->width, vs->height
    );
    pico_assert(tex != NULL);

    layer = malloc(sizeof(Pico_Layer));
    *layer = (Pico_Layer) {
        .key = ttl_hash_put(G.hash, n, key, layer),
        .tex = tex,
        .view = {
            .grid = 0,
            .dim  = {vs->width, vs->height},
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
        .extra = vs,
    };
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    vs->layer = layer;
    return layer->key->key;
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
    Pico_Video_State* vs =
        (Pico_Video_State*)layer->extra;
    assert(vs != NULL && "not a video layer");

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

int pico_output_draw_video (
    const char* path, Pico_Rel_Rect* rect
) {
    Pico_Video_State* vs = _pico_video_open(path);
    pico_assert(vs != NULL);

    /* Auto-create layer using path as name */
    const char* name = pico_layer_video(
        path, path
    );

    /* Auto-sync: calculate frame from elapsed time */
    if (vs->t0 == 0) {
        vs->t0 = SDL_GetTicks();
    }
    int dt = SDL_GetTicks() - vs->t0;
    int frame = dt * vs->fps / 1000;

    if (!pico_video_sync(name, frame)) {
        return 0;
    }

    /* Draw */
    _pico_output_draw_layer(vs->layer, rect);
    return 1;
}

Pico_Video pico_get_video (
    const char* path, Pico_Rel_Rect* rect
) {
    Pico_Video_State* vs = _pico_video_open(path);
    pico_assert(vs != NULL);

    Pico_Video info = {
        .dim   = {vs->width, vs->height},
        .fps   = vs->fps,
        .frame = (vs->frame.cur < 0) ? 0 : vs->frame.cur,
        .done  = vs->frame.done,
    };

    /* Fill rect dimensions if provided */
    if (rect != NULL && (rect->w == 0 || rect->h == 0)) {
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
