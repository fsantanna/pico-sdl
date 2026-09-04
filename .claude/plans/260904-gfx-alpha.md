# gfx-alpha

- bug: SDL2_gfx `*RGBA` primitives set the renderer blend mode
    - `NONE` when `a==255`, `BLEND` otherwise
    - never restored
    - opaque oval/poly/tri leaves `NONE` for later draws
    - reported by kboltiz (SNKRX battle overlay)

# Repro

- `tst/bug-gfx-alpha.c` (`check.h`, in `make tests`)
    - left: translucent rect over red, blends
    - right: opaque oval, then same rect, solid white

# Fix

- `src/output.c`
    - helper `_output_gfx_restore`: reset to `SDL_BLENDMODE_BLEND`
    - call after gfx in `draw_oval`, `draw_poly`, `draw_tri`

# Pending

- `make int T=bug-gfx-alpha`, then `make gen T=bug-gfx-alpha`
- `make tests`
