local pico = require 'pico'

pico.init(true)

pico.set.title "Blending..."

local pos = pico.pos(50,50)
local rct = {x=pos.x, y=pos.y, w=60, h=12}

do
    print "pixel dimming"
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.pixel(pos)
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("pixel_dimmed")
        }
        ]]
    end
end

do
    print "text dimming"
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.text(pos, "SOME TEXT");
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("text_dimmed")
        }
        ]]
    end
end

do
    print "rect dimming"
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.rect(rct);
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("rect_dimmed")
        }
        ]]
    end
end

do
    print "oval dimming"
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.oval(rct);
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("oval_dimmed")
        }
        ]]
    end
end

do
    print "tri dimming"
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.tri(rct);
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("tri_dimmed")
        }
        ]]
    end
end

do
    print "line dimming"
    local p1 = { x=pos.x-20, y=pos.y-6 }
    local p2 = { x=pos.x+20, y=pos.y+6 }
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.line(p1, p2);
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("line_dimmed")
        }
        ]]
    end
end

do
    print "poly dimming"
    local poly = {{x=5, y=5}, {x=59, y=10}, {x=20, y=31}};
    for a=255, 0, -5 do
        pico.output.clear()
        pico.set.color.draw { r=255, g=0, b=0, a=a }
        pico.output.draw.poly(poly)
        pico.input.delay(50)
        --[[
        if (a == 120) {
            _pico_check("poly_dimmed")
        }
        ]]
    end
end

pico.init(false)
