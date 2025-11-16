# Quick Start

```
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
sudo luarocks install pico-sdl
pico-lua tst/main.lua
```

# Documentation

## Types

- Anchor: `{ x: integer|string, y: integer|string }`
    - `x: string` (`'left'`, `'center'`, `'right'`)
    - `y: string` (`'top'`, `'middle'`, `'bottom'`)
- Color: `{ r: integer, g: integer, b: integer [, a: integer=0xFF] }`
- Dim: `{ x: integer, y: integer }`
- Event: `{ TODO }`
- Pct: `{ x: integer, y: integer }`
- Pos: `{ x: integer, y: integer }`
- Rect: `{ x: integer, y: integer, w: integer, h: integer }`

## API

- [pico.init](#TODO): Initializes and terminates `pico`.
    - `pico.init (on: boolean)`
- [pico.dim](#TODO): Dimension relative to the screen size (or given rectangle).
    - `pico.dim (pct: Pct [,out: Dim]) -> Dim`
    - `pico.dim (x: integer, y: integer [,out: Dim]) -> Dim`
- [pico.pos](#TODO): Position relative to the screen size (or given rectangle).
    - `pico.pos (pct: Pct [,out: Rect]) -> Pos`
    - `pico.pos (x: integer, y: integer [,out: Rect]) -> Pos`
- **pico.get**: State read operations.
    - [pico.get.rotate](#TODO): Rotation angle in degrees.
        - `pico.get.rotate () -> integer`
    - [pico.get.size](#TODO): Physical and logical window dimensions.
        - `pico.get.size () -> { phy=Dim, log=Dim }`
- **pico.set**: State write operations.
    - [pico.set.crop](#TODO): Crop area.
        - `pico.set.crop ([r: Rect])`
    - [pico.set.cursor](#TODO): Text cursor position.
        - `pico.set.cursor (pos: Pos)`
    - [pico.set.expert](#TODO): Expert operation mode.
        - `pico.set.expert (on: boolean)`
    - [pico.set.font](#TODO): Font for texts.
        - `pico.set.font (path: [string], h: integer)`
    - [pico.set.grid](#TODO): Toggles window grid.
        - `pico.set.grid (on: boolean)`
    - [pico.set.rotate](#TODO): Rotation angle in degrees.
        - `pico.set.rotate (ang: integer)`
    - [pico.set.scale](#TODO): Scale relative to original dimensions.
        - `pico.set.scale (pct: Pct)`
    - [pico.set.size](#TODO): Physical and logical window dimensions.
        - `pico.set.size (phy: Dim|boolean, log: Dim|boolean)`
    - [pico.set.title](#TODO): Window title.
        - `pico.set.title (title: string)`
    - [pico.set.zoom](#TODO): Window zoom.
        - `pico.set.zoom (pct: Pct)`
    - **pico.set.color**: Color operations.
        - [pico.set.color.clear](#TODO): Window clear color.
            - `pico.set.color.clear (clr: Color)`
            - `pico.set.color.clear (r, g, b [,a])`
        - [pico.set.color.draw](#TODO): Draw color.
            - `pico.set.color.draw (clr: Color)`
            - `pico.set.color.draw (r, g, b [,a])`
    - **pico.set.anchor**: Anchor operations.
        - [pico.set.anchor.draw](#TODO): Draw anchor.
            - `pico.set.anchor.draw (anc: Anchor)`
            - `pico.set.anchor.draw (x, y)`
        - [pico.set.anchor.rotate](#TODO): Rotate anchor.
            - `pico.set.anchor.rotate (anc: Anchor)`
            - `pico.set.anchor.rotate (x, y)`
- **pico.input**: Input operations.
    - [pico.input.delay](#TODO): Freezes execution for milliseconds.
        - `pico.input.delay (ms: integer)`
    - [pico.input.event](#TODO): Waits event for a number of milliseconds.
        - `pico.input.event ([evt: Event], [ms: integer])`
- **pico.output**: Output operations
    - [pico.output.clear](#TODO): Clears window.
        - `pico.output.clear ()`
    - **pico.output.draw**: Draw operations.
        - [pico.output.draw.buffer](#TODO): Draws array of colors.
            - `pico.output.draw.buffer (pos: Pos, buffer: {{Color}})`
        - [pico.output.draw.image](#TODO): Draws image.
            - `pico.output.draw.image (pos: Pos, path: string [,Dim])`
        - [pico.output.draw.line](#TODO): Draws line.
            - `pico.output.draw.line (p1: Pos, p2: Pos)`
        - [pico.output.draw.oval](#TODO): Draws oval.
            - `pico.output.draw.oval (rect: Rect)`
        - [pico.output.draw.pixel](#TODO): Draws pixel.
            - `pico.output.draw.pixel (pos: Pos)`
        - [pico.output.draw.pixels](#TODO): Draws vector of pixels.
            - `pico.output.draw.pixels (poss: {Pos})`
        - [pico.output.draw.poly](#TODO): Draws polygon.
            - `pico.output.draw.poly (pts: {Pos})`
        - [pico.output.draw.rect](#TODO): Draws rectangle.
            - `pico.output.draw.rect (rect: Rect)`
        - [pico.output.draw.text](#TODO): Draws text.
            - `pico.output.draw.text (pos: Pos, text: string)`
        - [pico.output.draw.tri](#TODO): Draws triangle.
            - `pico.output.draw.tri (rect: Rect)`
    - [pico.output.preset](#TODO): Redraws the window.
        - `pico.output.present ()`
    - [pico.output.screenshot](#TODO): Takes a window screenshot.
        - `pico.output.screenshot ([path: string], [rect: Rect])`
    - [pico.output.sound](#TODO): Plays sound.
        - `pico.output.sound (path: string)`
    - [pico.output.write](#TODO): Writes text.
        - `pico.output.write (text: string)`
    - [pico.output.writeln](#TODO): Writes text and breaks line.
        - `pico.output.writeln (text: string)`
- **pico.vs**: Collision operations.
    - [pico.vs.pos_rect](#TODO): Collision between position and rectangle.
        - `pico.vs.pos_rect (pos: Pos, rect: Rect [,anc, anc]) -> boolean`
    - [pico.vs.rect_rect](#TODO): Collision between two rectangles.
        - `pico.vs.rect_rect (r1: Rect, r2: Rect [,anc, anc]) -> boolean`

# Install & Run

## Dependencies

```
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

## Luarocks

```
sudo luarocks install pico-sdl
pico-lua tst/main.lua
```

## Manual

Assumes `lua5.4`:

```
gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../src/hash.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image
sudo cp pico-lua /usr/local/bin/
sudo cp pico.so /usr/local/share/lua/5.4/
pico-lua tst/main.lua
```
