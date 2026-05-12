# API: pico-lua

## Types

In alphabetical order:

- **Anchor**: `string` | `{ x: number, y: number }`
    - String: `'C'`, `'NW'`, `'N'`, `'NE'`, `'E'`, `'SE'`, `'S'`, `'SW'`, `'W'`
- **Color**: `table` | `integer` | `string`
    - Table: `{ ['!'|'%'], r: number, g: number, b: number [, a: number] }`
    - Integer: `0xRRGGBB` (24-bit, alpha=0xFF) or `0xRRGGBBAA` (32-bit)
    - String:
        `'black'`, `'white'`, `'gray'`, `'silver'`, `'red'`, `'green'`,
        `'blue'`, `'yellow'`, `'cyan'`, `'magenta'`, `'orange'`, `'purple'`,
        `'pink'`, `'brown'`, `'lime'`, `'teal'`, `'navy'`, `'maroon'`,
        `'olive'`
- **Dim**: `{ ['!'|'%'|'#'], [w: number], [h: number] }`
    - Missing `w` or `h` defaults to 0 ("infer from source").
- **Event**: `{ tag: string, ... }`
    - `{ tag='quit' }`
    - `{ tag='win.resize', w: integer, h: integer }`
    - `{ tag='key.dn'|'key.up',
        key: string, ctrl: boolean, shift: boolean, alt: boolean }`
    - `{ tag='mouse.motion'|'mouse.button.dn'|'mouse.button.up',
        '!', x: integer, y: integer,
        left: boolean, right: boolean, middle: boolean }`
- **Flip**: `'none'` | `'horizontal'` | `'vertical'` | `'both'`
- **Mouse**: `{ ['!'|'%'|'#'], x: number, y: number, left: boolean, right: boolean, middle: boolean }`
- **Pos**: `{ x: number, y: number [,'!'|'%'|'#', anchor: Anchor] }`
- **Rect**: `{ x: number, y: number, [w: number], [h: number] [,'!'|'%'|'#', anchor: Anchor] }`
    - Missing `w` or `h` defaults to 0 ("infer from source").
- **Rotation**: `{ angle: integer, anchor: Anchor }`
- **Tile**: `{ w: integer, h: integer }`
- **Video**: `{ dim: Dim, fps: integer, frame: integer, done: boolean }`

## Operations

In alphabetical order:

- **pico.color**
    - **pico.color.alpha**: Returns a copy of the color with alpha set.
        - `pico.color.alpha (clr: Color, a: integer) -> Color`
    - **pico.color.darker**: Makes a color darker.
        - `pico.color.darker (clr: Color, pct: number) -> Color`
    - **pico.color.lighter**: Makes a color lighter.
        - `pico.color.lighter (clr: Color, pct: number) -> Color`
    - **pico.color.mix**: Mixes two colors.
        - `pico.color.mix (c1: Color, c2: Color) -> Color`
- **pico.cv**
    - Chain-walk projection between cur and a named ancestor layer
      (via `hier.up`). The named end is always given; the other end is
      cur. `layer = nil` collapses to cur (mode/anchor conversion only,
      no projection). Target must be cur or an ancestor of cur.
    - **pico.cv.dim.to**: Projects a dim from cur to a named ancestor.
        - `pico.cv.dim.to (layer: string?, fr: Dim, to: Dim)`
        - `to` is written in `layer`'s frame; `to.mode` controls form.
    - **pico.cv.dim.from**: Brings a dim from a named ancestor into cur.
        - `pico.cv.dim.from (layer: string?, fr: Dim, to: Dim)`
        - `fr` is interpreted in `layer`; `to` is written in cur.
    - **pico.cv.pos.to**: Projects a pos from cur to a named ancestor.
        - `pico.cv.pos.to (layer: string?, fr: Pos, to: Pos)`
    - **pico.cv.pos.from**: Brings a pos from a named ancestor into cur.
        - `pico.cv.pos.from (layer: string?, fr: Pos, to: Pos)`
    - **pico.cv.rect.to**: Projects a rect from cur to a named ancestor.
        - `pico.cv.rect.to (layer: string?, fr: Rect, to: Rect)`
    - **pico.cv.rect.from**: Brings a rect from a named ancestor into cur.
        - `pico.cv.rect.from (layer: string?, fr: Rect, to: Rect)`
- **pico.get**
    - **pico.get.pencil**: Gets pencil configuration.
        - `pico.get.pencil () -> { color: Color, font: string?, style: 'fill'|'stroke' }`
    - **pico.get.image**: Gets image dimensions.
        - `pico.get.image (path: string [, dim: Dim]) -> Dim`
    - **pico.get.keyboard**: Gets keyboard modifier state.
        - `pico.get.keyboard () ->
          { key: string, ctrl: boolean, shift: boolean, alt: boolean }`
    - **pico.get.layer**: Gets current layer name.
        - `pico.get.layer () -> string?`
        - Returns `nil` for main layer
    - **pico.get.mouse**: Gets mouse position and button state in cur's
      frame. Use `pico.cv.pos.from` / `pico.set.layer` to obtain coords
      in other frames.
        - `pico.get.mouse (mode: string) -> Mouse`
        - `mode`: `'!'` pixels, `'%'` percentage, `'#'` tiles
    - **pico.get.now**: Gets milliseconds since initialization.
        - `pico.get.now () -> integer`
    - **pico.get.text**: Gets text dimensions.
        - `pico.get.text (text: string, dim: Dim) -> Dim`
    - **pico.get.video**: Gets video information.
        - `pico.get.video (path: string [, rect: Rect]) -> Video`
    - **pico.get.effect**: Gets effect configuration.
        - `pico.get.effect () -> { alpha: integer, color: Color, flip: Flip, grid: boolean, rotate: Rotation }`
    - **pico.get.scene**: Gets scene configuration.
        - `pico.get.scene () -> { dim: Dim, tile: Tile, target: Rect, source: Rect, clip: Rect, keep: boolean }`
    - **pico.get.window**: Gets window configuration.
        - `pico.get.window () -> { fullscreen: boolean, show: boolean, title: string }`
- **pico.init**: Initializes and finalizes pico.
    - `pico.init (on: boolean)`
- **pico.input**
    - **pico.input.delay**: Freezes execution for milliseconds.
        - `pico.input.delay (ms: integer) -> integer`
        - Returns elapsed time in milliseconds (delta time)
    - **pico.input.event**: Waits for an event.
        - `pico.input.event () -> Event, integer`
        - `pico.input.event (ms: integer) -> Event?, integer`
        - `pico.input.event (filter: string) -> Event, integer`
        - `pico.input.event (filter: string, ms: integer) -> Event?, integer`
        - Returns event (or nil on timeout) and elapsed time in ms (delta time)
        - Filters: `'quit'`, `'win.resize'`, `'key.dn'`, `'key.up'`,
          `'mouse.motion'`, `'mouse.button.dn'`, `'mouse.button.up'`
    - **pico.input.loop**: Blocks on event loop until quit.
        - `pico.input.loop ()`
- **pico.layer**
    - All layer creators accept an optional mode prefix
      (`'!'`|`'%'`|`'#'`|`'='`) as the first argument.
    - `up` is the parent layer name (string) or `nil` for the main layer.
    - All creators (except `images`) accept an **optional trailing
      `rect: Rect`** that sets `scene.target` of the new layer in one
      call. For `image` and `video` it requires an explicit `key`.
    - **pico.layer.empty**: Creates an empty layer.
        - `pico.layer.empty ([mode,] up: string?, key: string,
          clear: boolean, dim: Dim [, tile: Tile])`
        - `pico.layer.empty ([mode,] up: string?, key: string,
          clear: boolean, rect: Rect [, tile: Tile])`
        - `clear`: sets `scene.clear` (true = auto-clear each frame).
        - When the 4th arg is a `Rect` (has `x` field): its
          `mode/w/h` are used as the layer `Dim`, and the full
          `Rect` is set as `scene.target` (one-call create+place).
    - **pico.layer.image**: Creates a layer from an image file.
        - `pico.layer.image ([mode,] up: string?, key: string?, path: string
          [, rect: Rect])`
        - If `key` is omitted, uses `"/image/<path>"` as layer name
          (and `rect` is not allowed in that form).
    - **pico.layer.images**: Creates sub-layer images from a reference
        "sprite sheet" image.
        - `pico.layer.images ([mode,] up: string?, key: string, path: string, t: table) -> {string}`
            - `t`:
                - **Grid**: `{ '#', w: integer, h: integer, [n: integer], [key: string] }`
                    - generates sub-layers `"<key>-01"`, `"<key>-02"`, ...
                        - the inner `key` overrides the outer `key` prefix
                    - uses left-to-right order
                    - `n` limits the count (default `w*h`)
                - **Explicit**: `{ '!', [name1: Rect, name2: Rect, ...] }`
                    - generates sub-layers `"<key>-<name>"` for each given
                        `name=crop` pair
            - Each sub-layer attaches to the same `up` parent as the image.
        - Returns the list of generated sub-layer names.
    - **pico.layer.pixmap**: Creates a layer from a pixmap.
        - `pico.layer.pixmap ([mode,] up: string?, key: string,
          pixmap: {{Color}} [, rect: Rect])`
    - **pico.layer.text**: Creates a layer from text.
        - `pico.layer.text ([mode,] up: string?, key: string,
          height: integer, text: string [, rect: Rect])`
    - **pico.layer.video**: Creates a layer from a video file.
        - `pico.layer.video ([mode,] up: string?, key: string?, path: string
          [, rect: Rect])`
        - If `key` is omitted, uses `"/video/<path>"` as layer name
          (and `rect` is not allowed in that form).
    - **pico.layer.sub**: Creates a sub-layer from a source layer.
        - `pico.layer.sub ([mode,] up: string?, key: string,
          src: string, crop: Rect [, rect: Rect])`
        - Source must exist and cannot be a sub-layer itself.
- **pico.output**
    - **pico.output.clear**: Clears screen.
        - `pico.output.clear ()`
    - **pico.output.present**: Presents buffer (expert mode only).
        - `pico.output.present ()`
    - **pico.output.screenshot**: Takes a screenshot.
        - `pico.output.screenshot ([path: string] [, rect: Rect]) -> string`
    - **pico.output.sound**: Plays sound.
        - `pico.output.sound (path: string)`
    - **pico.output.draw**
        - **pico.output.draw.image**: Draws image.
            - `pico.output.draw.image (path: string, rect: Rect)`
        - **pico.output.draw.layer**: Draws a layer onto the current layer.
            - `pico.output.draw.layer (name: string, rect: Rect)`
        - **pico.output.draw.layers**: Composites the window layer hierarchy
          (window's children, including world) onto window.tex. Called
          automatically by `pico.output.present` in non-expert mode.
            - `pico.output.draw.layers ()`
        - **pico.output.draw.line**: Draws line.
            - `pico.output.draw.line (p1: Pos, p2: Pos)`
        - **pico.output.draw.oval**: Draws oval.
            - `pico.output.draw.oval (rect: Rect)`
        - **pico.output.draw.pixel**: Draws pixel.
            - `pico.output.draw.pixel (pos: Pos)`
        - **pico.output.draw.pixels**: Draws multiple pixels.
            - `pico.output.draw.pixels (ps: {Pos})`
        - **pico.output.draw.pixmap**: Draws a pixmap of colored pixels.
            - `pico.output.draw.pixmap (name: string, pixmap: {{Color}}, rect: Rect)`
        - **pico.output.draw.poly**: Draws polygon.
            - `pico.output.draw.poly (ps: {Pos})`
        - **pico.output.draw.rect**: Draws rectangle.
            - `pico.output.draw.rect (rect: Rect)`
        - **pico.output.draw.text**: Draws text.
            - `pico.output.draw.text (text: string, rect: Rect)`
        - **pico.output.draw.tri**: Draws triangle.
            - `pico.output.draw.tri (p1: Pos, p2: Pos, p3: Pos)`
        - **pico.output.draw.video**: Draws video frame.
            - `pico.output.draw.video (path: string, rect: Rect) -> boolean`
- **pico.quit**: Pushes a quit event to terminate the application.
    - `pico.quit ()`
- **pico.set**
    - **pico.set.dim**: Sets both window and world to the same dimensions.
        - `pico.set.dim (dim: Dim)`
    - **pico.set.pencil**: Sets pencil configuration.
        - `pico.set.pencil (cfg: { [color: Color], [font: string?], [style: 'fill'|'stroke'] })`
    - **pico.set.expert**: Toggles expert mode.
        - `pico.set.expert (on: boolean [, fps: integer|boolean]) -> integer`
        - fps: `nil`/`false` = wait forever, `true` = as fast as possible, `N>0` = fixed FPS
        - Returns frame period in ms: `-1` = block forever, `0` = immediate, `N>0` = frame period
    - **pico.set.effect**: Sets effect configuration.
        - `pico.set.effect (cfg: { [alpha: integer], [color: Color], [flip: Flip], [grid: boolean], [rotate: Rotation] })`
    - **pico.set.video**: Sets video frame.
        - `pico.set.video (name: string, frame: integer) -> boolean`
    - **pico.set.scene**: Sets scene configuration.
        - `pico.set.scene (cfg: { [dim: Dim], [source: Rect], [clip: Rect], [target: Rect], [tile: Tile], [keep: boolean] })`
        - `tile` sets tile size in pixels (required when `dim` mode is `'#'`)
    - **pico.set.window**: Sets window configuration.
        - `pico.set.window (cfg: { [fullscreen: boolean], [show: boolean], [title: string] })`
    - **pico.set.layer**: Switches to a layer.
        - `pico.set.layer (name: string?)`
        - `nil` switches to main layer
    - **pico.set.mouse**: Sets mouse cursor position. `pos` is
      interpreted in cur's frame and projected to window for the SDL
      warp.
        - `pico.set.mouse (pos: Pos)`
        - `pos.mode`: `'!'` pixels, `'%'` percentage, `'#'` tiles
- **pico.vs**
    - Collision checks.
      Each side has the canonical shape `(Lx, vx)` where `Lx` is the
      name of a direct child of cur (or absent for cur itself) and
      `vx` is a `Pos` or `Rect` value relative to `Lx`.
    - Args are matched left-to-right against the 4-slot template
      `(L1, v1, L2, v2)`. Both explicit `nil`s and trailing omissions
      are accepted; a mismatched type leaves the slot empty without
      consuming the arg, so `(L1, L2)`, `(v1, v2)`, `(L1, v2)`, etc.
      are all valid shortenings.
    - When a `rect` slot is absent (both `Lx` and `vx`) it defaults to
      the bounds of cur (`scene.dst`). `pos` slots have no default and
      must always be supplied.
    - **pico.vs.pos_pos**: True if two points fall on the same pixel
      (after integer rounding in cur).
        - `pico.vs.pos_pos (L1: string?, p1: Pos, L2: string?, p2: Pos) -> boolean`
        - `p1` and `p2` are required.
    - **pico.vs.pos_rect**: True if a point is inside a rectangle.
        - `pico.vs.pos_rect (L1: string?, p1: Pos, L2: string?, r2: Rect?) -> boolean`
        - `p1` is required; `r2` defaults to cur's bounds (or `L2`'s
          bounds when `L2` is set and `r2` is absent).
    - **pico.vs.rect_pos**: True if a point is inside a rectangle
      (mirror of `pos_rect`).
        - `pico.vs.rect_pos (L1: string?, r1: Rect?, L2: string?, p2: Pos) -> boolean`
        - `p2` is required; `r1` defaults to cur's bounds (or `L1`'s
          bounds when `L1` is set and `r1` is absent).
    - **pico.vs.rect_rect**: True if two rectangles overlap.
        - `pico.vs.rect_rect (L1: string?, r1: Rect?, L2: string?, r2: Rect?) -> boolean`
        - All slots are optional; each missing rect side defaults to
          its layer's bounds (or cur's bounds when the layer is also
          absent). The 2-string form `(L1, L2)` compares the bounds
          of both child layers.
