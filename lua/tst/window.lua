require 'pico.check'

pico.init(true)
pico.set.expert(true, 0)    -- expert: window-direct draws + manual present

-- initial current layer is world (not window)
do
    print("init: current layer is world")
    assert(pico.get.layer() == "world")
end

-- switching to the predefined window layer
do
    print("set.layer(\"window\")")
    local old = pico.set.layer("window")
    assert(old == "world")
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

-- drawing: window direct red rect (NW-ish) + world blue (SE-ish).
-- Both in pct mode, centered, 40% × 40%:
--   red rect on window at (33%, 33%)
--   blue world target  at (66%, 66%)
-- The two regions slightly overlap; world composites over red in
-- the overlap.
do
    print("window red @33% + world blue @66%")

    -- configure world: blue, target at (66%, 66%)
    pico.set.layer("world")
    pico.set.scene { target={'%', x=0.66, y=0.66, w=0.4, h=0.4, anchor='C'} }
    pico.set.effect { color={'!', r=0x00, g=0x00, b=0xFF} }
    pico.output.clear()

    -- direct draw on window: gray bg + red rect at (33%, 33%)
    pico.set.layer("window")
    pico.set.effect { color={'!', r=0x80, g=0x80, b=0x80} }
    pico.output.clear()
    pico.set.pencil { color={'!', r=0xFF, g=0x00, b=0x00} }
    pico.output.draw.rect({'%', x=0.33, y=0.33, w=0.4, h=0.4, anchor='C'})

    pico.output.draw.layers()   -- expert: compose world onto window.tex
    pico.output.present()       -- expert: manual present before screenshot
    pico.check("window-01")
    pico.set.layer("world")
end

pico.init(false)
