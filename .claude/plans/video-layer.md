# video-layer.md — Pico_Layer_Video as subtype of Pico_Layer

## Problem

`Pico_Layer` has a `void* extra` field (line 55) solely to hold a pointer to
`Pico_Video_State`. This creates a bidirectional link (`extra` ↔ `layer`)
between two separate hash entries (`PICO_KEY_LAYER` + `PICO_KEY_VIDEO`),
requiring complex cross-cleanup in `_pico_hash_clean`.

## Goal

Use C struct-prefix inheritance: `Pico_Layer_Video` embeds `Pico_Layer` as
its first member. Only the video entry lives in the hash — no separate layer
entry, no `void* extra`, no bidirectional link.

## Design

### Type hierarchy

```c
typedef struct {
    const Pico_Key* key;
    SDL_Texture*    tex;
    Pico_View       view;
} Pico_Layer;                    // remove void* extra

typedef struct {
    Pico_Layer      base;        // must be first (C inheritance)
    FILE*           fp;
    /* ... video-specific fields ... */
} Pico_Layer_Video;              // replaces Pico_Video_State
```

A `Pico_Layer_Video*` can be cast to `Pico_Layer*` and vice-versa.

### Hash storage

- `Pico_Layer_Video` is stored under `PICO_KEY_LAYER` (keyed by **name**),
  since it IS a layer.
- `PICO_KEY_VIDEO` is eliminated entirely.
- `_pico_video_open` is replaced: video state now lives inside the layer.

### Layer type discrimination

Add a type tag to `Pico_Layer` so cleanup knows what to free:

```c
typedef enum {
    PICO_LAYER_PLAIN,
    PICO_LAYER_VIDEO,
} PICO_LAYER;

typedef struct {
    PICO_LAYER      type;        // new field
    const Pico_Key* key;
    SDL_Texture*    tex;
    Pico_View       view;
} Pico_Layer;
```

Plain layers (image, text, empty, buffer) use `PICO_LAYER_PLAIN`.
Video layers use `PICO_LAYER_VIDEO`.

### Cleanup

`_pico_hash_clean(PICO_KEY_LAYER)` checks `layer->type`:
- `PICO_LAYER_PLAIN`: destroy texture, free layer (as today).
- `PICO_LAYER_VIDEO`: also close fp, free planes, then free
  (as `Pico_Layer_Video`).

The `PICO_KEY_VIDEO` case disappears entirely.

## Changes

### pico.c (or video.c after video-files.md)

| location             | change                                         |
|----------------------|-------------------------------------------------|
| `Pico_Layer`         | remove `void* extra`, add `PICO_LAYER type`     |
| `Pico_Video_State`   | rename to `Pico_Layer_Video`, embed `Pico_Layer base` as first field, remove `Pico_Layer* layer` |
| `PICO_KEY_TYPE`      | remove `PICO_KEY_VIDEO`                         |
| `_pico_hash_clean`   | remove `PICO_KEY_VIDEO` case; in `PICO_KEY_LAYER`, check `type` for video cleanup |
| `_pico_video_open`   | remove (video state lives inside the layer)     |
| `pico_layer_video`   | allocate `Pico_Layer_Video`, parse Y4M, set `base.type = PICO_LAYER_VIDEO`, store under `PICO_KEY_LAYER` |
| `pico_video_sync`    | look up layer, cast to `Pico_Layer_Video*`      |
| `pico_output_draw_video` | create layer via `pico_layer_video`, cast to access video fields |
| `pico_get_video`     | look up layer by name (not by path), cast       |
| all `pico_layer_*`   | set `type = PICO_LAYER_PLAIN` on allocation     |
| `pico_layer_empty`, `_pico_layer_image`, etc. | set `.type = PICO_LAYER_PLAIN` |

### pico.h

| location      | change                                           |
|---------------|--------------------------------------------------|
| `Pico_Video`  | unchanged (public query struct, not internal)     |
| `pico_get_video` | unchanged (path IS the layer name — `pico_output_draw_video` uses `pico_layer_video(path, path)`) |

## API impact

No public API changes. `pico_get_video(path)` continues to work because
`pico_output_draw_video` already uses the path as the layer name.

## Order

Apply **after** video-files.md and video-struct.md.
