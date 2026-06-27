v0.6 (jun/26)
-------------

Additions:

- `pico.push` / `pico.pop`: explicit memory scopes with bulk deallocation
- `pico.layer.screenshot`: capture layer into new layer
- `pico.color.transparent` (was missing)

Modifications:

- Layer hierarchy:
    - no more automatic clear
    - `pico.output.clear` traverses and clears

PATCHES:

- `0.6-2`: fix clip in expert mode

v0.5 (may/26)
-------------

Additions:

- layers as a tree hierarchy:
    - window <- world <- custom layers
    - `up` parameter on layer constructors
    - composite hierarchy:
        - explicit: `pico.output.draw.layers`
        - implicit: `pico.output.present` (non expert or explicit argument)
- `pico.xin.*`: nest child onto parent `Rect`
- `pico.set.mouse`: warp cursor
- `pico.color.hex`: `0xRRGGBBAA` format
- CLI: `--version`, `--help` for `pico-sdl` and `pico-lua`

Modifications:

- `{Rect,Dim,Pos}`: flattened, `up` chain removed
- Views split in three APIs:
    - `scene` (clip, dim, dst, src, tile, clear)
    - `effect` (alpha, color, flip, grid, rotate)
    - `pencil` (color, font, style)
- Layers
    - constructors: optional trailing `Rect` as scene target
    - `pico.layer.buffer` -> `pico.layer.pixmap`
    - `pico.set.layer`: returns previous layer name
    - tile grid overlay
- Getters/Setters:
    - `pico.set.expert(on, fps) -> ms`:
        - `ms`: -1 block, 0 immediate, N>0 frame period
    - `pico.get.mouse`: accepts mode or `Pos` template, and layer
    - `pico.get/set.window` (fs, show, title)
    - `pico.get.{image,video,text}`: refactor parameters and return
- `pico.output.screenshot`: accepts layer parameter
- `pico.{cv,vs}.*`: extended to handle layers

Removals:

- `'w'` window mode (now a regular layer)
- C:
    - type `Pico_Color_A`
- Lua:
    - `pico.push` / `pico.pop` (retained by layers)
    - `pico.set { ... }` (multiple pico.set.* APIs)

v0.4 (xxx/xx)
-------------

Skipped to use `v0.5` as a major update.

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
