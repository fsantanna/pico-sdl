require 'pico.check'

pico.init(true)

-- pico.get.text
do
    local dim = pico.get.text("ABC", {'!', w=nil, h=10})
    assert(dim.w>0 and dim.h==10)

    local pct = {'%', w=nil, h=0.1}
    local dim = pico.get.text("ABC", pct)
    assert(pico.equal(pct.w, 0.12) and pico.equal(pct.h, 0.10))
    assert(dim.w==12 and dim.h==10)

    local pct = {'%', h=0.2}
    pico.get.text("ABC", pct)
    assert(pico.equal(pct.w, 0.24) and pico.equal(pct.h, 0.20))

    local p = {'%', h=0.2};
    pico.get.text("ABC", p);
    assert(pico.equal(p.w, 0.24) and pico.equal(p.h, 0.20));
end

-- numbers
--pico.get.text(10,10)
pico.output.draw.text(10, {'!', x=0, y=0, h=10, anchor='NW'})
pico.output.clear()

-- pico.draw.text
do
    local r = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-01")

    local r = {'%', x=0.5, y=0.5, h=0.2}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-02")
end

pico.output.clear()

-- pico.set.draw.font: DejaVuSans.ttf
do
    pico.set.draw{font="../../DejaVuSans.ttf"}

    local r = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-03")

    local r = {'%', x=0.5, y=0.5, h=0.2}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-04")
end

pico.init(false)
