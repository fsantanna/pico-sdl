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
    - rotation and flip: moved from global state to per-layer view
    - renamed `ref` -> `base` parameter in conversion functions
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

