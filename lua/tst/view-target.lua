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
    pico.set.color.draw('red')
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
    pico.set.color.draw('white')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("view-target-03")
end

-- 04: explicit layer with view target, drawn with rect=NULL
do
    print("target: explicit layer")
    pico.layer.empty("bg", {w=32, h=32})
    pico.set.layer("bg")
    pico.set.view {
        target = {'%', x=1, y=1, w=0.5, h=0.5, anchor='SE'}
    }
    pico.set.color.clear({r=0x80, g=0x00, b=0x00})
    pico.output.clear()
    pico.set.color.draw('white')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.layer(nil)
    pico.set.color.clear('black')
    pico.output.clear()
    pico.output.draw.layer("bg")
    pico.check("view-target-04")
end

pico.init(false)
