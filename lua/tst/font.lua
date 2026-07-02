require 'pico.check'

pico.init(true)

-- pico.get.text
do
    local dim = pico.get.text({'!', w=nil, h=10}, "ABC")
    assert(dim.w>0 and dim.h==11)

    local pct = {'%', w=nil, h=0.1}
    local dim = pico.get.text(pct, "ABC")
    assert(dim == pct)
    assert((pct.w*100+0.5)//1==17 and (pct.h*100+0.5)//1==11)

    local pct = {'%', h=0.2}
    pico.get.text(pct, "ABC")
    assert(pct.w*100//1==36 and pct.h*100//1==20)

    local p = {'%', h=0.2};
    pico.get.text(p, "ABC");
    assert(p.w*100//1==36 and p.h*100//1==20);
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

-- pico.set.pencil.font: DejaVuSans.ttf
do
    pico.set.pencil{font="../../res/DejaVuSans.ttf"}

    local r = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-03")

    local r = {'%', x=0.5, y=0.5, h=0.2}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-04")
end

pico.init(false)
