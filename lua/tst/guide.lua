-- guide.lua
-- Mirrors lua/doc/guide.md scenes for interactive inspection.
-- Run with: make int T=guide
-- State carries between scenes; restarts only where the guide explicitly does.

require 'pico.check'

-------------------------------------------------------------------------------
-- Section 2: Initialization
-------------------------------------------------------------------------------

-- §2.1: open
pico.init(true)
pico.set.window { title="guide-02-01-01" }
pico.check("guide-02-01-01")

-- §2.2: configure window and view
pico.set.window {
    title = "guide-02-02-01",
    dim   = { '!', w=200, h=200 },
}
pico.set.view {
    dim = { '!', w=200, h=200 },
}
pico.check("guide-02-02-01")

-- §2.2 (cont): pico.set.dim shortcut (same visual)
pico.set.dim { '!', w=200, h=200 }
pico.set.window { title="guide-02-02-01 (dim)" }
pico.check("guide-02-02-01")

-- §2.3: close and reopen
pico.init(false)
pico.init(true)
pico.set.window { title="guide-02-03-01" }
pico.check("guide-02-01-01")

-------------------------------------------------------------------------------
-- Section 3: Basic Drawing
-------------------------------------------------------------------------------

-- §3.1.a: pixel
pico.set.window { title="guide-03-01-01" }
pico.output.draw.pixel { '!', x=50, y=50 }
pico.check("guide-03-01-01")

-- §3.1.b: rect added on top of pixel (no clear)
pico.set.window { title="guide-03-01-02" }
pico.output.draw.rect { '!', x=20, y=20, w=30, h=30 }
pico.check("guide-03-01-02")

-- §3.2.a: clear erases previous shapes
pico.set.window { title="guide-03-02-01" }
pico.output.clear()
pico.check("guide-02-01-01")

-- §3.2.b: yellow background, red rect
pico.set.window { title="guide-03-02-02" }
pico.set.show { color='yellow' }
pico.set.draw { color='red'    }
pico.output.clear()
pico.output.draw.rect { '!', x=50, y=50, w=50, h=50 }
pico.check("guide-03-02-02")

-- §3.2.c: silver pixel (0xCCCCCC) + green pixel (percent mode with alpha)
pico.set.window { title="guide-03-02-03" }
pico.set.draw { color = 0xCCCCCC }
pico.output.draw.pixel { '!', x=26, y=26 }
pico.set.draw { color={ '%', r=0, g=0.5, b=0, a=0.5 } }
pico.output.draw.pixel { '!', x=73, y=73 }
pico.check("guide-03-02-03")

-- §3.2.d: restart resets state
pico.init(false)
pico.init(true)
pico.set.window { title="guide-03-02-04" }
pico.check("guide-02-01-01")

-- §3.3.a: two texts, second uses DejaVu
pico.set.window { title="guide-03-03-01" }
pico.output.draw.text("Hello", {'!', x=50, y=33, h=30})
pico.set.draw { font='../../res/DejaVuSans.ttf' }
pico.output.draw.text("Hello", {'!', x=50, y=66, h=30})
pico.check("guide-03-03-01")

-- §3.3.b: image (clear + draw)
pico.set.window { title="guide-03-03-02" }
pico.output.clear()
pico.output.draw.image('../../res/open.png', {'!', x=50, y=50})
pico.check("guide-03-03-02")

-- §3.3.c: pixmap (PI = 10x10 blue circle with silver pi inside)
pico.set.window { title="guide-03-03-03" }
local _ = 'black'
local o = 'blue'
local x = 'silver'
local PI = {
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
pico.output.draw.pixmap("pi", PI, {'!', x=50, y=50, w=80})
pico.check("guide-03-03-03")

-------------------------------------------------------------------------------
-- Section 4: Positioning: Modes & Anchors
-------------------------------------------------------------------------------

-- §4: pixel + three rects with different anchors (50% alpha)
pico.init(false)
pico.init(true)
pico.set.window { title="guide-04-00-01" }
pico.set.draw { color='white' }
pico.output.draw.pixel { '%', x=0.5, y=0.5 }
pico.set.draw { color=pico.color.alpha('red',   0x80) }
pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='NW' }
pico.set.draw { color=pico.color.alpha('green', 0x80) }
pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='C' }
pico.set.draw { color=pico.color.alpha('blue',  0x80) }
pico.output.draw.rect { '%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='SE' }
pico.check("guide-04-00-01")

-- §4.1: tile mode (5x5 tiles of 20x20 px; 40x40 tile window)
pico.init(false)
pico.init(true)
pico.set.window { title="guide-04-01-01" }
pico.set.view {
    dim  = { '#', w=5, h=5 },
    tile = { w=20, h=20 },
}
pico.set.window {
    dim = { '#', w=40, h=40 },
}
pico.output.draw.rect { '#', x=3, y=3, w=1, h=1 }
pico.output.draw.rect { '#', x=5, y=1, w=2, h=1, anchor='NE' }
pico.check("guide-04-01-01")

-------------------------------------------------------------------------------
-- Section 5: Advanced Views
-------------------------------------------------------------------------------

-- §5: target moves world to a smaller bottom-right region
pico.init(false); pico.init(true)
pico.set.window { title="guide-05-00-01" }
pico.output.draw.image('../../res/open.png',
    {'%', x=0.5, y=0.5, w=0.5, h=0.5})
pico.set.view { target = {'%', x=0.8, y=0.8, w=0.3, h=0.2} }
pico.check("guide-05-00-01")

-- §5: source crops a region of the world (continues from previous state)
pico.set.window { title="guide-05-00-02" }
pico.set.view {
    source = {'%', x=0.5, y=0.5, w=0.4, h=0.4},
}
pico.check("guide-05-00-02")

-- §5: clip restricts drawing to a small center region
pico.init(false); pico.init(true)
pico.set.window { title="guide-05-00-03" }
pico.set.view { clip = {'%', x=0.5, y=0.5, w=0.25, h=0.25} }
pico.output.draw.image('../../res/open.png',
    {'%', x=0.5, y=0.5, w=0.5, h=0.5})
pico.check("guide-05-00-03")

-- §5.1.a: restart with centered image and black window background
pico.init(false); pico.init(true)
pico.set.window { title="guide-05-01-01", color='black' }
pico.output.draw.image('../../res/open.png',
    {'%', x=0.5, y=0.5, w=0.5, h=0.5})
pico.check("guide-05-01-01")

-- §5.1.b: source crop (w=h=0.5) -> 2x zoom in
pico.set.window { title="guide-05-01-02" }
pico.output.draw.image('../../res/open.png',
    {'%', x=0.5, y=0.5, w=0.5, h=0.5})
pico.set.view { source = {'%', x=0.5, y=0.5, w=0.5, h=0.5} }
pico.check("guide-05-01-02")

-- §5.1.c: source double (w=h=2) -> 1/2 zoom out
pico.set.window { title="guide-05-01-03" }
pico.set.view { source = {'%', x=0.5, y=0.5, w=2, h=2} }
pico.check("guide-05-01-03")

-- §5.1.d: shift source offset -> scroll
pico.set.window { title="guide-05-01-04" }
pico.set.view { source = {'%', x=0.8, y=0.2, w=2, h=2} }
pico.check("guide-05-01-04")

-------------------------------------------------------------------------------
-- Section 6: Events
-------------------------------------------------------------------------------

-- §6.1.a: 3 pixels at distinct horizontal positions (delays omitted in tests)
pico.init(false); pico.init(true)
pico.set.window { title="guide-06-01-01" }
pico.output.draw.pixel { '%', x=0.25, y=0.5 }
pico.output.draw.pixel { '%', x=0.50, y=0.5 }
pico.output.draw.pixel { '%', x=0.75, y=0.5 }
pico.check("guide-06-01-01")

-- §6.1.b: circle drawn pixel-by-pixel
pico.output.clear()
pico.set.window { title="guide-06-01-02" }
for i=1, 400 do
    local rad = i * math.pi / 180
    pico.output.draw.pixel { '%',
        x = 0.5 + 0.3*math.cos(rad),
        y = 0.5 + 0.3*math.sin(rad),
    }
end
pico.check("guide-06-01-02")

-- §6.3: image (pico.input.loop omitted; would block tests)
pico.output.clear()
pico.set.window { title="guide-06-03-01" }
pico.output.draw.image('../../res/open.png',
    {'%', x=0.5, y=0.5, w=0.5, h=0.5})
pico.check("guide-06-03-01")

-------------------------------------------------------------------------------
-- Section 7: Layers
-------------------------------------------------------------------------------

-- §7.1: create the "flag" layer (no check; nothing on screen yet)
pico.output.clear()
pico.layer.empty(nil, "flag", {w=300, h=200})
pico.set.layer("flag")
pico.set.draw { color={ r=0x00, g=0x2B, b=0x7F } }
pico.output.draw.rect { '%', x=0.00, y=0.0, w=0.33, h=1.0, anchor='NW' }
pico.set.draw { color={ r=0xFC, g=0xD1, b=0x16 } }
pico.output.draw.rect { '%', x=0.33, y=0.0, w=0.34, h=1.0, anchor='NW' }
pico.set.draw { color={ r=0xCE, g=0x11, b=0x26 } }
pico.output.draw.rect { '%', x=0.67, y=0.0, w=0.33, h=1.0, anchor='NW' }

-- §7.2: composite flag twice on the world
pico.set.window { title="guide-07-02-01" }
pico.set.layer()
pico.output.draw.layer("flag", {'%', x=0.33, y=0.33, w=0.2})
pico.output.draw.layer("flag", {'%', x=0.66, y=0.66, w=0.5})
pico.check("guide-07-02-01")

-- §7.3.a: rotate flag, draw at top-right
pico.set.window { title="guide-07-03-01" }
pico.set.layer("flag")
pico.set.show { rotate={angle=30, anchor='C'} }
pico.set.layer()
pico.output.clear()
pico.output.draw.layer("flag", {'%', x=0.75, y=0.25, w=0.3})
pico.check("guide-07-03-01")

-- §7.3.b: reset rotation, flip horizontally, draw at bottom-left
pico.set.window { title="guide-07-03-02" }
pico.set.layer("flag")
pico.set.show {
    rotate = {angle=0},
    flip   = 'horizontal',
}
pico.set.layer()
pico.output.draw.layer("flag", {'%', x=0.25, y=0.80, w=0.2})
pico.check("guide-07-03-02")

-- §7.4: sub-layers cropping each stripe of the flag
pico.set.window { title="guide-07-04-01" }
pico.layer.sub(nil, "blue",   "flag", {'%', x=0.25, y=0.5, w=0.1, h=0.15})
pico.layer.sub(nil, "yellow", "flag", {'%', x=0.50, y=0.5, w=0.1, h=0.15})
pico.layer.sub(nil, "red",    "flag", {'%', x=0.75, y=0.5, w=0.1, h=0.15})
pico.output.clear()
pico.output.draw.layer("blue",   {'%', x=0.30, y=0.30, w=0.25})
pico.output.draw.layer("yellow", {'%', x=0.70, y=0.45, w=0.25})
pico.output.draw.layer("red",    {'%', x=0.45, y=0.75, w=0.25})
pico.check("guide-07-04-01")

pico.init(false)
