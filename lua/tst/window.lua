require 'pico.check'

pico.init(true)

-- initial current layer is world (not window)
do
    print("init: current layer is world")
    assert(pico.get.layer() == "world")
end

-- switching to the predefined window layer
do
    print("set.layer(\"window\")")
    local prev = pico.set.layer("window")
    assert(prev == "world")
    assert(pico.get.layer() == "window")
    pico.set.layer("world")
end

-- dims: window=500x500 (physical), world=100x100 (logical)
do
    print("dims: window vs world")
    pico.set.layer("window")
    local w = pico.get.scene().dim
    assert(w.w == 500 and w.h == 500)
    pico.set.layer("world")
    local l = pico.get.scene().dim
    assert(l.w == 100 and l.h == 100)
end

-- drawing on window uses physical pixel coordinates
-- black background + red 100x100 rect at NW (50, 50)
do
    print("draw on window (physical pixels)")
    pico.set.layer("window")
    pico.set.effect { color={'!', r=0x00, g=0x00, b=0x00} }
    pico.output.clear()
    pico.set.pencil { color={'!', r=0xFF, g=0x00, b=0x00} }
    pico.output.draw.rect({'!', x=50, y=50, w=100, h=100, anchor='NW'})
    pico.check("window-01")
    pico.set.layer("world")
end

pico.init(false)
