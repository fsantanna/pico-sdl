require 'pico.check'

pico.init(true)
pico.set.title("pixels")
pico.set.view(-1, {w=100, h=100}, nil, {w=5, h=5}, nil, nil)

do
    local pixels = {
        {x=1, y=1}, {x=2, y=1}, {x=3, y=1},
        {x=1, y=2}, {x=2, y=2}, {x=3, y=2},
        {x=1, y=3}, {x=2, y=3}, {x=3, y=3}
    }
    for i = 1, 9 do
        pico.output.clear()
        pico.output.draw.pixels({table.unpack(pixels, 1, i)})
        print(i .. " pixels")
        pico.input.delay(10)
    end
    pico.check("pixels-01")
end

do
    local pixels = {
        {'NW', x=0, y=0},
        {'NE', x=1, y=0},
        {'SW', x=0, y=1},
        {'SE', x=1, y=1},
    }
    pico.output.draw.pixels(pixels)
    pico.check("pixels-02")
end

pico.init(false)
