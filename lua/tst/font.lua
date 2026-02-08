require 'pico.check'

pico.init(true)

-- pico.get.text
do
    local dim = pico.get.text("ABC", {'!', w=nil, h=10})
    assert(dim.w>0 and dim.h==10)

    local pct = {'%', w=nil, h=0.1}
    local dim = pico.get.text("ABC", pct)
    assert(pct.w*100//1==17 and pct.h*100//1==10)
    assert(dim.w==17 and dim.h==10)

    local pct = {'%', h=0.2}
    pico.get.text("ABC", pct)
    assert(pct.w*100//1==34 and pct.h*100//1==20)

    local p = {'%', h=0.2};
    pico.get.text("ABC", p);
    assert(p.w*100//1==34 and p.h*100//1==20);
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

pico.init(false)
