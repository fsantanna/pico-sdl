require 'pico.check'

pico.init(true)

local phy = {'!', w=200, h=200}
local log = {'!', w=20, h=20}
pico.set.window { dim=phy }
pico.set.view { dim=log }

local r = {'!', x=10-2, y=10-2, w=4, h=4, anc='NW'}

print("pos_vs_rect")
for y = r.y-1, r.y+r.h do
    for x = r.x-1, r.x+r.w do
        pico.output.clear()
        pico.set.color.draw('white')
        pico.output.draw.rect(r)

        local p = {'!', x=x, y=y, anc='NW'}
        pico.set.color.draw('red')
        pico.output.draw.pixel(p)

        local on = pico.vs.pos_rect(p, r)
        --print(on and "in" or "out")
        pico.input.delay(10)

        if x==7 and y==7 then
            assert(not on)
            pico.check("collide_raw-01")
        end
        if x==8 and y==8 then
            assert(on)
            pico.check("collide_raw-02")
        end
        if x==9 and y==9 then
            assert(on)
            pico.check("collide_raw-03")
        end
        if x==10 and y==10 then
            assert(on)
            pico.check("collide_raw-04")
        end
        if x==11 and y==11 then
            assert(on)
            pico.check("collide_raw-05")
        end
        if x==12 and y==12 then
            assert(not on)
            pico.check("collide_raw-06")
        end
    end
end

print("rect_vs_rect")
for y = r.y-r.h, r.y+r.h do
    for x = r.x-r.w, r.x+r.w do
        pico.output.clear()
        pico.set.color.draw('white')
        pico.output.draw.rect(r)

        local r2 = {'!', x=x, y=y, w=4, h=4, anc='NW'}
        pico.set.color.draw('red')
        pico.output.draw.rect(r2)

        local overlap = pico.vs.rect_rect(r2, r)
        --print(overlap and "overlap" or "naw")
        pico.input.delay(10)

        print(x .. "/" .. y)
        if x==4 and y==4 then
            assert(not overlap)
            pico.check("collide_raw-07")
        end
        if x==5 and y==5 then
            assert(overlap)
            pico.check("collide_raw-08")
        end
        if x==11 and y==11 then
            assert(overlap)
            pico.check("collide_raw-09")
        end
        if x==12 and y==12 then
            assert(not overlap)
            pico.check("collide_raw-10")
        end
    end
end

pico.init(false)
