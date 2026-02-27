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
    pico.set.view {
        title  = "Hello!",
        grid   = false,
        window = {'!', w=200, h=200},
        world  = {'!', w=200, h=200},
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
        window = {'#', w=40, h=40},
        world  = {'#', w=5, h=5},
        tile   = {w=20, h=20},
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

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.pixel({'!', x=25, y=50})
    pico.output.draw.pixel({'!', x=50, y=50})
    pico.output.draw.pixel({'!', x=75, y=50})
    shot("guide-07-01-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 8: Layers
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.layer.empty("bg", {w=100, h=100})
    pico.set.layer("bg")
    pico.set.color.draw('blue')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.8})
    pico.set.layer(nil)
    pico.output.clear()
    pico.output.draw.layer("bg", {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    shot("guide-08-02-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 9: Expert Mode
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.expert(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=25, y=25, w=50, h=50})
    shot("guide-09-01-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.expert(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=25, y=25, w=50, h=50})
    pico.output.present()
    shot("guide-09-01-02")
    pico.init(false)
end

print("Done!")
