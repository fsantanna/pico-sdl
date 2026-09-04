# gfx-alpha

- bug: SDL2_gfx `*RGBA` primitives set the renderer blend mode
    - `NONE` when `a==255`, `BLEND` otherwise
    - never restored
    - opaque oval/poly/tri leaves `NONE` for later draws
    - reported by kboltiz (SNKRX battle overlay)

# Repro

- `tst/bug-gfx-alpha.c`
    - left: translucent rect over red, blends
    - right: opaque oval, then same rect, solid white

# Fix

- `src/output.c`
    - helper `_output_gfx_restore`: reset to `SDL_BLENDMODE_BLEND`
    - call after gfx in `draw_oval`, `draw_poly`, `draw_tri`

# Pending

- run `tst/bug-gfx-alpha.c` before/after
- `make tests`
