require 'pico.check'

pico.init(true)

local all = pico.get.view()
local window = all.window
local world = all.world
assert(window.w == 500 and window.h == 500)
assert(world.w == 100 and world.h == 100)

-- WORLD - bigger
print("shows lower-left X, center rect, center/up-right line")
for i = 0, 49 do
    world.w = world.w + 1
    world.h = world.h + 1
    pico.set.view { world = world }
    pico.output.clear()
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect({'!',
        x = world.w/2 - 5,
        y = world.h/2 - 5,
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
