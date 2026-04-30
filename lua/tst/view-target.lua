require 'pico.check'

pico.init(true)
pico.set.window { title="View Target" }

-- 01: target = bottom-right quadrant, draw centered rect
do
    print("target: bottom-right quadrant")
    pico.set.view {
        target = {'%', x=1, y=1, w=0.5, h=0.5, anchor='SE'}
    }
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("view-target-01")
end

-- 02: same target, r2 with up=r1
do
    print("target: up chain")
    pico.output.clear()
    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    pico.output.draw.rect(r1)
    local r2 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, up=r1}
    pico.set.draw { color='red' }
    pico.output.draw.rect(r2)
    pico.check("view-target-02")
end

-- 03: reset target, draw same — should fill full window
do
    print("target: reset to full window")
    pico.set.view {
        target = {'%', x=0.5, y=0.5, w=1, h=1}
    }
    pico.output.clear()
    pico.set.draw { color='white' }
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("view-target-03")
end

-- 04: explicit layer with view target, drawn with rect=NULL
do
    print("target: explicit layer")
    pico.layer.empty(nil, "bg", {w=32, h=32})
    pico.set.layer("bg")
    pico.set.view {
        target = {'%', x=1, y=1, w=0.5, h=0.5, anchor='SE'}
    }
    pico.set.show { color={r=0x80, g=0x00, b=0x00} }
    pico.output.clear()
    pico.set.draw { color='white' }
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.layer(nil)
    pico.set.show { color='black' }
    pico.output.clear()
    pico.output.draw.layer("bg")
    pico.check("view-target-04")
end

-- 05: target h only, w=0 -> w inferred from 2:1 layer aspect
do
    print("target: h only, w inferred")
    pico.layer.empty(nil, "bg2", {w=80, h=40})
    pico.set.layer("bg2")
    pico.set.view {
        target = {'%', x=0.5, y=0.5, h=0.4}
    }
    pico.set.show { color={r=0x80, g=0x00, b=0x00} }
    pico.output.clear()
    pico.set.layer(nil)
    pico.set.show { color='black' }
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-05")
end

-- 06: target w only, h=0 -> h inferred from 2:1 layer aspect
do
    print("target: w only, h inferred")
    pico.set.layer("bg2")
    pico.set.view {
        target = {'%', x=0.5, y=0.5, w=0.4, h=0}
    }
    pico.set.layer(nil)
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-06")
end

-- 07: target w=h=0 -> full layer dim
do
    print("target: w=h=0, full layer dim")
    pico.set.layer("bg2")
    pico.set.view {
        target = {'%', x=0, y=0, w=0, anchor='NW'}
    }
    pico.set.layer(nil)
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-07")
end

pico.init(false)
