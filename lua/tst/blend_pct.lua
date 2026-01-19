require 'pico.check'

pico.init(true)
pico.set.title("Blend")
pico.set.view {
    window = {w=640, h=360},
    world  = {w=64,  h=36},
}

do
    print("image dimming")
    local r = {'C', x=0.5, y=0.5, w=0.5, h=0}
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
    local p = {'NW', x=0.5, y=0.5}
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
    local r = {'C', x=0.5, y=0.5, w=0, h=0.28}
    for a = 255, 1, -5 do
        pico.output.clear()
        pico.set.alpha(a)
        pico.set.color.draw(255, 0, 0)
        pico.output.draw.text("SOME TEXT", r)
        pico.input.delay(10)
        if a == 120 then
            pico.check("blend_pct-03")
        end
    end
end

do
    print("rect dimming")
    local r = {'C', x=0.5, y=0.5, w=0.95, h=0.34}
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
    local r = {'C', x=0.5, y=0.5, w=0.95, h=0.34}
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
    local p1 = {'C', x=0.50, y=0.33}
    local p2 = {'C', x=0.33, y=0.66}
    local p3 = {'C', x=0.66, y=0.66}
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
    local p1 = {'C', x=0.33, y=0.33}
    local p2 = {'C', x=0.66, y=0.66}
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
        {'C', x=0.50, y=0.33},
        {'C', x=0.33, y=0.66},
        {'C', x=0.66, y=0.66},
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
