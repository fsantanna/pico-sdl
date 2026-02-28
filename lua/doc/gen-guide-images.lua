-- gen-guide-images.lua
-- Generates all screenshots for the guide
-- Run with: pico-lua gen-guide-images.lua

local function shot(name)
    pico.output.screenshot("img/" .. name .. ".png")
    print(name)
end

-------------------------------------------------------------------------------
-- Section 2: Initialization
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    shot("guide-02-01-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.set.window {
        title = "Hello!",
        dim   = { '!', w=200, h=200 },
    }
    pico.set.view {
        grid = false,
        dim  = { '!', w=200, h=200 },
    }
    shot("guide-02-02-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    shot("guide-02-03-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 3: Basic Drawing
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.pixel({'!', x=50, y=50})
    shot("guide-03-01-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    shot("guide-03-02-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    shot("guide-03-03-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    pico.output.draw.image('img/open.png', {'!', x=70, y=20, w=20, h=20})
    shot("guide-03-04-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 4: Internal State
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    pico.output.draw.image('img/open.png', {'!', x=70, y=20, w=20, h=20})
    pico.set.color.draw('red')
    pico.output.draw.text("Hello", {'!', x=50, y=80, h=10})
    shot("guide-04-01-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    pico.output.draw.image('img/open.png', {'!', x=70, y=20, w=20, h=20})
    pico.set.color.draw('red')
    pico.output.draw.text("Hello", {'!', x=50, y=80, h=10})
    pico.set.alpha(0x88)
    pico.output.draw.oval({'!', x=50, y=80, w=35, h=15})
    shot("guide-04-02-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    pico.output.draw.image('img/open.png',
        {'!', x=70, y=20, w=20, h=20})
    pico.set.color.draw('red')
    pico.output.draw.text("Hello", {'!', x=50, y=80, h=10})
    pico.set.alpha(0x88)
    pico.output.draw.oval({'!', x=50, y=80, w=35, h=15})
    pico.set {
        alpha = 0xFF,
        color = { draw = 'blue' },
        style = 'stroke',
    }
    pico.output.draw.rect({'!', x=60, y=50, w=30, h=30})
    shot("guide-04-03-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 5: Positioning - Mode & Anchor
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.color.draw('red')
    pico.set.alpha(0x88)
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    shot("guide-05-01-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.alpha(0x88)
    pico.output.clear()
    pico.set.color.draw('white')
    pico.output.draw.pixel({'%', x=0.5, y=0.5})
    pico.set.color.draw('red')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='NW'})
    pico.set.color.draw('green')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='C'})
    pico.set.color.draw('blue')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='SE'})
    shot("guide-05-02-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.view {
        dim  = { '#', w=5, h=5 },
        tile = { w=20, h=20 },
    }
    pico.set.window {
        dim = { '#', w=40, h=40 },
    }
    pico.output.clear()
    pico.output.draw.rect {'#', x=3, y=3, w=1, h=1}
    pico.output.draw.rect {'#', x=5, y=1, w=2, h=1, anchor='NE'}
    shot("guide-05-03-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 6: Advanced View
-------------------------------------------------------------------------------

-- 6.1 Target: world on right half of window
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.view {
        target = {'%', x=0.66, y=0.66, w=0.4, h=0.3},
    }
    shot("guide-06-01-01")
    pico.init(false)
end

-- 6.2 Source crop: sub-region at original scale
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.view {
        source = {'%', x=0.5, y=0.6, w=0.3, h=0.3},
        target = {'%', x=0.5, y=0.5, w=0.3, h=0.3},
    }
    shot("guide-06-02-01")
    pico.init(false)
end

-- 6.3 Clip: drawing restricted to center
do
    pico.init(true)
    pico.output.clear()
    pico.set.view {
        clip = {'%', x=0.5, y=0.5, w=0.25, h=0.25},
    }
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    shot("guide-06-03-01")
    pico.init(false)
end

-- 6.4 Zoom: small source stretched to full window
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.view {
        source = { '%', x=0.5, y=0.5, w=0.5, h=0.5 },
    }
    shot("guide-06-04-01")
    pico.init(false)
end

-- 6.4 Scroll: offset source panning the view
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.view {
        source = { '%', x=0.6, y=0.5, w=0.5, h=0.5 },
    }
    shot("guide-06-04-02")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 7: Events
-------------------------------------------------------------------------------

-- pico.input.delay: 3 pixels
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.pixel({'!', x=25, y=50})
    pico.output.draw.pixel({'!', x=50, y=50})
    pico.output.draw.pixel({'!', x=75, y=50})
    shot("guide-07-01-01")
    pico.init(false)
end

-- pico.input.delay: circle loop
do
    pico.init(true)
    pico.output.clear()
    for i=1, 400 do
        local rad = i * math.pi / 180
        pico.output.draw.pixel {
            '!',
            x = 50 + 30 * math.cos(rad),
            y = 50 + 30 * math.sin(rad),
        }
        --pico.input.delay(10)
    end
    shot("guide-07-01-02")
    pico.init(false)
end

-- pico.input.loop: key bindings
do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.image('img/open.png',
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    shot("guide-07-03-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 8: Layers
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.layer.empty("flag", {w=300, h=200})
    pico.set.layer("flag")
    pico.set.color.draw { r=0x00, g=0x2B, b=0x7F }
    pico.output.draw.rect {'%', x=0.00, y=0.0, w=0.33, h=1.0, anchor='NW'}
    pico.set.color.draw { r=0xFC, g=0xD1, b=0x16 }
    pico.output.draw.rect {'%', x=0.33, y=0.0, w=0.34, h=1.0, anchor='NW'}
    pico.set.color.draw { r=0xCE, g=0x11, b=0x26 }
    pico.output.draw.rect {'%', x=0.67, y=0.0, w=0.33, h=1.0, anchor='NW'}
    pico.set.layer()
    pico.output.clear()
    pico.output.draw.layer("flag", {'%', x=0.33, y=0.33, w=0.2})
    pico.output.draw.layer("flag", {'%', x=0.66, y=0.66, w=0.5})
    shot("guide-08-02-01")

    -- 8.3 step 1: rotate 30°, draw at NE
    pico.set.layer("flag")
    pico.set.view { rotation = {angle=30, anchor='C'} }
    pico.set.layer()
    pico.output.draw.layer("flag", {'%', x=0.75, y=0.25, w=0.3})
    shot("guide-08-03-01")

    -- 8.3 step 2: reset rotation, flip horizontal, draw at SW
    pico.set.layer("flag")
    pico.set.view {
        rotation = {angle=0},
        flip = "horizontal",
    }
    pico.set.layer()
    pico.output.draw.layer("flag", {'%', x=0.25, y=0.80, w=0.2})
    shot("guide-08-03-02")

    -- 8.4 sub-layers: 3 squares from the flag in a swirl
    pico.layer.sub("blue",   "flag",
        {'%', x=0.25, y=0.5, w=0.1, h=0.15, anc='C'})
    pico.layer.sub("yellow", "flag",
        {'%', x=0.50, y=0.5, w=0.1, h=0.15, anc='C'})
    pico.layer.sub("red",    "flag",
        {'%', x=0.75, y=0.5, w=0.1, h=0.15, anc='C'})
    pico.output.clear()
    pico.output.draw.layer("blue",
        {'%', x=0.30, y=0.30, w=0.25})
    pico.output.draw.layer("yellow",
        {'%', x=0.70, y=0.45, w=0.25})
    pico.output.draw.layer("red",
        {'%', x=0.45, y=0.75, w=0.25})
    shot("guide-08-04-01")

    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 9: Expert Mode
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.set.expert(true)
    pico.output.draw.rect { '!', x=33, y=33, w=40, h=40 }
    pico.output.draw.rect { '!', x=66, y=66, w=40, h=40 }
    shot("guide-09-01-01")
    pico.output.present()
    shot("guide-09-01-02")
    pico.init(false)
end

print("Done!")
