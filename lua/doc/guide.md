# Guide

1.  [Introduction](#1-introduction)
2.  [Initialization](#2-initialization)
3.  [Basic Drawing](#3-basic-drawing)
4.  [Positioning: Modes & Anchors](#4-positioning-modes--anchors)

## 1. Introduction

`pico-lua` is a graphics library for 2D games and applications.
It is designed around 4 groups of APIs:

- `pico.output.*`: draw shapes and layers; play sound and video.
- `pico.input.*`: handle events, such as time passing and key presses.
- `pico.get.*` and `pico.set.*`: manage internal library state,
    such as current drawing color, and window dimensions.
- `pico.layer.*`: structure visuals in hierarquical scenes.

This guide is an interactive walk-through of `pico-lua`.
It is not meant to be exhaustive.
Consult the [API](api.md) for completeness.

From the command line, simply execute `pico-lua` with no parameters:

```bash
$ pico-lua
Lua 5.4.4  Copyright (C) 1994-2022 Lua.org, PUC-Rio
>
```

The Lua prompt `>` indicates that `pico-lua` is ready to receive commands.

## 2. Initialization

### 2.1. Open

To initialize `pico-lua`, we pass `true` to `pico.init`:

<table>
<tr><td><pre>
> pico.init(true)
</pre>
</td><td>
<img src="../tst/asr/guide-02-01-01.png" width="400">
</td></tr>
</table>

We immediately see a `500x500` window divided in small `5x5` rectangles
representing `100x100` **logical pixels**.

In the context of `pico-lua`, we use the term **world** to designate the
logical view, which applications use as the main reference.

By default, note that `pico-lua` conventionally exhibits the grid and
coordinate labels to aid development with visual inspection.
You may click the image to zoom in.

### 2.2. Configure

To configure the window and the world view, we use `pico.set.window` and
`pico.set.view`:

<table>
<tr><td><pre>
> pico.set.window {
    title = "Hello!",
    dim   = { '!', w=200, h=200 },
  }
> pico.set.view {
    dim = { '!', w=200, h=200 },
  }
</pre>
</td><td>
<img src="../tst/asr/guide-02-02-01.png" width="200">
</td></tr>
</table>

After the two commands, the window title changes and the grid disappears, since
the pixel size is now `1x1`.

The character `'!'` indicates a dimension in "raw mode", which we will discuss
in [#Positioning Modes](#4-positioning-modes--anchors).

You could also use the simpler `pico.set.dim` to set the window and view
dimensions at the same time:

<table>
<tr><td><pre>
> pico.set.dim { '!', w=200, h=200 }
</pre>
</td><td>
<img src="../tst/asr/guide-02-02-01.png" width="200">
</td></tr>
</table>

*(The image is the same as before.)*

### 2.3. Close

In the end of the session, we should pass `false` to `pico.init` to properly
finalize `pico-lua`:

<table>
<tr><td><pre>
> pico.init(false)
</pre>
</td><td>
*(no image)*
</td></tr>
</table>

Then, we can restart `pico-lua` by calling `pico.init(true)` again:

<table>
<tr><td><pre>
> pico.init(true)
</pre>
</td><td>
<img src="../tst/asr/guide-02-01-01.png" width="200">
</td></tr>
</table>

We can see that the title, grid, and sizes are now reset to default.

## 3. Basic Drawing

To ease prototyping, drawing operations in `pico-lua` take immediate effect.

### 3.1. Shapes

To draw a single pixel, we call `pico.output.draw.pixel`:

<table>
<tr><td><pre>
> pico.output.draw.pixel { '!', x=50, y=50 }
</pre>
</td><td>
<img src="../tst/asr/guide-03-01-01.png" width="200">
</td></tr>
</table>

The pixel occupies a physical `5x5` square representing a single world pixel,
as expected.

To draw a rectangle, we call `pico.output.draw.rect`:

<table>
<tr><td><pre>
> pico.output.draw.rect { '!', x=20, y=20, w=30, h=30 }
</pre>
</td><td>
<img src="../tst/asr/guide-03-01-02.png" width="200">
</td></tr>
</table>

The table specifies a rectangle at position `(20,20)` with size `30x30`.

Unlike most graphics libraries, `pico-lua` **centers** objects by default,
which we will discuss in [#Anchors](#52-anchors).
With the aid of the tick marks, we can see that the rectangle is actually
centered at the given position.

Note that `pico-lua` retains both shapes on the screen.

Other drawing operations include `pico.output.draw.line`,
`pico.output.draw.polygon`, and `pico.output.draw.oval`.

### 3.2. Colors

To clear the screen, we call `pico.output.clear`:

<table>
<tr><td><pre>
> pico.output.clear()
</pre>
</td><td>
<img src="../tst/asr/guide-02-01-01.png" width="200">
</td></tr>
</table>

Now the pixel and rectangle are gone.

By default `pico-lua`, uses black to clear the screen and white to draw shapes.

To pick different colors, we use `pico.set.show` and `pico.set.draw` passing
the `color` attribute:

<table>
<tr><td><pre>
> pico.set.show { color='yellow' }  -- background
> pico.set.draw { color='red'    }  -- shapes
> pico.output.clear()
> pico.output.draw.rect { '!', x=50, y=50, w=50, h=50 }
</pre>
</td><td>
<img src="../tst/asr/guide-03-02-02.png" width="200">
</td></tr>
</table>

In the example, we set the background color as yellow, and the drawing color
for shapes as red.
A `set` has no immediate effect on the screen, and only affects further
operations.
Next, we actually clear the screen and draw a centered rectangle.

Colors can also be specified as numbers or tables:

<table>
<tr><td><pre>
> pico.set.draw {       -- 0xRRGGBB[AA]
    color = 0xCCCCCC
  }
  pico.output.draw.pixel { '!', x=26, y=26 }

> pico.set.draw {       -- percent mode '%' (0.0-1.0)
    color={ '%', r=0, g=0.5, b=0, a=0.5 }
  }
  pico.output.draw.pixel { '!', x=73, y=73 }
</pre>
</td><td>
<img src="../tst/asr/guide-03-02-03.png" width="200">
</td></tr>
</table>

Let's restart `pico-lua` for the next examples:

<table>
<tr><td><pre>
> pico.init(false)
> pico.init(true)
</pre>
</td><td>
<img src="../tst/asr/guide-02-01-01.png" width="200">
</td></tr>
</table>

### 3.3. Text, Image and Pixmap

To draw text, we call `pico.output.draw.text`:

<table>
<tr><td><pre>
> pico.output.draw.text("Hello", {'!', x=50, y=33, h=30})
> pico.set.draw { font='DejaVuSans.ttf' }
> pico.output.draw.text("Hello", {'!', x=50, y=66, h=30})
</pre>
</td><td>
<img src="../tst/asr/guide-03-03-01.png" width="200">
</td></tr>
</table>

*(You may need to save the [font](../../res/DejaVuSans.ttf) on your machine.)*

We first draw `Hello` at the top using the default built-in font.
Then, we switch the font and draw `Hello` again at the bottom.

Note that `pico-lua` preserves the correct text aspect ratio when width `w` is
omitted.

To draw an [image](../../res/open.png), we call `pico.output.draw.image`:

<table>
<tr><td><pre>
> pico.output.clear()
> pico.output.draw.image('open.png', {'!', x=50, y=50})
</pre>
</td><td>
<img src="../tst/asr/guide-03-03-02.png" width="200">
</td></tr>
</table>

*(You may need to save the [image](../../res/open.png) on your machine.)*

For images, we can omit both width `w` and height `h` to preserve original
dimensions and aspect ratio.

`pico-lua` also supports pixmaps, which are arrays of colored pixels:

<table>
<tr><td><pre>
> _ = 'black'
  o = 'blue'
  x = 'silver'
  PI = {
      {_,_,_,o,o,o,o,_,_,_},
      {_,o,o,o,o,o,o,o,o,_},
      {_,o,x,x,x,x,x,x,o,_},
      {o,o,o,x,o,o,x,o,o,o},
      {o,o,o,x,o,o,x,o,o,o},
      {o,o,o,x,o,o,x,o,o,o},
      {o,o,o,x,o,o,x,o,o,o},
      {_,o,o,x,o,o,x,o,o,_},
      {_,o,o,o,o,o,o,o,o,_},
      {_,_,_,o,o,o,o,_,_,_},
  }
> pico.output.draw.pixmap("pi", PI, {'!', x=50, y=50, w=80})
</pre>
</td><td>
<img src="../tst/asr/guide-03-03-03.png" width="200">
</td></tr>
</table>

The table `PI` determines the color of each pixel to draw.
We use single-char color variables to ease visualizing: the Greek letter `pi`,
on top of blue circle, on top of a black background.

Note that pixmaps require a string identifier as first parameter for proper
caching.

Internally, `pico-lua` caches texts, images and pixmaps such that they are
reused on subsequent redraws.
We will detail caching in [#Layers](#8-layers).

## 4. Positioning: Modes & Anchors

The positioning **mode** determines the unit used in world coordinates:

- `'!'` - Raw: world pixel coordinates
            (from `0` to world `w/h`)
- `'%'` - Percentage: coordinates relative to world `w/h`
            (from `0.0` to `1.0`)
- `'#'` - Tile: grid coordinates based on tile and world `w/h`
            (from `0` to `w/h`)

The percentage mode `'%'` is preferable, since it is independent of world
dimensions and adapts naturally to resizes.
For instance, `{ '%', x=0.5, y=0.5 }` always points to the center of the
world, regardless of its size.

The mode is set at index `1` in tables representing positions, dimensions, and
rectangles:

- `{ '%', x=0.5, y=0.5 }`:          a centered relative position
- `{ '!', w=20, h=30 }`:            a raw dimension
- `{ '#', x=4, y=4, w=2, h=1 }`:    a rectangle covering 2 tiles horizontally

The positioning **anchor** determines the reference point **within** the shape:

```
+-----------+
|NW   N   NE|
| W   C   E |
|SW   S   SE|
+-----------+
```

When drawing, the anchor position is pinned to the given coordinate.
As shown in the previous examples, by default, `pico-lua` uses the center
anchor `'C'`.

The anchor is set at field `anchor` in tables representing positions:

- `{ '%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='SW' }`:
    the rectangle's *southwest* corner pinned at center of the screen

Let's draw a pixel and three rectangles, all at the same position, but using
different anchors:

<table>
<tr><td><pre>
> pico.init(false) ; pico.init(true)
> pico.set.draw { color='white' }
> pico.output.draw.pixel { '%', x=0.5, y=0.5 }
> pico.set.draw { color=pico.color.alpha('red', 0x80) }
> pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='NW' }
> pico.set.draw { color=pico.color.alpha('green', 0x80) }
> pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='C' }
> pico.set.draw { color=pico.color.alpha('blue', 0x80) }
> pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='SE' }
</pre>
</td><td>
<img src="../tst/asr/guide-04-00-01.png" width="200">
</td></tr>
</table>

The pixel represents the reference position `%(0.5,0.5)` used by the three
rectangles.
The `0x80` alpha makes the overlapping regions reveal how the pinned anchors
offset each rectangle from the same coordinate:

- the **red** rectangle places its `NW` corner at `(0.5,0.5)`
- the **green** one places its center `C` there
- the **blue** one places its `SE` corner there
