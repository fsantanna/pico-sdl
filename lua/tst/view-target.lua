require 'pico.check'

pico.init(true)
pico.set.window { title="View Target" }

-- 01: target = bottom-right quadrant, draw centered rect
do
    print("target: bottom-right quadrant")
    pico.set.scene {
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
    pico.set.pencil { color='red' }
    pico.output.draw.rect(r2)
    pico.check("view-target-02")
end

-- 03: reset target, draw same — should fill full window
do
    print("target: reset to full window")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=1, h=1}
    }
    pico.output.clear()
    pico.set.pencil { color='white' }
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("view-target-03")
end

-- 04: explicit layer with view target, drawn with rect=NULL
do
    print("target: explicit layer")
    pico.layer.empty(nil, "bg", {w=32, h=32})
    pico.set.scene("bg", {
        target = {'%', x=1, y=1, w=0.5, h=0.5, anchor='SE'}
    })
    pico.set.effect("bg", { color={r=0x80, g=0x00, b=0x00} })
    pico.output.clear("bg")
    pico.set.pencil("bg", { color='white' })
    pico.output.draw.rect("bg", {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.effect { color='black' }
    pico.output.clear()
    pico.output.draw.layer("bg")
    pico.check("view-target-04")
end

-- 05: target h only, w=0 -> w inferred from 2:1 layer aspect
do
    print("target: h only, w inferred")
    pico.layer.empty(nil, "bg2", {w=80, h=40})
    pico.set.scene("bg2", {
        target = {'%', x=0.5, y=0.5, h=0.4}
    })
    pico.set.effect("bg2", { color={r=0x80, g=0x00, b=0x00} })
    pico.output.clear("bg2")
    pico.set.effect { color='black' }
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-05")
end

-- 06: target w only, h=0 -> h inferred from 2:1 layer aspect
do
    print("target: w only, h inferred")
    pico.set.scene("bg2", {
        target = {'%', x=0.5, y=0.5, w=0.4, h=0}
    })
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-06")
end

-- 07: target w=h=0 -> full layer dim
do
    print("target: w=h=0, full layer dim")
    pico.set.scene("bg2", {
        target = {'%', x=0, y=0, w=0, anchor='NW'}
    })
    pico.output.clear()
    pico.output.draw.layer("bg2")
    pico.check("view-target-07")
end

pico.init(false)
