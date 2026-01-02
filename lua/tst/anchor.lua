local pico = require 'pico'

pico.init(true)

pico.set.title "Anchoring..."
pico.set.view_raw(nil, {w=200, h=200}, nil, {w=10, h=10}, nil, nil)

-- PIXELS with different anchors using _pct
do
    print "centered pixel (anchor C)"
    pico.output.clear()
    pico.output.draw.pixel_pct({x=0.5, y=0.5, anchor=pico.anchor.C})
    pico.input.event('key.dn')
end

do
    print "pixel at 50% with NW anchor"
    pico.output.clear()
    pico.output.draw.pixel_pct({x=0.5, y=0.5, anchor=pico.anchor.NW})
    pico.input.event('key.dn')
end

do
    print "pixel at 50% with SE anchor"
    pico.output.clear()
    pico.output.draw.pixel_pct({x=0.5, y=0.5, anchor=pico.anchor.SE})
    pico.input.event('key.dn')
end

-- RECTS with different anchors
do
    print "centered rect (anchor C)"
    pico.output.clear()
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.4, h=0.4, anchor=pico.anchor.C})
    pico.input.event('key.dn')
end

do
    print "rect at 50% with NW anchor"
    pico.output.clear()
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.4, h=0.4, anchor=pico.anchor.NW})
    pico.input.event('key.dn')
end

do
    print "rect at 50% with SE anchor"
    pico.output.clear()
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.4, h=0.4, anchor=pico.anchor.SE})
    pico.input.event('key.dn')
end

do
    print "rect at 50% with E anchor (right-middle)"
    pico.output.clear()
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.4, h=0.4, anchor=pico.anchor.E})
    pico.input.event('key.dn')
end

do
    print "rect at 20%,20% with custom anchor (0.25, 0.25)"
    pico.output.clear()
    pico.output.draw.rect_pct({
        x=0.2, y=0.2, w=0.4, h=0.4,
        anchor={x=0.25, y=0.25}
    })
    pico.input.event('key.dn')
end

-- RECTS with anchors outside [0,1]
do
    print "rect centered with anchor -0.25,-0.25 (extends beyond)"
    pico.output.clear()
    pico.output.draw.rect_pct({
        x=0.5, y=0.5, w=0.4, h=0.4,
        anchor={x=-0.25, y=-0.25}
    })
    pico.input.event('key.dn')
end

do
    print "rect centered with anchor 1.25,1.25 (extends beyond)"
    pico.output.clear()
    pico.output.draw.rect_pct({
        x=0.5, y=0.5, w=0.4, h=0.4,
        anchor={x=1.25, y=1.25}
    })
    pico.input.event('key.dn')
end

pico.init(false)
