# video-struct.md — Pack similar fields into anonymous structs

**Status: Done**

## Problem

`Pico_Video_State` (to become `Pico_Layer_Video`) has 16+ flat fields.
Related fields are not grouped, making the struct harder to read.

## Goal

Group related fields into anonymous structs for clarity, without changing
any external behavior.

## Current layout (Pico_Video_State, pico.c:58-75)

```c
typedef struct {
    FILE*          fp;
    unsigned char* y_plane;
    unsigned char* u_plane;
    unsigned char* v_plane;
    int            width;
    int            height;
    int            fps;
    int            y_size;
    int            uv_size;
    int            frame_size;
    long           data_offset;
    int            total_frames;
    int            cur_frame;
    int            done;
    Uint32         t0;
    Pico_Layer*    layer;
} Pico_Video_State;
```

## Proposed layout (Pico_Layer_Video)

```c
typedef struct {
    Pico_Layer base;             // inherited (video-layer.md)
    FILE*      fp;
    struct {                     // YUV plane buffers
        unsigned char* y;
        unsigned char* u;
        unsigned char* v;
    } plane;
    struct {                     // byte sizes for I/O
        int y;                   // width * height
        int uv;                  // (width/2) * (height/2)
        int frame;               // 6 + y + 2*uv
    } size;
    long data_offset;            // byte offset to first frame
    struct {                     // frame tracking
        int total;
        int cur;                 // -1 = not yet read
        int done;                // 1 = past EOF
    } frame;
    Uint32 t0;                   // auto-sync start time
} Pico_Layer_Video;
```

Notes:
- `width`/`height`/`fps` move to `base.view.dim` and a new `fps` field
  (or `fps` stays at top level since it has no group).
- `layer` pointer is gone (video-layer.md eliminates it).

### fps placement

`fps` is video metadata, not a view property. Keep it as a direct field:

```c
typedef struct {
    Pico_Layer base;
    FILE*      fp;
    int        fps;
    struct { ... } plane;
    struct { ... } size;
    long       data_offset;
    struct { ... } frame;
    Uint32     t0;
} Pico_Layer_Video;
```

`width`/`height` are redundant with `base.view.dim.w` / `base.view.dim.h`.

## Field mapping

| old field       | new access                 |
|-----------------|----------------------------|
| `y_plane`       | `plane.y`                  |
| `u_plane`       | `plane.u`                  |
| `v_plane`       | `plane.v`                  |
| `y_size`        | `size.y`                   |
| `uv_size`       | `size.uv`                  |
| `frame_size`    | `size.frame`               |
| `width`         | `base.view.dim.w`          |
| `height`        | `base.view.dim.h`          |
| `fps`           | `fps`                      |
| `total_frames`  | `frame.total`              |
| `cur_frame`     | `frame.cur`                |
| `done`          | `frame.done`               |
| `data_offset`   | `data_offset`              |
| `t0`            | `t0`                       |
| `fp`            | `fp`                       |
| `layer`         | removed (video-layer.md)   |

## Changes

All changes are internal to video functions. Every `vs->y_plane` becomes
`vs->plane.y`, every `vs->cur_frame` becomes `vs->frame.cur`, etc.
No public API changes.

## Order

Apply **after** video-files.md, **before** video-layer.md.
The `Pico_Layer base` field shown in the proposed layout is a preview of
video-layer.md — this plan only restructures the existing flat fields into
anonymous structs.
