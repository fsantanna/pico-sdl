require 'pico.check'

pico.init(true)
pico.set.window { title="pixels", dim={'!', w=100, h=100} }
pico.set.view { dim={'!', w=5, h=5} }

do
    local pixels = {
        {'!', x=1, y=1, anchor='NW'}, {'!', x=2, y=1, anchor='NW'}, {'!', x=3, y=1, anchor='NW'},
        {'!', x=1, y=2, anchor='NW'}, {'!', x=2, y=2, anchor='NW'}, {'!', x=3, y=2, anchor='NW'},
        {'!', x=1, y=3, anchor='NW'}, {'!', x=2, y=3, anchor='NW'}, {'!', x=3, y=3, anchor='NW'}
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
        {'%', x=0, y=0, anchor='NW'},
        {'%', x=1, y=0, anchor='NE'},
        {'%', x=0, y=1, anchor='SW'},
        {'%', x=1, y=1, anchor='SE'},
    }
    pico.output.draw.pixels(pixels)
    pico.check("pixels-02")
end

pico.init(false)
