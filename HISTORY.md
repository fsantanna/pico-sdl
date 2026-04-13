v0.4 (???/26)
-------------

Additions:
    - CLI options: `--version`, `--help`
    - window mode 'w': absolute window positioning
    - tile mode '#':
        - visual grid
        - init in `pico_layer_empty`
    - `pico_*_color_clear_alpha`: get/set clear color with alpha
    - `pico_color_hex`: convert hex integer (0xRRGGBB) to color
    - `pico_cv_*`:
        - `pico_cv_pos_rel_win`, `pico_cv_pos_win_rel`
        - `pico_cv_dim_abs_rel`, `pico_cv_dim_rel_rel`

Modifications:
    - `pico_get_mouse`: accepts `rect` for relative positioning
    - `pico_set_mouse`: sets mouse position to simulate movement

v0.3 (mar/26)
-------------

Additions:
    - layers: empty, buffer, image, text
        - independent views for compositing
        - `pico.layer.sub`: sub-layers from crop regions (sprite sheets)
        - `pico.layer.images`: batch sub-layer creation from image
    - tiles: `#` mode for grid-based positioning
    - colors: `'%'` mode, `pico_color_alpha`, `pico.color.mix`
    - `pico.cv.*`: all combinations
        - pos, rect, dim
        - rel->abs, abs->rel, rel->rel
    - `pico.push/pop`: save/restore drawing state
    - `pico.set {}`: all-at-once setter for Lua
    - video: step-based playback

Modifications:
    - view:
        - only handles world, window goes to pico_window_*
        - handles grid, target, source, flip, rotate, clip
        - target, source, clip stored as relative
    - rotate and flip: moved from global state to per-layer show
    - renamed `ref` -> `base` parameter in conversion functions
    - renamed `pico.get.ticks` -> `pico.get.now`
    - `pico.set.expert`: accepts FPS (0, -1/true, N>0)
    - `pico.input.*`: all inputs return `dt` (ms elapsed)

Removals:
    - colors: (r,g,b) three-argument form in Lua

Documentation:
    - guide to pico-lua: `lua/doc/guide.md`

v0.2 (jan/26)
-------------

- (no history)

v0.1 (nov/25)
-------------

- (no history)

