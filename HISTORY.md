v0.5 (may/26)
-------------

Additions:
    - layers as a hierarchy:
        - window is now a layer ("window"); world its child ("world")
        - `up` (parent) parameter on all layer creators
        - `pico_output_draw_layer`: draw any layer onto current
        - `pico_output_draw_layers`: composite the full hier
            - auto-called by `pico_output_present` in non-expert mode
        - per-layer screenshot
    - `pico.layer.pixmap`: layer from RGBA pixel data
    - `pico.layer.empty`: tile cols x rows + tile size at creation
    - layer creators: optional trailing `rect` sets scene.dst
    - layer realm modes (`!` excl, `=` shared, `~` replace):
        - `pico_layer_*_mode` variants
    - bulk getters/setters via structs:
        - `Pico_Layer_Pencil`, `Pico_Layer_Effect`,
          `Pico_Layer_Scene`, `Pico_Window`
    - `pico.in.*` (rect, pos, dim): compose child onto parent rect
    - `pico.vs.*`: collision with layer args, flexible parsing,
      added `rect_pos`, layer bounds as default rect
    - `pico.cv.*` redesigned: `(L_to, to, L_fr, fr)` with
      ancestor/descendant projection
    - `pico_set_mouse`: warp cursor (round-trips with get)
    - mouse get/set: optional layer arg, full anchor support
    - `pico_color_hex`: `0xRRGGBB` / `0xRRGGBBAA` -> color
    - embedded DejaVu Sans font (no external file required)
    - tile grid overlay (per-layer)
    - CLI: `--version`, `--help` for `pico-sdl` and `pico-lua`

Modifications:
    - renames (breaking):
        - `view`   -> `scene`   (clip, dim, dst, src, tile, clear)
        - `show`   -> `effect`  (alpha, color, flip, grid, rotate)
        - `draw`   -> `pencil`  (color, font, style)
        - `target` -> `dst`     (C only; Lua keeps `target`)
        - `source` -> `src`     (C only; Lua keeps `source`)
        - `keep`   -> `clear`   (inverted semantics)
        - `root`   -> `world`   (predefined layer name)
        - `parent` -> `up`      (layer creation)
        - `pico.layer.buffer` -> `pico.layer.pixmap`
    - public C API: pointer args/out-params -> plain values
    - `Pico_Rel_{Rect,Dim,Pos}`: flattened, `up` chain removed
    - state get/set: no more optional `layer` arg
      (use `pico_set_layer` to switch)
    - `pico_set_layer`: returns previous layer name
    - `pico_set_expert(on, fps) -> ms`: cleaner three-mode return
      (-1 block, 0 immediate, N>0 frame period)
    - `pico_get_mouse`: takes `Pico_Rel_Pos*` (mode + anchor)
    - `Pico_Window` slimmed to `{fs, show, title}`;
      color/dim moved to its scene
    - colors: unified always-RGBA (`Pico_Color_A` removed)
    - `pico_get_image` / `pico_get_video`: `dim`/`rect` template
      completes missing `w` or `h` from aspect ratio

Removals:
    - `pico.push` / `pico.pop` (state stack)
    - `pico.set { ... }` all-at-once setter
    - `Pico_Color_A` type
    - `_color_clear` / `_color_clear_alpha` (replaced by `scene.clear`)
    - `'w'` window-pixel mode (window is now a regular layer)
    - `PICO_LAYER_WINDOW` / `PICO_LAYER_WORLD` enums (use strings)
    - `Pico_Layer.parent` (not needed at runtime)

Documentation:
    - `lua/doc/api.md`: full API reference (types + operations)
    - `lua/doc/guide.md`: expanded chapters
      (events, layers, tiles, anchors, animations, hierarchy)
    - `lua/doc/rects.lua`, `lua/doc/anims.lua`: runnable examples
    - embedded result images (`res/`, `lua/doc/img/`)

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

