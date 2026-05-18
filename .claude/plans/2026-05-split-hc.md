# split *.hc -> *.[hc]

## Goal

Split each `src/*.hc` (except `realm.hc`, which is external) into
proper `.h`+`.c` modules with **separate translation units**,
packaged as `src/libpico-sdl.a`.

Decisions:
- `G` global: extern in a shared `state.h`.
- Cross-module helpers: keep static where possible;
  only export what's actually called from another module.
- Build: produce static lib `libpico-sdl.a`;
  the `pico-sdl` script links user code against it.

## File map

| current     | new `.h`                                            | new `.c` |
|-------------|-----------------------------------------------------|----------|
| —           | state.h : `G` struct + `extern G;`                  | (G defined in `pico.c`) |
| aux.hc      | aux.h    : exported `_raw_*`, `_rel_*`, `_rnd_*`    | aux.c    |
| colors.hc   | colors.h : public (included by `pico.h`)            | colors.c |
| geom.hc     | geom.h   : `pico_cv_*`, `pico_vs_*`, `pico_in_*`    | geom.c   |
| layers.hc   | layers.h : `Pico_Layer*` types + exported `_pico_layer_*`, `_layer_attach`, `_pico_output_draw_layer(s)` | layers.c |
| mem.hc      | mem.h    : alloc-ctx structs + exported `_alloc_*`, `_free_*`, `_layer_new` | mem.c |
| video.hc    | video.h  : `Pico_Layer_Video` + exported `_alloc_layer_video`, `_free_layer_video`, public `pico_*_video` | video.c |

## state.h

- struct type lifted from `pico.c:35-56` (current `G`).
- `extern struct PicoState G;` (or equivalent typedef).
- Drops `static` on the `G` definition in `pico.c`.
- Includes: `<SDL2/SDL.h>`, `layers.h` (needs `Pico_Layer`).

## Static audit

| symbol                                                | scope after split   | reason                                  |
|-------------------------------------------------------|---------------------|-----------------------------------------|
| `_raw_*`, `_rel_*`, `_rnd_*`                          | exported (aux.h)    | called from pico,geom,layers,mem,video  |
| `_f_rat`                                              | static in aux.c     | aux-internal                            |
| `_dim/pos/rect_root_to/fr`                            | static in geom.c    | geom-internal                           |
| `_root_of`, `_root_pos`, `_root_rect`                 | static in geom.c    | geom-internal                           |
| `_pico_layer_name`, `_layer_attach`                   | exported (layers.h) | cross-module                            |
| `_pico_layer_{pixmap,image,text}`                     | exported (layers.h) | cross-module                            |
| `_pico_output_draw_layer`, `_pico_output_draw_layers` | exported (layers.h) | cross-module                            |
| `_show_tile`, `_show_grid`                            | static in layers.c  | layers-internal                         |
| `_alloc_*`, `_free_*`, `_layer_new`                   | exported (mem.h)    | used by pico,layers,video               |
| `_y4m_parse_header`, `_y4m_read_frame`, `_y4m_update_texture` | static in video.c | video-internal                  |
| `_alloc_layer_video`, `_free_layer_video`             | exported (video.h)  | called from mem.c                       |
| `_pico_guard`, `_tex_create`, `_font_get`             | exported via state.h (or `pico_internal.h`) | used by many modules    |
| `_pico_output_present`                                | exported            | called from many modules                |
| `_pico_keyboard`, `sdl_to_pico`, `pico_event_handler` | stay static in pico.c | pico.c-only                           |
| `_pico_output_sound_cache`                            | static in pico.c    | pico.c-only                             |

## pico.c changes

- Define `G` (drop `static`); type lives in `state.h`.
- Replace `#define X_C / #include "x.hc"` ritual with plain
  `#include "x.h"`.
- Drop inline `_alloc_*` / `_free_*` / `_layer_*` definitions
  (now in `mem.c` / `layers.c` / `video.c`).
- Keep: init/quit, get/set, input event loop, output draw
  primitives, and the helpers marked "stay static in pico.c"
  above.

## pico.h change

- `#include "colors.hc"` -> `#include "colors.h"`.

## Build system

| file              | change |
|-------------------|--------|
| Makefile          | add `lib` target: compile `src/{pico,aux,colors,geom,layers,mem,video}.c` to `.o`; archive into `src/libpico-sdl.a`. `tests` depends on `lib`. `realm:` target unchanged. |
| pico-sdl (script) | replace `"$PICO/src/pico.c"` arg with `-L "$PICO/src" -lpico-sdl`. Ensure `make lib` ran. |
| valgrind.supp     | recheck `src:pico.c:117` (SDL_Init line) and update if shifted. |

## Order of work

1. Create `state.h`; promote `G` to extern.
2. Split `colors.hc` (smallest, public).
3. Split `aux.hc` (required by everything else).
4. Split `layers.hc`, `mem.hc`, `video.hc` together
   (interdependent).
5. Split `geom.hc`.
6. Rewrite Makefile + `pico-sdl` script for the static lib.
7. `make tests` green; update `valgrind.supp` line if needed.

## Risks

- `G` init order: fine (single struct in pico.c, zero-init then
  assigned in `pico_init`).
- A missed cross-module call after static-promotion becomes a
  link error -- caught immediately.
- `tst/vs.c:251` has a stale comment referencing `layers.hc`;
  leave or retarget to `layers.c`.

## Verification

- `make lib && make tests` green.
- `nm src/libpico-sdl.a | grep ' T '` shows only intended exports.

## Progress

- [x] state.h
- [x] colors split
- [x] aux split
- [x] layers / mem / video split
- [x] geom split
- [x] Makefile + pico-sdl script
- [x] valgrind.supp line check (117 → 91)

## Follow-ups (beyond original plan)

- [x] PICO_TESTS runtime via `getenv` (drop `-DPICO_TESTS`)
- [x] naming convention: intra-module statics drop `_pico_` prefix;
      cross-module gets `_pico_<module>_*` (then partly walked back
      for the aux module to keep names short)
- [x] extract `output.c` from pico.c
- [x] extract `input.c` from pico.c
- [x] extract `get-set.c` from pico.c
- [x] move `pico_layer_*` public wrappers from pico.c to layer.c
- [x] absorb per-module headers into single `_pico.h` umbrella;
      delete state.h/aux.h/layers.h/mem.h/video.h
- [x] rename `layers.c` -> `layer.c`; `_pico_layers_*` -> `_pico_layer_*`;
      `_pico_layers_draw` -> `_pico_layer_output`
- [x] merge `aux.c` into `geom.c`; make `_raw_*`/`_rel_*`/`_rnd_*`
      static intra-geom (drop `_pico_` prefix)
- [x] add `_pico_abs_*` sugar (rnd of raw); migrate ~27 call sites
- [x] add `_pico_mode_rect` (rel of raw) -> promoted to
      `pico_cv_mode_*` -> removed entirely (polymorphism via
      `pico_cv_*(NULL, &to, NULL, &fr)` covers same-layer mode conv)
- [x] fix 6 silent float->int truncations (wrap raw in rnd/abs)
- [x] inline `events.h`, `anchors.h`, `colors.h` into pico.h;
      drop the `PICO_ANCHORS_C` ritual; delete those 3 headers
- [x] nm audit: all underscore exports are `_pico_*` (no leaks)

## Final layout

`src/`:
- public: `pico.h`, `keys.h`, `tiny_ttf.h`
- internal: `_pico.h`
- external: `realm.hc`
- TUs: `aux is merged into geom`, `colors.c`, `geom.c`, `get-set.c`,
       `input.c`, `layer.c`, `mem.c`, `output.c`, `pico.c`, `video.c`
- artifact: `libpico-sdl.a`

`pico.c` is now small (~250 lines): init/quit + the `_pico_guard`,
`_pico_tex_create`, `_pico_font_get` helpers + anchor constants.
