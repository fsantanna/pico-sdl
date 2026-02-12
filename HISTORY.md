v0.3 (???/???)
--------------

Additions:
    - layers: empty, buffer, image, text
        - independent views for compositing
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

