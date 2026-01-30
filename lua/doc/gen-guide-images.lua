-- gen-guide-images.lua
-- Generates all screenshots for the guide
-- Run with: pico-lua gen-guide-images.lua

local W = 160  -- window size
local L = 16   -- logical/world size

local function setup(title)
    pico.init(true)
    pico.set.view {
        title  = title or "Guide",
        window = {'!', w=W, h=W},
        world  = {'!', w=L, h=L},
    }
end

local function shot(name)
    pico.output.screenshot("img/" .. name .. ".png")
    print(name)
end

local function finish()
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 2: Initialization
-------------------------------------------------------------------------------

do
    setup("pico-lua")
    pico.output.clear()
    shot("guide-01")
    finish()
end

do
    setup("Hello!")
    pico.output.clear()
    shot("guide-02")
    finish()
end

do
    setup("No Grid")
    pico.set.view { grid=false }
    pico.output.clear()
    shot("guide-03")
    finish()
end

do
    pico.init(true)
    pico.set.view {
        title  = "Custom Size",
        window = {'!', w=W, h=W},
        world  = {'!', w=8, h=8},
    }
    pico.output.clear()
    shot("guide-04")
    finish()
end

-------------------------------------------------------------------------------
-- Section 3: Basic Drawing
-------------------------------------------------------------------------------

do
    setup("Clear")
    pico.output.clear()
    shot("guide-05")
    finish()
end

do
    setup("Pixel")
    pico.output.clear()
    pico.output.draw.pixel({'!', x=8, y=8})
    shot("guide-06")
    finish()
end

do
    setup("Rectangle")
    pico.output.clear()
    pico.output.draw.rect({'!', x=4, y=4, w=8, h=8, anc='NW'})
    shot("guide-07")
    finish()
end

do
    setup("Line")
    pico.output.clear()
    pico.output.draw.line({'!', x=2, y=2}, {'!', x=14, y=14})
    shot("guide-08")
    finish()
end

do
    setup("Oval")
    pico.output.clear()
    pico.output.draw.oval({'!', x=8, y=8, w=10, h=6, anc='C'})
    shot("guide-09")
    finish()
end

do
    setup("Triangle")
    pico.output.clear()
    pico.output.draw.tri(
        {'!', x=8,  y=2},
        {'!', x=2,  y=14},
        {'!', x=14, y=14}
    )
    shot("guide-10")
    finish()
end

do
    setup("Text")
    pico.output.clear()
    pico.output.draw.text("Hi", {'%', x=0.5, y=0.5, w=0.5, h=0.3, anc='C'})
    shot("guide-11")
    finish()
end

do
    setup("Colors")
    pico.output.clear()
    pico.set.color.draw('red')
    pico.output.draw.rect({'!', x=1, y=1, w=4, h=4, anc='NW'})
    pico.set.color.draw('green')
    pico.output.draw.rect({'!', x=6, y=1, w=4, h=4, anc='NW'})
    pico.set.color.draw('blue')
    pico.output.draw.rect({'!', x=11, y=1, w=4, h=4, anc='NW'})
    pico.set.color.draw('yellow')
    pico.output.draw.rect({'!', x=1, y=6, w=4, h=4, anc='NW'})
    pico.set.color.draw('cyan')
    pico.output.draw.rect({'!', x=6, y=6, w=4, h=4, anc='NW'})
    pico.set.color.draw('magenta')
    pico.output.draw.rect({'!', x=11, y=6, w=4, h=4, anc='NW'})
    pico.set.color.draw('orange')
    pico.output.draw.rect({'!', x=1, y=11, w=4, h=4, anc='NW'})
    pico.set.color.draw('purple')
    pico.output.draw.rect({'!', x=6, y=11, w=4, h=4, anc='NW'})
    pico.set.color.draw('white')
    pico.output.draw.rect({'!', x=11, y=11, w=4, h=4, anc='NW'})
    shot("guide-12")
    finish()
end

do
    setup("Alpha")
    pico.output.clear()
    pico.set.color.draw('red')
    pico.output.draw.rect({'!', x=2, y=4, w=8, h=8, anc='NW'})
    pico.set.color.draw('blue')
    pico.set.alpha(128)
    pico.output.draw.rect({'!', x=6, y=4, w=8, h=8, anc='NW'})
    shot("guide-13")
    finish()
end

do
    setup("Stroke")
    pico.output.clear()
    pico.set.style('fill')
    pico.output.draw.rect({'!', x=2, y=4, w=5, h=8, anc='NW'})
    pico.set.style('stroke')
    pico.output.draw.rect({'!', x=9, y=4, w=5, h=8, anc='NW'})
    shot("guide-14")
    finish()
end

-------------------------------------------------------------------------------
-- Section 4: Positioning - Mode & Anchor
-------------------------------------------------------------------------------

do
    setup("Raw Mode")
    pico.output.clear()
    pico.output.draw.rect({'!', x=2, y=2, w=6, h=6, anc='NW'})
    shot("guide-15")
    finish()
end

do
    setup("Pct Mode")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.4, h=0.4, anc='C'})
    shot("guide-16")
    finish()
end

do
    setup("Anchors")
    pico.output.clear()
    local a = {'NW', 'N', 'NE', 'W', 'C', 'E', 'SW', 'S', 'SE'}
    local pos = {
        {x=0.15, y=0.15}, {x=0.5, y=0.15}, {x=0.85, y=0.15},
        {x=0.15, y=0.5},  {x=0.5, y=0.5},  {x=0.85, y=0.5},
        {x=0.15, y=0.85}, {x=0.5, y=0.85}, {x=0.85, y=0.85},
    }
    for i = 1, 9 do
        pico.output.draw.rect({
            '%', x=pos[i].x, y=pos[i].y, w=0.18, h=0.18, anc=a[i]
        })
    end
    shot("guide-17")
    finish()
end

do
    setup("Anchor Effect")
    pico.output.clear()
    pico.set.color.draw('gray')
    pico.output.draw.pixel({'%', x=0.5, y=0.5})
    pico.set.color.draw('red')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='NW'})
    pico.set.color.draw('green')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='C'})
    pico.set.color.draw('blue')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='SE'})
    shot("guide-18")
    finish()
end

do
    setup("Hierarchy")
    pico.output.clear()
    local parent = {'%', x=0.5, y=0.5, w=0.6, h=0.6, anc='C'}
    pico.set.color.draw('gray')
    pico.output.draw.rect(parent)
    pico.set.color.draw('white')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C', up=parent})
    shot("guide-19")
    finish()
end

do
    pico.init(true)
    pico.set.view {
        title  = "Tiles",
        window = {'!', w=W, h=W},
        world  = {'#', w=4, h=4},
        tile   = {w=4, h=4},
    }
    pico.output.clear()
    pico.output.draw.rect({'#', x=1, y=1, w=1, h=1, anc='NW'})
    pico.output.draw.rect({'#', x=3, y=2, w=1, h=1, anc='NW'})
    pico.output.draw.rect({'#', x=2, y=4, w=2, h=1, anc='NW'})
    shot("guide-20")
    finish()
end

-------------------------------------------------------------------------------
-- Section 5: Advanced View
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.view {
        title  = "Zoom 1:1",
        window = {'!', w=W, h=W},
        world  = {'!', w=W, h=W},
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=40, y=40, w=80, h=80, anc='NW'})
    shot("guide-21")
    finish()
end

do
    pico.init(true)
    pico.set.view {
        title  = "Zoom 2x",
        window = {'!', w=W, h=W},
        world  = {'!', w=W/2, h=W/2},
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=40, h=40, anc='NW'})
    shot("guide-22")
    finish()
end

do
    pico.init(true)
    pico.set.view {
        title  = "Scroll",
        window = {'!', w=W, h=W},
        world  = {'!', w=W, h=W},
        source = {'!', x=40, y=40, w=W, h=W},
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=40, y=40, w=80, h=80, anc='NW'})
    shot("guide-23")
    finish()
end

do
    setup("Crop")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.8, anc='C'})
    pico.set.crop({'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C'})
    pico.set.color.draw('red')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.6, h=0.6, anc='C'})
    shot("guide-24")
    finish()
end

-------------------------------------------------------------------------------
-- Section 6: Events (static images showing animation frames)
-------------------------------------------------------------------------------

do
    setup("Animation 1")
    pico.output.clear()
    pico.output.draw.pixel({'!', x=4, y=8})
    shot("guide-25")
    finish()
end

do
    setup("Animation 2")
    pico.output.clear()
    pico.output.draw.pixel({'!', x=8, y=8})
    shot("guide-26")
    finish()
end

do
    setup("Animation 3")
    pico.output.clear()
    pico.output.draw.pixel({'!', x=12, y=8})
    shot("guide-27")
    finish()
end

-------------------------------------------------------------------------------
-- Section 7: Expert Mode (cumulative session)
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.view {
        title  = "Expert",
        window = {'!', w=W, h=W},
        world  = {'!', w=L, h=L},
    }
    pico.set.expert(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=4, y=4, w=8, h=8, anc='NW'})
    shot("guide-28")

    pico.output.present()
    shot("guide-29")

    finish()
end

print("Done! Generated all guide images.")
