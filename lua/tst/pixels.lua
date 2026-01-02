local pico = require 'pico'

pico.init(true)

pico.set.title "Pixels"
-- Set view to 5x5 world
pico.set.view_raw(nil, nil, nil, {w=5, h=5}, nil, nil)

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
