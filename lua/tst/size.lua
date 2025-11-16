local pico = require 'pico'

pico.init(true)

pico.set.title "Size - Fullscreen"

local size = pico.get.size()
local pt1  = pico.pos(50, 50)
local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}

print("no fullscreen - less pixels")
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

print("ok fullscreen - less pixels")
pico.set.size(true, false)
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

print("no fullscreen - more pixels")
pico.set.size(size.phy, {x=128,y=72})
local pt2  = pico.pos(50, 50)
local rct2 = {x=pt2.x, y=pt2.y, w=32, h=18}
pico.output.clear()
pico.output.draw.rect(rct2)

pico.input.event('key.dn')

print("no fullscreen - less pixels")
pico.set.size({x=640,y=360}, size.log)
pico.output.clear()
pico.output.draw.rect(rct1)

pico.input.event('key.dn')

pico.init(false)
