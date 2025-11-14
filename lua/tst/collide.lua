package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Collide"
pico.set.size({x=200,y=200}, {x=20,y=20})

local pt = pico.pos(50, 50)
local r = { x=pt.x, y=pt.y, w=4, h=4 }

--[[
print "pos_vs_rect - same anchor"
pico.set.anchor.draw{x='right',y='bottom'}
for y=r.y-r.w, r.y+1 do
    for x=r.x-r.w, r.x+1 do
        pico.output.clear()
        pico.set.color.draw(255,255,255)
        pico.output.draw.rect(r)

        local pt = {x=x,y=y}
        local X = pico.vs.pos_rect(pt, r)
        pico.set.color.draw(255,0,0)
        pico.output.draw.pixel(pt)

        print(X and "in" or "out")

        pico.input.event('key.dn')
    end
end

print "pos_vs_rect_ext - px bottom-right, rct top-left"
for y=r.y, r.y+r.h+1 do
    for x=r.x, r.x+r.w+1 do
        pico.output.clear();
        pico.set.color.draw(255,255,255);
        pico.set.anchor.draw { x='left', y='top' }
        pico.output.draw.rect(r);

        local pt = {x=x, y=y};
        local X = pico.vs.pos_rect (
            pt, r,
            {x='right', y='bottom'},
            {x='left',  y='top'}
        )

        pico.set.color.draw(255,0,0);
        pico.set.anchor.draw {x='right', y='bottom'}
        pico.output.draw.pixel(pt);

        print(X and "in  " or "out ");

        pico.input.event('key.dn')
    end
end
]]

print "rect_vs_rect - same anchor"
pico.set.anchor.draw({x='left', y='top'})
for y=r.y-r.h, r.y+r.h do
    for x=r.x-r.w, r.x+r.w do
        pico.output.clear()
        pico.set.color.draw(255,255,255)
        pico.output.draw.rect(r)

        local r2 = {x=x,y=y,w=4,h=4}
        local X = pico.vs.rect_rect(r2, r)
        pico.set.color.draw(255,0,0)
        pico.output.draw.rect(r2)

        print(X and "overlap" or "naw")

        pico.input.event('key.dn')
    end
end

--[[
    puts("rect_vs_rect_ext - bottom-right, top-left");
    for (int y = r.y; y < r.y+2*r.h+1; y++) {
        for (int x = r.x; x < r.x+2*r.w+1; x++) {
            pico_output_clear();
            pico_set_color_draw((Pico_Color){255,255,255,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_output_draw_rect(r);

            Pico_Rect r2 = {x,y,4,4};
            int in = pico_rect_vs_rect_ext(r2, (Pico_Anchor){PICO_RIGHT,PICO_BOTTOM},
                                            r, (Pico_Anchor){PICO_LEFT,PICO_TOP});
            pico_set_color_draw((Pico_Color){255,0,0,255});
            pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
            pico_output_draw_rect(r2);

            puts(in ? "overlap" : "naw");

            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    puts("assert error");
    pico_set_rotate(10);
    pico_pos_vs_rect(pt, r);
]]

pico.init(false)
