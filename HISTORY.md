v0.8 (xxx/xx)
-------------

Modifications (C API only, Lua not yet ported):

- Resources are explicit handles with alloc/free (realms removed):
    - `Pico_Layer*`: `pico_layer_*` constructors return a handle (was
      a key) and take no `key`; release with `pico_layer_free`
    - `Pico_Font*`: `pico_font_load`, `pico_font_free`
    - `Pico_Sound*`: `pico_sound_load`, `pico_sound_free`
    - every function that received a layer name receives a
      `Pico_Layer*` (`pico_set_layer`, `pico_output_draw_layer`,
      `pico_output_screenshot`, `pico_get/set_mouse`,
      `pico_get/set_video`, `pico_cv_*`, `pico_vs_*`)
    - `pico_layer_world`, `pico_layer_window`: root layer handles
      (were the names `"world"`, `"window"`)
- `pico_open(win)`, `pico_close()`: replace `pico_init(on)`
    - `pico_open` receives the initial window state (`NULL` for
      defaults), e.g., to start hidden
    - `pico_close` frees all handles still alive
- `pico_output_draw_text`: renamed from `pico_output_draw_text_fix`
- `pico_set_pencil_font`, `pico_output_sound`, `pico_get_video`:
  receive a handle
    - `pico_set_pencil_font_path`, `pico_output_sound_path`,
      `pico_get_video_path`: receive a path (loaded once, released at
      `pico_close`)

Removals (C API only):

- `pico_push`, `pico_pop`, `pico_unique`
- all `*_mode` variants (`'!'`, `'='`, `'~'`)
- `pico_output_draw_text_dyn`: use `pico_output_draw_text` or a
  `pico_layer_text` handle
- `pico_output_draw_pixmap`: use `pico_layer_pixmap`

Fixes:

- quit event:
    - clean shutdown before exit (was skipping teardown)
    - `pico.input.loop`: just escapes the loop (was exiting the process)

v0.7 (aug/26)
-------------

Additions:

- `pico.unique`: unique monotonic id
- `pico.get/set.aids`: enable/disable aids

Modifications:

- `pico.output.draw.text.{fix,dyn}`: sufix is mandatory
- Tiles:
    - require positioning mode
    - new combinations between `dim x tile`
- Layers:
    - defaults to transparent background (was black)
    - constructor:
        - new table format arguments (was positional)
        - text `dim` argument (was height)
        - returns the layer key (was void)
        - `key` is optional (defaults to unique `/unique/N`)
    - target:
        - detached assumes current layer as parent
        - both attached/detached with lazy evaluation (w/h=0) at request time

Fixes:

- window clip: was not clipping at all
- text rendering: now uses reasonable native height
- layer attach: parent texture must be targetable
- event flooding: generate tick regardless

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

- `0.6-3`: check `realm_put`,  don't check layers limits (ignore)
- `0.6-2`: fix clip in expert mode

v0.5 (may/26)
-------------

Additions:

- Layers as a tree hierarchy:
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
