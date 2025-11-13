package.cpath = package.cpath .. ";../?.so"

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

--[[
    {
        print("line dimming");
        Pico_Pos p1 = {pos.x-20, pos.y-6};
        Pico_Pos p2 = {pos.x+20, pos.y+6};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_line(p1, p2);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("line_dimmed");
            }
        }
    }

    {
        print("poly dimming");
        Pico_Pos poly[] = {{5, 5}, {59, 10}, {20, 31}};
        for (int a = 255; a > 0; a-=5) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,0,0,a});
            pico_output_draw_poly(poly, 3);
            pico_input_delay(50);
            if (a == 120) {
                _pico_check("poly_dimmed");
            }
        }
    }
}

]]

pico.init(false)
