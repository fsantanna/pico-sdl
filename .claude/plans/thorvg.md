# Plan: ThorVG Integration into pico-sdl

## Goal

Replace SDL2_gfx (and optionally SDL2_ttf) with ThorVG as the primary
vector rendering engine. Add SVG support. Gain anti-aliasing, gradients,
bezier paths, variable stroke, per-shape transforms/opacity, and more.

## Architecture

- **ThorVG as primary renderer, SDL2 for windowing/input/audio/texture**
- ThorVG SwCanvas renders to `uint32_t[]` buffer (ARGB8888)
- Buffer uploaded to SDL_Texture (SDL_PIXELFORMAT_ARGB8888)
- Reuses existing layer/hash infrastructure
- Immediate mode: persistent canvas with `draw(false)` (no clear),
  shapes accumulate; `pico_output_clear()` clears canvas
- Each layer gets its own ThorVG canvas + buffer

## Colorspace

- `TVG_COLORSPACE_ARGB8888S` = `SDL_PIXELFORMAT_ARGB8888` (straight alpha)

## Dependencies

- **Drop**: `SDL2_gfx` (immediately), `SDL2_ttf` (Phase 3)
- **Add**: ThorVG static lib (~150KB), link with `-lthorvg -lstdc++`
- **Keep**: SDL2, SDL2_image, SDL2_mixer

## Phases

### Phase 1 — Foundation + SVG + Replace SDL2_gfx primitives

1. **Build system**: integrate ThorVG as static lib (Meson or vendored
   source); update `pico-sdl` shell script to link `-lthorvg -lstdc++`;
   drop `-lSDL2_gfx`
2. **ThorVG init/term**: call `tvg_engine_init(TVG_ENGINE_SW)` in
   `pico_init(1)` and `tvg_engine_term()` in `pico_init(0)`
3. **Canvas per layer**: each `Pico_Layer` gets a ThorVG SwCanvas +
   `uint32_t* buffer`; main layer canvas sized to `PICO_DIM_LOG`
4. **Rendering pipeline**: after ThorVG draws shapes to buffer, upload
   buffer to SDL_Texture via `SDL_UpdateTexture()`; existing
   `_pico_output_present()` displays texture unchanged
5. **Replace primitives** (drop SDL2_gfx):
   - `pico_output_draw_rect` → `tvg_shape_append_rect`
   - `pico_output_draw_oval` → `tvg_shape_append_circle`
   - `pico_output_draw_line` → `tvg_shape_move_to` + `tvg_shape_line_to`
   - `pico_output_draw_tri` → `tvg_shape_move_to` + 3x `line_to` + close
   - `pico_output_draw_poly` → `tvg_shape_move_to` + N x `line_to` + close
   - `pico_output_draw_pixel` → 1x1 rect or direct buffer write
   - `pico_output_draw_pixels` → batch of 1x1 rects or direct buffer
6. **SVG support**: auto-detect `.svg` extension in
   `pico_output_draw_image()`; load with `tvg_picture_load()`;
   rasterize to buffer; upload as SDL_Texture; cache via existing
   layer/hash system
7. **Fill/Stroke**: `PICO_STYLE_FILL` → `tvg_shape_set_fill_color`;
   `PICO_STYLE_STROKE` → `tvg_shape_set_stroke_color` +
   `tvg_shape_set_stroke_width(1)`
8. **Alpha**: apply `S.alpha` via ThorVG paint opacity
9. **Immediate mode adaptation**: each draw call creates a ThorVG shape,
   pushes to canvas, calls `tvg_canvas_draw(false)` (no clear),
   then syncs buffer to SDL_Texture
10. **Update tests**: regenerate `tst/asr/` reference images (AA will
    change pixel output); existing tests validate same visual results

### Phase 2 — New capabilities (stroke width, rotation, gradients, bezier)

1. **Stroke width**: new API `pico_set_stroke(int width)` or extend
   existing style
2. **Per-shape rotation**: apply current view rotation/flip to ThorVG
   paint transforms via `tvg_paint_rotate()` / `tvg_paint_scale()`
3. **Rounded rects**: extend `pico_output_draw_rect` with optional
   corner radius (e.g., `tvg_shape_append_rect(rx, ry)`)
4. **Gradients**: new API for linear/radial gradient fills
5. **Bezier paths**: new API `pico_output_draw_path()` with
   `tvg_shape_cubic_to()`

### Phase 3 — Replace SDL_ttf with ThorVG text

1. Replace `TTF_RenderText_Solid` with ThorVG font loading
   (`tvg_font_load`) and text rendering
2. Gains: gradient/stroke/rotation on text
3. Tradeoff: no hinting (acceptable — pico-sdl uses large text,
   already uses embedded `tiny_ttf.h`)
4. Drop `-lSDL2_ttf` from link line

### Phase 4 — Advanced features (optional, low priority)

1. Per-shape opacity
2. Blend modes
3. Masking / clipping
4. Lottie animation support
5. TVG binary format
6. Blur/shadow/tint effects

## Key Files to Modify

| File              | Changes                                       |
|-------------------|-----------------------------------------------|
| `src/pico.c`      | Core: init, draw functions, canvas management |
| `src/pico.h`      | New types/APIs (stroke, path, gradient)       |
| `pico-sdl`        | Build: link flags, drop SDL2_gfx              |
| `Makefile`        | Build ThorVG, update link                     |
| `check.h`         | Possibly unchanged                            |
| `tst/asr/*.png`   | Regenerate all (AA changes pixels)            |
| `src/video.h`     | Unchanged (raster pipeline stays)             |

## Risks / Notes

- **Immediate-mode-to-scene-graph**: main adaptation challenge. Each
  pico draw call must create shape, push, draw(false), sync. Performance
  negligible at 100x100 canvas.
- **Test image regeneration**: AA rendering will differ from SDL2_gfx
  aliased output. All `tst/asr/` images must be regenerated. This is
  expected and desired (better quality).
- **C++ linkage**: ThorVG is C++ but has a C API (`thorvg_capi.h`).
  pico-sdl stays pure C; only needs `-lstdc++` at link time.
- **Grid overlay**: `_show_grid()` currently uses
  `pico_output_draw_line` / `pico_output_draw_text` — these will
  automatically use ThorVG after replacement.

## Status

- [ ] Phase 1: Foundation + SVG + Replace SDL2_gfx
- [ ] Phase 2: New capabilities
- [ ] Phase 3: Replace SDL_ttf
- [ ] Phase 4: Advanced features

## Pending Actions

- Decide how to vendor/build ThorVG (Meson subproject vs. pre-built
  static lib vs. source files copied into repo)
- Decide on ThorVG version to target
- Confirm SVG test assets
- Discuss Phase 1 implementation order
