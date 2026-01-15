local pico = require 'pico'

pico.init(true)

pico.set.title "Size - Fullscreen"

local pt1  = pico.pos(50, 50)
local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}

print("no fullscreen - less pixels")
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

print("ok fullscreen - less pixels")
pico.set.fullscreen(true)
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

print("no fullscreen - more pixels")
pico.set.fullscreen(false)
pico.set.dim.world(128,72)
local pt2  = pico.pos(50, 50)
local rct2 = {x=pt2.x, y=pt2.y, w=32, h=18}
pico.output.clear()
pico.output.draw.rect(rct2)

pico.input.event('key.dn')

print("no fullscreen - less pixels")
pico.set.dim.window(640,360)
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

do
    print("500x500 - phy=log - centered 250x250 rect")
    pico.set.dim.window(500,500)
    pico.set.dim.world(500,500)
    pico.output.clear()
    local pt  = pico.pos(50, 50)
    local rct = {x=pt.x, y=pt.y, w=250, h=250}
    pico.output.draw.rect(rct)
    pico.input.event('key.dn')
end

pico.init(false)
