require 'pico.check'

pico.init(true)
pico.set.title("pixels")
pico.set.view {
    window = {'!', w=100, h=100},
    world  = {'!', w=5, h=5}
}

do
    local pixels = {
        {'!', x=1, y=1, anc='NW'}, {'!', x=2, y=1, anc='NW'}, {'!', x=3, y=1, anc='NW'},
        {'!', x=1, y=2, anc='NW'}, {'!', x=2, y=2, anc='NW'}, {'!', x=3, y=2, anc='NW'},
        {'!', x=1, y=3, anc='NW'}, {'!', x=2, y=3, anc='NW'}, {'!', x=3, y=3, anc='NW'}
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
        {'%', x=0, y=0, anc='NW'},
        {'%', x=1, y=0, anc='NE'},
        {'%', x=0, y=1, anc='SW'},
        {'%', x=1, y=1, anc='SE'},
    }
    pico.output.draw.pixels(pixels)
    pico.check("pixels-02")
end

pico.init(false)
