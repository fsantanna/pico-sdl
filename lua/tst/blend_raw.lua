require 'pico.check'

pico.init(true)
pico.set.view {
    title  = "Blend",
    window = {'!', w=640, h=360},
    world  = {'!', w=64,  h=36},
}

do
    print("pixel dimming")
    local p = {'!', x=32, y=18, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.pixel(p)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-01")
        end
    end
end

do
    print("text dimming")
    local r = {'!', x=32-13, y=18-5, w=0, h=10, anc='NW'}
    pico.get.text("SOME TEXT", r)
    r.x = 32 - r.w//2
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.text("SOME TEXT", r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-02")
        end
    end
end

do
    print("rect dimming")
    local r = {'!', x=32-30, y=18-6, w=60, h=12, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.rect(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-03")
        end
    end
end

do
    print("oval dimming")
    local r = {'!', x=32-30, y=18-6, w=60, h=12, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.oval(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-04")
        end
    end
end

do
    print("tri dimming")
    local p1 = {'!', x=2,  y=12, anc='NW'}
    local p2 = {'!', x=2,  y=24, anc='NW'}
    local p3 = {'!', x=62, y=24, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.tri(p1, p2, p3)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-05")
        end
    end
end

do
    print("line dimming")
    local p1 = {'!', x=12, y=12, anc='NW'}
    local p2 = {'!', x=52, y=24, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.line(p1, p2)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-06")
        end
    end
end

do
    print("poly dimming")
    local poly = {
        {'!', x=5,  y=5, anc='NW'},
        {'!', x=59, y=10, anc='NW'},
        {'!', x=20, y=31, anc='NW'},
    }
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.poly(poly)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-07")
        end
    end
end

pico.init(false)
