# video-layer.md — Pico_Layer_Video as subtype of Pico_Layer

Status: **done**

## Problem

`Pico_Layer` had a `void* extra` field solely to hold a pointer to
`Pico_Video_State`. This created a bidirectional link (`extra` <-> `layer`)
between two separate hash entries (`PICO_KEY_LAYER` + `PICO_KEY_VIDEO`),
requiring complex cross-cleanup in `_pico_hash_clean`.

## Solution

C struct-prefix inheritance: `Pico_Layer_Video` embeds `Pico_Layer base` as
its first member. Only one hash entry (`PICO_KEY_LAYER`) per video layer.
No `void* extra`, no `PICO_KEY_VIDEO`, no bidirectional link.

## Type hierarchy

```c
typedef enum {
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
} PICO_LAYER;

typedef struct {
    PICO_LAYER      type;
    const Pico_Key* key;
    SDL_Texture*    tex;
    Pico_View       view;
} Pico_Layer;                       // in pico.c

typedef struct {
    Pico_Layer base;                // must be first (C inheritance)
    FILE*      fp;
    int        fps;
    struct { unsigned char *y, *u, *v; } plane;
    struct { int y, uv, frame; }         size;
    long       data_offset;
    struct { int total, cur, done; }     frame;
    Uint32     t0;
} Pico_Layer_Video;                 // in video.h
```

## File organization (two-include pattern)

`video.h` has two independent sections:

```
#ifndef PICO_VIDEO_H        <- types section
  Pico_Layer_Video typedef
  _pico_hash_clean_video declaration
#endif

#ifdef PICO_VIDEO_C          <- implementation section
  _y4m_parse_header
  _pico_hash_clean_video definition
  _pico_layer_video
  pico_layer_video
  _y4m_read_frame
  _y4m_update_texture
  pico_set_video
  pico_output_draw_video
  pico_get_video
#endif
```

`pico.c` includes `video.h` twice:

1. After `Pico_Layer` definition — gets types + cleanup declaration
2. At end of file with `#define PICO_VIDEO_C` — gets implementations

This eliminates all forward declarations.

## Changes applied

### pico.c

| location             | change                                         |
|----------------------|-------------------------------------------------|
| `PICO_KEY_TYPE`      | removed `PICO_KEY_VIDEO`                        |
| `PICO_LAYER` enum    | new: `PICO_LAYER_PLAIN`, `PICO_LAYER_VIDEO`    |
| `Pico_Layer`         | removed `void* extra`, added `PICO_LAYER type`  |
| after `Pico_Layer`   | `#include "video.h"` (first include, types)     |
| `_pico_hash_clean`   | removed `PICO_KEY_VIDEO` case; `PICO_KEY_LAYER` checks `type` for video cleanup |
| `G.main`             | `.type = PICO_LAYER_PLAIN`                      |
| all `pico_layer_*`   | `.type = PICO_LAYER_PLAIN` on allocation        |
| end of file          | `#define PICO_VIDEO_C` + `#include "video.h"`   |

### video.h

| location                  | change                                    |
|---------------------------|-------------------------------------------|
| `#ifndef` section         | `Pico_Layer_Video` typedef + `_pico_hash_clean_video` declaration |
| `_pico_video_open`        | removed (absorbed into `_pico_layer_video`) |
| `_pico_layer_video`       | new: looks up or creates `Pico_Layer_Video` under `PICO_KEY_LAYER` |
| `pico_layer_video`        | simplified wrapper returning `->base.key->key` |
| `_y4m_read_frame`         | param `Pico_Layer_Video*`                 |
| `_y4m_update_texture`     | uses `vs->base.tex`, `vs->base.view.dim.w` |
| `pico_set_video`         | casts `(Pico_Layer_Video*)layer`          |
| `pico_output_draw_video`  | uses `_pico_layer_video`, `&vs->base`     |
| `pico_get_video`          | uses `_pico_layer_video`, `vs->base.view.dim` |

### pico.h

No changes. Public API unchanged.

## Order

Applied **after** video-files.md and video-struct.md.
