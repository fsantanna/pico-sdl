package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Anchoring..."
pico.set.size({x=200,y=200}, {x=10,y=10})

-- PIXELS
do
    print "centered pixel - 1dir/1baixo"
    pico.set.anchor.draw { x='center', y='middle' }
    local pt = pico.pos { x=50, y=50 }
    pico.output.clear()
    pico.output.draw.pixel(pt)
    pico.input.event('key.dn');
    --_pico_check("pixel50x50y_center")
end
do
    print "centered pixel - 1dir/1baixo"
    pico.set.anchor.draw { x='left', y='top' }
    local pt = pico.pos { x=50, y=50 }
    pico.output.clear()
    pico.output.draw.pixel(pt)
    pico.input.event('key.dn');
    --_pico_check("pixel50x50y_lefttop")
end
do
    print "centered pixel - 1esq/1cima"
    pico.set.anchor.draw { x='right', y='bottom' }
    local pt = pico.pos { x=50, y=50 }
    pico.output.clear()
    pico.output.draw.pixel(pt)
    pico.input.event('key.dn');
    --_pico_check("pixel50x50y_rightbottom");
end

-- RECTS
do
    print "centered rect - exact"
    pico.set.anchor.draw { x='center', y='middle' }
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_center")
end

pico.init(false)
