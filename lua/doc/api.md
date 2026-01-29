# API

## Types

- **Anchor**: `string` | `{ x: number, y: number }`
    - Strings: `'C'`, `'NW'`, `'N'`, `'NE'`, `'E'`, `'SE'`, `'S'`, `'SW'`, `'W'`
- **Color**: `string` | `{ r: integer, g: integer, b: integer }`
    - Strings: `'black'`, `'white'`, `'gray'`, `'silver'`, `'red'`, `'green'`,
      `'blue'`, `'yellow'`, `'cyan'`, `'magenta'`, `'orange'`, `'purple'`,
      `'pink'`, `'brown'`, `'lime'`, `'teal'`, `'navy'`, `'maroon'`, `'olive'`
- **Color_A**: `{ r: integer, g: integer, b: integer, a: integer }`
- **Dim**: `{ w: integer, h: integer }`
- **Event**: `{ tag: string, ... }`
    - `{ tag='quit' }`
    - `{ tag='key.dn'|'key.up', key: string }`
    - `{ tag='mouse.motion'|'mouse.button.dn'|'mouse.button.up',
        '!', x: integer, y: integer, anc: string [, but: string] }`
- **Rel_Dim**: `{ '!'|'%', w: number, h: number [, up: Rel_Rect] }`
- **Rel_Pos**: `{ '!'|'%', x: number, y: number [, anc: Anchor, up: Rel_Rect] }`
- **Rel_Rect**: `{ '!'|'%', x: number, y: number, w: number, h: number
    [, anc: Anchor, up: Rel_Rect] }`
- **Abs_Rect**: `{ x: integer, y: integer, w: integer, h: integer }`

## Operations

- **pico.init**: Initializes and finalizes pico.
    - `pico.init (on: boolean)`
- **pico.quit**: Pushes a quit event to terminate the application.
    - `pico.quit ()`
- **pico.cv**
    - **pico.cv.pos**: Converts relative position to absolute.
        - `pico.cv.pos (pos: Rel_Pos [, ref: Abs_Rect]) -> Rel_Pos`
    - **pico.cv.rect**: Converts relative rectangle to absolute.
        - `pico.cv.rect (rect: Rel_Rect [, ref: Abs_Rect]) -> Rel_Rect`
- **pico.vs**
    - **pico.vs.pos_rect**: Collision between position and rectangle.
        - `pico.vs.pos_rect (pos: Rel_Pos, rect: Rel_Rect) -> boolean`
    - **pico.vs.rect_rect**: Collision between two rectangles.
        - `pico.vs.rect_rect (r1: Rel_Rect, r2: Rel_Rect) -> boolean`
- **pico.color**
    - **pico.color.darker**: Makes a color darker.
        - `pico.color.darker (clr: Color, pct: number) -> Color`
    - **pico.color.lighter**: Makes a color lighter.
        - `pico.color.lighter (clr: Color, pct: number) -> Color`
- **pico.get**
    - **pico.get.image**: Gets image dimensions.
        - `pico.get.image (path: string [, dim: Rel_Dim]) -> Dim`
    - **pico.get.text**: Gets text dimensions.
        - `pico.get.text (text: string, dim: Rel_Dim) -> Dim`
    - **pico.get.ticks**: Gets milliseconds since initialization.
        - `pico.get.ticks () -> integer`
    - **pico.get.view**: Gets view configuration.
        - `pico.get.view () -> { grid: boolean, fullscreen: boolean,
            window: Dim, world: Dim }`
- **pico.set**
    - **pico.set.alpha**: Sets alpha transparency.
        - `pico.set.alpha (a: integer)`
    - **pico.set.crop**: Sets crop area.
        - `pico.set.crop ([rect: Abs_Rect])`
    - **pico.set.expert**: Toggles expert mode.
        - `pico.set.expert (on: boolean)`
    - **pico.set.style**: Sets drawing style.
        - `pico.set.style (style: 'fill'|'stroke')`
    - **pico.set.title**: Sets window title.
        - `pico.set.title (title: string)`
    - **pico.set.view**: Sets view configuration.
        - `pico.set.view (config: { grid: boolean, fullscreen: boolean,
            window: Rel_Dim, target: Rel_Rect, world: Rel_Dim,
            source: Rel_Rect, clip: Rel_Rect })`
    - **pico.set.color**
        - **pico.set.color.clear**: Sets clear color.
            - `pico.set.color.clear (clr: Color)`
            - `pico.set.color.clear (r, g, b)`
        - **pico.set.color.draw**: Sets draw color.
            - `pico.set.color.draw (clr: Color)`
            - `pico.set.color.draw (r, g, b)`
- **pico.input**
    - **pico.input.delay**: Freezes execution for milliseconds.
        - `pico.input.delay (ms: integer)`
    - **pico.input.event**: Waits for an event.
        - `pico.input.event () -> Event`
        - `pico.input.event (ms: integer) -> Event?`
        - `pico.input.event (filter: string) -> Event`
        - `pico.input.event (filter: string, ms: integer) -> Event?`
        - Filters: `'quit'`, `'key.dn'`, `'key.up'`, `'mouse.button.dn'`
- **pico.output**
    - **pico.output.clear**: Clears window.
        - `pico.output.clear ()`
    - **pico.output.present**: Presents buffer (expert mode only).
        - `pico.output.present ()`
    - **pico.output.screenshot**: Takes a window screenshot.
        - `pico.output.screenshot ([path: string] [, rect: Rel_Rect]) -> string`
    - **pico.output.sound**: Plays sound.
        - `pico.output.sound (path: string)`
    - **pico.output.draw**
        - **pico.output.draw.buffer**: Draws array of RGBA colors.
            - `pico.output.draw.buffer (buffer: {{Color_A}}, rect: Rel_Rect)`
        - **pico.output.draw.image**: Draws image.
            - `pico.output.draw.image (path: string, rect: Rel_Rect)`
        - **pico.output.draw.line**: Draws line.
            - `pico.output.draw.line (p1: Rel_Pos, p2: Rel_Pos)`
        - **pico.output.draw.oval**: Draws oval.
            - `pico.output.draw.oval (rect: Rel_Rect)`
        - **pico.output.draw.pixel**: Draws pixel.
            - `pico.output.draw.pixel (pos: Rel_Pos)`
        - **pico.output.draw.pixels**: Draws multiple pixels.
            - `pico.output.draw.pixels (poss: {Rel_Pos})`
        - **pico.output.draw.poly**: Draws polygon.
            - `pico.output.draw.poly (pts: {Rel_Pos})`
        - **pico.output.draw.rect**: Draws rectangle.
            - `pico.output.draw.rect (rect: Rel_Rect)`
        - **pico.output.draw.text**: Draws text.
            - `pico.output.draw.text (text: string, rect: Rel_Rect)`
        - **pico.output.draw.tri**: Draws triangle.
            - `pico.output.draw.tri (p1: Rel_Pos, p2: Rel_Pos, p3: Rel_Pos)`
