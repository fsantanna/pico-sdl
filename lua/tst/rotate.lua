package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Rotate"
pico.set.grid(true)
pico.set.size({x=500,y=500}, {x=100,y=100})
pico.set.anchor.draw('center', 'middle')
--pico.set.font(NULL, 16)

local pt   = pico.pos(50, 50)
local rect = {x=pt.x, y=pt.y, w=50, h=50}

do
    print("RECT")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.rect(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.rect(rect)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("RECT - anchor rotate 110%110%")
    pico.set.rotate(0)
    pico.output.clear()
    pico.set.anchor.rotate(110,110)
    pico.output.draw.rect(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.rect(rect)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("RECT - anchor rotate -10%-10%")
    pico.set.rotate(0)
    pico.output.clear()
    pico.set.anchor.rotate {x=-10,y=-10}
    pico.output.draw.rect(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.rect(rect)
        pico.input.delay(50)
    end
    pico.set.anchor.rotate(50,50)
    pico.input.delay(500)
end

do
    print("RECT")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.rect(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.rect(rect)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("OVAL")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.oval(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.oval(rect)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("TRI")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.tri(rect)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.tri(rect)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("LINE")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.line({x=pt.x-25,y=pt.y-25}, {x=pt.x+25,y=pt.y+25})
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.line({x=pt.x-25,y=pt.y-25}, {x=pt.x+25,y=pt.y+25})
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("TEXT")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.text(pt, "HEY")
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.text(pt, "HEY")
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("IMAGE")
    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.image(pt, "open.png")
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.image(pt, "open.png")
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

do
    print("BUFFER")

    local buffer = {
        {
            { r=0x00, g=0x00, b=0x00 },
            { r=0xFF, g=0xFF, b=0x00 },
            { r=0x00, g=0x00, b=0x00 },
        },
        {
            { r=0xFF, g=0x00, b=0x00 },
            { r=0x00, g=0xFF, b=0x00 },
            { r=0x00, g=0x00, b=0xFF },
        },
        {
            { r=0x00, g=0x00, b=0x00 },
            { r=0x00, g=0xFF, b=0xFF },
            { r=0x00, g=0x00, b=0x00 },
        },
    }

    pico.set.rotate(0)
    pico.output.clear()
    pico.output.draw.buffer(pt, buffer)
    pico.input.delay(500)
    for i=1, 72 do
        pico.set.rotate(pico.get.rotate() + 5)
        pico.output.clear()
        pico.output.draw.buffer(pt, buffer)
        pico.input.delay(50)
    end
    pico.input.delay(500)
end

pico.init(false)
