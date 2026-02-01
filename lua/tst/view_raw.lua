require 'pico.check'

pico.init(true)

-- TITLE
print("title: set and get")
pico.set.window { title="Test Title" }
local win = pico.get.window()
assert(win.title == "Test Title", "title mismatch: " .. tostring(win.title))
pico.set.window { title="View Raw" }

local win = pico.get.window()
local all = pico.get.view()
local window = win.dim
local world = all.world
assert(window.w == 500 and window.h == 500)
assert(world.w == 100 and world.h == 100)
window[1] = '!'
world[1] = '!'

-- SIZE (using set.dim for both window and world)
pico.set.dim(window)
local all = pico.get.view()
assert(all.world.w==window.w and all.world.h==window.h)
pico.set.view { world=world }   -- fallback after test

-- WORLD - bigger
print("shows lower-left X, center rect, center/up-right line")
for i = 0, 49 do
    world.w = world.w + 1
    world.h = world.h + 1
    pico.set.view { world = world }
    pico.output.clear()
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect({'!',
        x = world.w//2 - 5,
        y = world.h//2 - 5,
        w = 10,
        h = 10,
        anc = 'NW'
    })
    pico.set.color.draw(255, 0, 0)
    pico.output.draw.line(
        {'%', x=0.5, y=0.5},
        {'%', x=1.0, y=0}
    )
    pico.input.delay(10)
    if i == 0 then
        pico.check("view_raw-01")
    elseif i == 48 then
        pico.check("view_raw-02")
    end
end

-- SCROLL - left/up
print("scrolls left/up")
for i = 0, 49 do
    pico.set.view {
        source = {'!', x=i, y=i, w=100, h=100, anc='NW'}
    }
    pico.output.clear()
    pico.output.draw.text("Uma frase bem grande...", {'!', x=10, y=50, w=0, h=10, anc='NW'})
    pico.input.delay(10)
    if i == 0 then
        pico.check("view_raw-03")
    elseif i == 49 then
        pico.check("view_raw-04")
    end
end

pico.init(false)
