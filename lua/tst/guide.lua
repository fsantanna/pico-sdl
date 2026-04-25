-- guide.lua
-- Mirrors lua/doc/guide.md scenes for interactive inspection.
-- Run with: make int T=guide

require 'pico.check'

-------------------------------------------------------------------------------
-- Section 2: Initialization
-------------------------------------------------------------------------------

-- §2.1: open
do
    pico.init(true)
    pico.set.window { title="guide-02-01-01" }
    pico.check("guide-02-01-01")
    pico.init(false)
end

-- §2.2: configure window and view
do
    pico.init(true)
    pico.set.window {
        title = "guide-02-02-01",
        dim   = { '!', w=200, h=200 },
    }
    pico.set.view {
        dim = { '!', w=200, h=200 },
    }
    pico.check("guide-02-02-01")
    pico.init(false)
end

-- §2.3: close and reopen
do
    pico.init(true)
    pico.set.window { title="guide-02-03-01" }
    pico.check("guide-02-03-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 3: Basic Drawing
-------------------------------------------------------------------------------

-- §3.1.a: pixel
do
    pico.init(true)
    pico.set.window { title="guide-03-01-01" }
    pico.output.draw.pixel { '!', x=50, y=50 }
    pico.check("guide-03-01-01")
    pico.init(false)
end

-- §3.1.b: pixel + rect (retain on screen)
do
    pico.init(true)
    pico.set.window { title="guide-03-01-02" }
    pico.output.draw.pixel { '!', x=50, y=50 }
    pico.output.draw.rect { '!', x=20, y=20, w=30, h=30 }
    pico.check("guide-03-01-02")
    pico.init(false)
end

-- §3.2.a: clear erases previous shapes
do
    pico.init(true)
    pico.set.window { title="guide-03-02-01" }
    pico.output.draw.pixel { '!', x=50, y=50 }
    pico.output.draw.rect { '!', x=20, y=20, w=30, h=30 }
    pico.output.clear()
    pico.check("guide-03-02-01")
    pico.init(false)
end

-- §3.2.b: yellow background, red rect
do
    pico.init(true)
    pico.set.window { title="guide-03-02-02" }
    pico.set.show { color='yellow' }
    pico.set.draw { color='red'    }
    pico.output.clear()
    pico.output.draw.rect { '!', x=50, y=50, w=50, h=50 }
    pico.check("guide-03-02-02")
    pico.init(false)
end

-- §3.2.c: silver pixel (0xCCCCCC) + green pixel (percent mode with alpha)
do
    pico.init(true)
    pico.set.window { title="guide-03-02-03" }
    pico.set.show { color='yellow' }
    pico.set.draw { color='red' }
    pico.output.clear()
    pico.output.draw.rect { '!', x=50, y=50, w=50, h=50 }
    pico.set.draw { color = 0xCCCCCC }
    pico.output.draw.pixel { '!', x=26, y=26 }
    pico.set.draw { color={ '%', r=0, g=0.5, b=0, a=0.5 } }
    pico.output.draw.pixel { '!', x=74, y=74 }
    pico.check("guide-03-02-03")
    pico.init(false)
end

-- §3.2.d: restart resets state
do
    pico.init(true)
    pico.set.window { title="guide-03-02-04" }
    pico.check("guide-03-02-04")
    pico.init(false)
end

-- §3.3.a: two texts, second uses DejaVu
do
    pico.init(true)
    pico.set.window { title="guide-03-03-01" }
    pico.output.draw.text("Hello", {'%', x=0.5, y=0.33, h=0.1})
    pico.set.draw { font='../../DejaVuSans.ttf' }
    pico.output.draw.text("World", {'%', x=0.5, y=0.66, h=0.1})
    pico.check("guide-03-03-01")
    pico.init(false)
end

-- §3.3.b: image
do
    pico.init(true)
    pico.set.window { title="guide-03-03-02" }
    pico.output.draw.image('doc/img/open.png', {'!', x=50, y=50})
    pico.check("guide-03-03-02")
    pico.init(false)
end

-- §3.3.c: pixmap (PI = 10x10 blue circle with silver pi inside)
do
    pico.init(true)
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
    pico.output.draw.pixmap("PI", PI, {'%', x=0.5, y=0.5, w=1, h=1})
    pico.check("guide-03-03-03")
    pico.init(false)
end
