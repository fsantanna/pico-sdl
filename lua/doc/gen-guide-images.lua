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
    shot("guide-02-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.view {
        title  = "Hello!",
        grid   = false,
        window = {'!', w=200, h=200},
        world  = {'!', w=200, h=200},
    }
    shot("guide-02-02")
    pico.init(false)
end

do
    pico.init(true)
    shot("guide-02-04")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 3: Basic Drawing
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.pixel({'!', x=50, y=50})
    shot("guide-03-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    shot("guide-03-02")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    shot("guide-03-03")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=20, y=20, w=30, h=30})
    pico.output.draw.image('img/open.png', {'!', x=70, y=20, w=20, h=20})
    shot("guide-03-04")
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
    shot("guide-04-01")
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
    shot("guide-04-02")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 5: Positioning - Mode & Anchor
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C'})
    shot("guide-05-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.output.clear()
    pico.set.color.draw('red')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='NW'})
    pico.set.color.draw('green')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='C'})
    pico.set.color.draw('blue')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='SE'})
    shot("guide-05-02")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.view {
        window = {'!', w=200, h=200},
        world  = {'#', w=4, h=4},
        tile   = {w=25, h=25},
    }
    pico.output.clear()
    pico.output.draw.rect({'#', x=1, y=1, w=1, h=1})
    pico.output.draw.rect({'#', x=3, y=2, w=1, h=1})
    pico.output.draw.rect({'#', x=2, y=4, w=2, h=1})
    shot("guide-05-03")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 6: Advanced View
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.view {
        window = {'!', w=200, h=200},
        world  = {'!', w=200, h=200},
        grid   = false,
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=50, y=50, w=100, h=100})
    shot("guide-06-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.view {
        window = {'!', w=200, h=200},
        world  = {'!', w=200, h=200},
        grid   = false,
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=50, y=50, w=100, h=100})
    pico.set.view {
        world  = {'!', w=100, h=100},
    }
    shot("guide-06-02")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.view {
        window = {'!', w=200, h=200},
        world  = {'!', w=200, h=200},
        grid   = false,
    }
    pico.output.clear()
    pico.output.draw.rect({'!', x=50, y=50, w=100, h=100})
    pico.set.view {
        world  = {'!', w=200, h=200},
        source = {'!', x=50, y=50, w=200, h=200},
    }
    shot("guide-06-03")
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
    shot("guide-07-01")
    pico.init(false)
end

-------------------------------------------------------------------------------
-- Section 8: Expert Mode
-------------------------------------------------------------------------------

do
    pico.init(true)
    pico.set.expert(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=25, y=25, w=50, h=50})
    shot("guide-08-01")
    pico.init(false)
end

do
    pico.init(true)
    pico.set.expert(true)
    pico.output.clear()
    pico.output.draw.rect({'!', x=25, y=25, w=50, h=50})
    pico.output.present()
    shot("guide-08-02")
    pico.init(false)
end

print("Done!")
