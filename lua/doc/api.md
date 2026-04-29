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
- **Dim**: `{ ['!'|'%'|'#'], w: number, h: number, [up: Rect] }`
- **Event**: `{ tag: string, ... }`
    - `{ tag='quit' }`
    - `{ tag='win.resize', w: integer, h: integer }`
    - `{ tag='key.dn'|'key.up',
        key: string, ctrl: boolean, shift: boolean, alt: boolean }`
    - `{ tag='mouse.motion'|'mouse.button.dn'|'mouse.button.up',
        '!', x: integer, y: integer,
        left: boolean, right: boolean, middle: boolean }`
- **Flip**: `'none'` | `'horizontal'` | `'vertical'` | `'both'`
- **Pos**: `{ x: number, y: number [,'!'|'%'|'#', anchor: Anchor, up: Rect] }`
- **Rect**: `{ x: number, y: number, w: number, h: number [,'!'|'%'|'#', anchor: Anchor, up: Rect] }`
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
    - **pico.cv.dim**: Converts relative dimensions to absolute.
        - `pico.cv.dim (dim: Dim [, base: Rect]) -> Dim`
        - `pico.cv.dim (dim: Dim, to: Dim [, base: Rect])`
    - **pico.cv.pos**: Converts relative position to absolute.
        - `pico.cv.pos (pos: Pos [,base: Rect]) -> Pos`
    - **pico.cv.rect**: Converts relative rectangle to absolute.
        - `pico.cv.rect (rect: Rect [, base: Rect]) -> Rect`
- **pico.get**
    - **pico.get.draw**: Gets draw configuration.
        - `pico.get.draw () -> { color: Color, font: string?, style: 'fill'|'stroke' }`
    - **pico.get.image**: Gets image dimensions.
        - `pico.get.image (path: string [, dim: Dim]) -> Dim`
    - **pico.get.keyboard**: Gets keyboard modifier state.
        - `pico.get.keyboard () ->
          { key: string, ctrl: boolean, shift: boolean, alt: boolean }`
    - **pico.get.layer**: Gets current layer name.
        - `pico.get.layer () -> string?`
        - Returns `nil` for main layer
    - **pico.get.mouse**: Gets mouse position and button state.
        - `pico.get.mouse ([mode: string]) -> Mouse`
        - Returns `{mode, x=N, y=N, left=bool, right=bool, middle=bool}`
        - Mode: `'!'` pixels, `'%'` percentage, `'#'` tiles, `'w'` window
          (default from `pico.set.mouse`)
    - **pico.get.now**: Gets milliseconds since initialization.
        - `pico.get.now () -> integer`
    - **pico.get.text**: Gets text dimensions.
        - `pico.get.text (text: string, dim: Dim) -> Dim`
    - **pico.get.video**: Gets video information.
        - `pico.get.video (path: string [, rect: Rect]) -> Video`
    - **pico.get.show**: Gets show configuration.
        - `pico.get.show () -> { alpha: integer, color: Color, flip: Flip, grid: boolean, keep: boolean, rotate: Rotation }`
    - **pico.get.view**: Gets view configuration.
        - `pico.get.view () -> { dim: Dim, tile: Tile, target: Rect, source: Rect, clip: Rect }`
    - **pico.get.window**: Gets window configuration.
        - `pico.get.window () -> { color: Color, dim: Dim, fullscreen: boolean, show: boolean, title: string }`
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
    - **pico.layer.empty**: Creates an empty layer.
        - `pico.layer.empty ([mode,] up: string?, key: string,
          dim: Dim [, tile: Tile])`
    - **pico.layer.image**: Creates a layer from an image file.
        - `pico.layer.image ([mode,] up: string?, key: string?, path: string)`
        - If `key` is omitted, uses `"/image/<path>"` as layer name.
    - **pico.layer.pixmap**: Creates a layer from a pixmap.
        - `pico.layer.pixmap ([mode,] up: string?, key: string,
          dim: Dim, pixmap: {{Color}})`
        - Pixmap is copied; pointer-based caching not possible.
    - **pico.layer.text**: Creates a layer from text.
        - `pico.layer.text ([mode,] up: string?, key: string,
          height: integer, text: string)`
    - **pico.layer.video**: Creates a layer from a video file.
        - `pico.layer.video ([mode,] up: string?, key: string?, path: string)`
    - **pico.layer.sub**: Creates a sub-layer from a crop region of a parent.
        - `pico.layer.sub ([mode,] up: string?, key: string,
          parent: string, crop: Rect)`
        - Parent must exist and cannot be a sub-layer itself.
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
    - **pico.set.draw**: Sets draw configuration.
        - `pico.set.draw (cfg: { [color: Color], [font: string?], [style: 'fill'|'stroke'] })`
    - **pico.set.expert**: Toggles expert mode.
        - `pico.set.expert (on: boolean [, fps: integer|boolean]) -> integer`
        - fps: `nil`/`false` = wait forever, `true` = as fast as possible, `N>0` = fixed FPS
        - Returns frame period in ms: `-1` = block forever, `0` = immediate, `N>0` = frame period
    - **pico.set.show**: Sets show configuration.
        - `pico.set.show (cfg: { [alpha: integer], [color: Color], [flip: Flip], [grid: boolean], [keep: boolean], [rotate: Rotation] })`
    - **pico.set.video**: Sets video frame.
        - `pico.set.video (name: string, frame: integer) -> boolean`
    - **pico.set.view**: Sets view configuration.
        - `pico.set.view (cfg: { [dim: Dim], [source: Rect], [clip: Rect], [target: Rect], [tile: Tile] })`
        - `tile` sets tile size in pixels (required when `dim` mode is `'#'`)
    - **pico.set.window**: Sets window configuration.
        - `pico.set.window (cfg: { [color: Color], [dim: Dim], [fullscreen: boolean], [show: boolean], [title: string] })`
    - **pico.set.layer**: Switches to a layer.
        - `pico.set.layer (name: string?)`
        - `nil` switches to main layer
    - **pico.set.mouse**: Sets mouse cursor position.
        - `pico.set.mouse (pos: Pos)`
- **pico.vs**
    - **pico.vs.pos_rect**: Collision between position and rectangle.
        - `pico.vs.pos_rect (pos: Pos, rect: Rect) -> boolean`
    - **pico.vs.rect_rect**: Collision between two rectangles.
        - `pico.vs.rect_rect (r1: Rect, r2: Rect) -> boolean`
