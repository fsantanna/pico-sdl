require 'pico.check'

pico.init(true)

local phy = {'!', w=200, h=200}
local log = {'!', w=20, h=20}
pico.set.window { dim=phy }
pico.set.view { dim=log }

local r = {'%', x=0.5, y=0.5, w=0.5, h=0.5}

print("pos_vs_rect")
for y = -1, 10 do
    for x = -1, 10 do
        pico.output.clear()
        pico.set.color.draw(255, 255, 255)
        pico.output.draw.rect(r)

        local p = {'%', x=x/10.0, y=y/10.0, up=r}
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.pixel(p)

        local on = pico.vs.pos_rect(p, r)
        print(on and "in" or "out")

        if x==-1 and y==-1 then
            assert(not on)
            pico.check("collide_pct-01")
        end
        if x==0 and y==0 then
            assert(on)
            pico.check("collide_pct-02")
        end
        if x==9 and y==9 then
            assert(on)
            pico.check("collide_pct-03")
        end
        if x==10 and y==10 then
            assert(not on)
            pico.check("collide_pct-04")
        end

        pico.input.delay(10)
    end
end

print("rect_vs_rect - same anchor")
for y = -25, 125, 10 do
    for x = -25, 125, 10 do
        pico.output.clear()
        pico.set.color.draw(255, 255, 255)
        pico.output.draw.rect(r)

        local r2 = {'%', x=x/100.0, y=y/100.0, w=0.5, h=0.5, up=r}
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.rect(r2)

        local overlap = pico.vs.rect_rect(r2, r)
        print(overlap and "overlap" or "naw")

        if x==-25 and y==-25 then
            assert(not overlap)
            pico.check("collide_pct-05")
        end
        if x==-15 and y==-15 then
            assert(overlap)
            pico.check("collide_pct-06")
        end
        if x==115 and y==115 then
            assert(overlap)
            pico.check("collide_pct-07")
        end
        if x==125 and y==125 then
            assert(not overlap)
            pico.check("collide_pct-08")
        end

        pico.input.delay(10)
    end
end

print("rect_vs_rect - diff anchor")
for y = -5, 10 do
    for x = -5, 10 do
        pico.output.clear()
        pico.set.color.draw(255, 255, 255)
        pico.output.draw.rect(r)

        local r2 = {'%', x=x/10.0, y=y/10.0, w=0.5, h=0.5, anc='NW', up=r}
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.rect(r2)

        local overlap = pico.vs.rect_rect(r2, r)
        print(overlap and "overlap" or "naw")

        if x==-5 and y==-5 then
            assert(not overlap)
            pico.check("collide_pct-09")
        end
        if x==-4 and y==-4 then
            assert(overlap)
            pico.check("collide_pct-10")
        end
        if x==9 and y==9 then
            assert(overlap)
            pico.check("collide_pct-11")
        end
        if x==10 and y==10 then
            assert(not overlap)
            pico.check("collide_pct-12")
        end

        pico.input.delay(10)
    end
end

pico.init(false)
