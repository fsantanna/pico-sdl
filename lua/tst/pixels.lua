local pico = require 'pico'

pico.init(true)

pico.set.title "Pixels"
pico.set.dim.window(100,100)
pico.set.dim.world(5,5)

local pixels = {
    {x=1, y=1}, {x=2, y=1}, {x=3, y=1},
    {x=1, y=2}, {x=2, y=2}, {x=3, y=2},
    {x=1, y=3}, {x=2, y=3}, {x=3, y=3},
}

print("press any key to start")
pico.input.event('key.dn')

while #pixels>0 do
    pico.output.clear()
    pico.output.draw.pixels(pixels)
    pico.input.event('key.dn')
    pixels[#pixels] = nil
end

pico.init(false)
