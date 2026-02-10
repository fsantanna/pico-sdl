# video-files.md — Move video code to video.[hc]

## Problem

All video code lives in `pico.c` (~300 lines), interleaved with the rest of
the library. As the video subsystem grows, this clutters the main file.

## Goal

Extract video code into `src/video.h` and `src/video.c`.

## Depends on

None — this is a mechanical extraction, applied first.

## Files

### src/video.h

Internal header (not user-facing). Contains:

- `Pico_Layer_Video` struct definition
- Declarations for internal helpers needed by pico.c:
    - `_pico_video_clean(Pico_Layer_Video*)` — cleanup for hash eviction

Declarations for public functions (already in `pico.h`):
- `pico_layer_video`
- `pico_set_video`
- `pico_output_draw_video`
- `pico_get_video`

### src/video.c

Contains all video implementation:

- `_y4m_parse_header` — Y4M header parser
- `_y4m_read_frame` — read single frame
- `_y4m_update_texture` — upload YUV to SDL texture
- `pico_layer_video` — create video layer
- `pico_set_video` — frame seeking
- `pico_output_draw_video` — all-in-one draw
- `pico_get_video` — query video properties

### Internal dependencies

`video.c` needs access to pico.c internals:

| symbol                    | usage                          |
|---------------------------|--------------------------------|
| `G.ren`                   | create textures, render        |
| `G.hash`                  | hash table for caching         |
| `Pico_Key` / key helpers  | hash key construction          |
| `_pico_output_draw_layer` | drawing the video layer        |
| `_sdl_dim`                | dimension calculations         |
| `S.layer`                 | current layer context          |

Options to expose these:
1. **Internal header** (`pico_int.h`): move shared internals there.
   Both `pico.c` and `video.c` include it.
2. **Minimal forward declarations** in `video.h`: declare only what
   `video.c` needs, keep `G`/`S` in `pico.c` and pass via parameters.

Option 2 is cleaner but requires refactoring video functions to take
explicit parameters instead of accessing globals.

Recommended: **option 1** (internal header) for now — minimal disruption,
can refactor globals later.

## Changes

### New files

| file          | content                                      |
|---------------|----------------------------------------------|
| `src/video.h` | `Pico_Layer_Video` type, internal declarations|
| `src/video.c` | all video implementation                     |

### Modified files

| file         | change                                        |
|--------------|-----------------------------------------------|
| `src/pico.c` | remove video section (~lines 1888-2199), `#include "video.h"`, update `_pico_hash_clean` to call `_pico_video_clean` |
| `src/pico.h` | unchanged (public API declarations stay here) |
| `pico-sdl`   | add `src/video.c` to compilation              |
| `Makefile`   | add `src/video.c` to test compilation         |

## Build

```bash
gcc -Wall -g -o output program.c src/pico.c src/video.c \
    -I src \
    -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
```
