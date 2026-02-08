require 'pico.check'

pico.init(true)
pico.set.window { title="Blend", dim={'!', w=640, h=360} }
pico.set.view { dim={'!', w=64, h=36} }

do
    print("pixel dimming")
    local p = {'!', x=32, y=18, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.pixel(p)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-01")
        end
    end
end

do
    print("text dimming")
    local d = {'!', w=0, h=10}
    pico.get.text("SOME TEXT", d)
    local r = {'!', x=32-d.w//2, y=18-5, w=d.w, h=10, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.text("SOME TEXT", r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-02")
        end
    end
end

do
    print("rect dimming")
    local r = {'!', x=32-30, y=18-6, w=60, h=12, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.rect(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-03")
        end
    end
end

do
    print("oval dimming")
    local r = {'!', x=32-30, y=18-6, w=60, h=12, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.oval(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-04")
        end
    end
end

do
    print("tri dimming")
    local p1 = {'!', x=2,  y=12, anchor='NW'}
    local p2 = {'!', x=2,  y=24, anchor='NW'}
    local p3 = {'!', x=62, y=24, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.tri(p1, p2, p3)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-05")
        end
    end
end

do
    print("line dimming")
    local p1 = {'!', x=12, y=12, anchor='NW'}
    local p2 = {'!', x=52, y=24, anchor='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
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
        {'!', x=5,  y=5, anchor='NW'},
        {'!', x=59, y=10, anchor='NW'},
        {'!', x=20, y=31, anchor='NW'},
    }
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw('red')
        pico.output.draw.poly(poly)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_raw-07")
        end
    end
end

pico.init(false)
