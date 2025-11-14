package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Anchoring..."
pico.set.size({x=200,y=200}, {x=10,y=10})

-- PIXELS
do
    print "centered pixel - 1dir/1baixo"
    pico.set.anchor.draw('center', 'middle')
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
    pico.set.anchor.draw('center', 'middle')
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_center")
end
do
    print "topleft centered - 1lin/1col"
    pico.set.anchor.draw { x='left', y='top' }
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_lefttop")
end
do
    print "bottomright centered - 1lin/1col"
    pico.set.anchor.draw('right', 'bottom')
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_rightbottom");
end
do
    print "rightmiddle centered - 1col"
    pico.set.anchor.draw { x='right', y='middle' }
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_rightcenter")
end
do
    print "anchor 25%25% 20%20% - 1lin/1col"
    pico.set.anchor.draw { x=25, y=25 }
    local pt = pico.pos { x=20, y=20 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_rightcenter")
end

-- RECTS out of [0,100]
do
    print "anchor -25%-25% centered - touching border"
    pico.set.anchor.draw { x=-25, y=-25 }
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect20x20y_-25x-25y")
end
do
    print "anchor 125%125% centered - touching border"
    pico.set.anchor.draw { x=125, y=125 }
    local pt = pico.pos { x=50, y=50 }
    local rct = { x=pt.x, y=pt.y, w=4, h=4 }
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn');
    --_pico_check("rect50x50y_125x125y");
end

pico.init(false)
