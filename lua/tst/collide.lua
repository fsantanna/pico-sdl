local pico = require 'pico'

pico.init(true)

pico.set.title "Collide"
-- Set view to 20x20 world
pico.set.view_raw(nil, nil, nil, {w=20, h=20}, nil, nil)

-- Center rectangle in raw coordinates
local r = { x=10, y=10, w=4, h=4 }

print "pos_vs_rect_raw - using raw coordinates"
for y=r.y-r.h, r.y+r.h do
    for x=r.x-r.w, r.x+r.w do
        pico.output.clear()
        pico.set.color.draw(255,255,255)
        pico.output.draw.rect_raw(r)

        local pt = {x=x, y=y}
        local hit = pico.collision.pos_rect_raw(pt, r)
        pico.set.color.draw(255,0,0)
        pico.output.draw.pixel_raw(pt)

        print(hit and "in" or "out")

        pico.input.event('key.dn')
    end
end

print "rect_vs_rect_raw - using raw coordinates"
for y=r.y-r.h, r.y+r.h do
    for x=r.x-r.w, r.x+r.w do
        pico.output.clear()
        pico.set.color.draw(255,255,255)
        pico.output.draw.rect_raw(r)

        local r2 = {x=x, y=y, w=4, h=4}
        local hit = pico.collision.rect_rect_raw(r2, r)
        pico.set.color.draw(255,0,0)
        pico.output.draw.rect_raw(r2)

        print(hit and "overlap" or "naw")

        pico.input.event('key.dn')
    end
end

print "pos_vs_rect_pct - using percentage coordinates"
local r_pct = {x=0.5, y=0.5, w=0.2, h=0.2, anchor=pico.anchor.C}

for y=0, 10 do
    for x=0, 10 do
        pico.output.clear()
        pico.set.color.draw(255,255,255)
        pico.output.draw.rect_pct(r_pct)

        local pt_pct = {x=x/10, y=y/10, anchor=pico.anchor.C}
        local hit = pico.collision.pos_rect_pct(pt_pct, r_pct)
        pico.set.color.draw(255,0,0)
        pico.output.draw.pixel_pct(pt_pct)

        print(hit and "in" or "out")

        pico.input.event('key.dn')
    end
end

pico.init(false)
