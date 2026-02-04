require 'pico.check'

pico.init(true)
pico.set.window { title="Blend", dim={'!', w=640, h=360} }
pico.set.view { dim={'!', w=64, h=36} }

do
    print("image dimming")
    local r = {'%', x=0.5, y=0.5, w=0.5, h=0, anc='C'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.output.draw.image("open.png", r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-01")
        end
    end
end

do
    print("pixel dimming")
    local p = {'%', x=0.5, y=0.5, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.pixel(p)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-02")
        end
    end
end

do
    print("text dimming")
    local r = {'%', x=0.5, y=0.5, w=0, h=0.28, anc='C'}
    local z = {'!', x=6.296000, y=12.960000, w=51.408001, h=10.080000, anc='NW'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.rect(z);
        pico.output.draw.text("SOME TEXT", r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-03")
        end
    end
end

do
    print("rect dimming")
    local r = {'%', x=0.5, y=0.5, w=0.95, h=0.34, anc='C'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.rect(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-04")
        end
    end
end

do
    print("oval dimming")
    local r = {'%', x=0.5, y=0.5, w=0.95, h=0.34, anc='C'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.oval(r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-05")
        end
    end
end

do
    print("tri dimming")
    local p1 = {'%', x=0.50, y=0.33, anc='C'}
    local p2 = {'%', x=0.33, y=0.66, anc='C'}
    local p3 = {'%', x=0.66, y=0.66, anc='C'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.tri(p1, p2, p3)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-06")
        end
    end
end

do
    print("line dimming")
    local p1 = {'%', x=0.33, y=0.33, anc='C'}
    local p2 = {'%', x=0.66, y=0.66, anc='C'}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.line(p1, p2)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-07")
        end
    end
end

do
    print("poly dimming")
    local poly = {
        {'%', x=0.50, y=0.33, anc='C'},
        {'%', x=0.33, y=0.66, anc='C'},
        {'%', x=0.66, y=0.66, anc='C'},
    }
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.poly(poly)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-08")
        end
    end
end

pico.init(false)
