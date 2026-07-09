#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "_pico.h"


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
        vs->plane.y, vs->base.scene.dim.w,
        vs->plane.u, vs->base.scene.dim.w / 2,
        vs->plane.v, vs->base.scene.dim.w / 2
    );
}

///////////////////////////////////////////////////////////////////////////////

/* Free video-specific resources (called from _pico_mem_free_layer) */
void _pico_video_free_layer (Pico_Layer_Video* vs) {
    free(vs->plane.y);
    free(vs->plane.u);
    free(vs->plane.v);
    fclose(vs->fp);
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
        G.window.ren, SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING, w, h
    );
    pico_assert(tex != NULL);

    Pico_Layer_Video* vs = (Pico_Layer_Video*)_pico_mem_layer_new (
        0, PICO_LAYER_VIDEO, sizeof(Pico_Layer_Video),
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

///////////////////////////////////////////////////////////////////////////////

static Pico_Layer_Video* _layer_video (
    int mode, const char* key, const char* path
) {
    assert(path!=NULL && "video path required");
    const char* str = (key != NULL) ? key : path;
    Pico_Layer_Video* ret = (Pico_Layer_Video*) realm_put(
        G.realm, mode, strlen(str)+1, (const void**)&str,
        _pico_mem_free_layer, _alloc_layer_video, (void*)path
    );
    pico_assert_key(ret, str);
    return ret;
}

const char* pico_layer_video (const char* up, const char* key, const char* path) {
    _pico_guard();
    return pico_layer_video_mode('!', up, key, path);
}

const char* pico_layer_video_mode (
    int mode, const char* up, const char* key, const char* path
) {
    _pico_guard();
    assert(path != NULL && "video path required");
    Pico_Layer_Video* ret = _layer_video(mode, key, path);
    if (up != NULL) {
        _pico_layer_attach(up, ret->base.name);
    }
    return ret->base.name;
}

///////////////////////////////////////////////////////////////////////////////

Pico_Video pico_get_video (Pico_Rel_Rect* rect, const char* path) {
    _pico_guard();
    Pico_Layer_Video* vs = _layer_video('=', path, path);

    Pico_Video info = {
        .dim   = vs->base.scene.dim,
        .fps   = vs->fps,
        .frame = (vs->frame.cur < 0) ? 0 : vs->frame.cur,
        .done  = vs->frame.done,
    };

    /* Fill rect dimensions if provided */
    if (rect!=NULL && (rect->w==0 || rect->h==0)) {
        Pico_Rel_Dim rel = { rect->mode, {rect->w, rect->h} };
        _pico_abs_dim(&rel, NULL, &info.dim);
        rect->w = rel.w;
        rect->h = rel.h;
    }

    return info;
}

int pico_set_video (const char* key, int frame) {
    _pico_guard();
    assert(key != NULL && "layer key required");

    /* Find layer by key */
    int n = strlen(key) + 1;
    Pico_Layer* layer = (Pico_Layer*)realm_get(
        G.realm, n, key);
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

int pico_output_draw_video (const char* path, Pico_Rel_Rect rect) {
    Pico_Layer_Video* vs = _layer_video('=', path, path);

    /* Auto-sync: calculate frame from elapsed time */
    if (vs->t0 == 0) {
        vs->t0 = SDL_GetTicks();
    }
    int dt = SDL_GetTicks() - vs->t0;
    int frame = dt * vs->fps / 1000;

    if (!pico_set_video(path, frame)) {
        return 0;
    }

    /* Draw */
    _pico_layer_output(&vs->base, &rect);
    return 1;
}
