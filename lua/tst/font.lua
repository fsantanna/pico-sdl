require 'pico.check'

pico.init(true)

-- pico.get.text
do
    local dim = pico.get.text("ABC", {w=0, h=10})
    assert(dim.w > 0 and dim.h == 10)
end
do
    local dim = pico.get.text("ABC", {'%', w=0, h=0.1})
    assert(dim.w > 0 and dim.h == 0.1)
end

do
    local r = {x=10, y=10, w=0, h=10}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-01")
end

do
    local r = {'C', x=0.5, y=0.5, w=0, h=0.2}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-02")
end

pico.init(false)
