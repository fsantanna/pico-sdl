require 'pico.check'

pico.init(true)
pico.set.window { title="Clip" }

local r1 = {'%', x=0.3, y=0.3, w=0.5, h=0.5}

do
    print("rect at 30%")
    pico.output.clear()
    pico.output.draw.rect(r1)
    pico.check("clip-01")
end

do
    print("gray background")
    pico.set.view { clip=r1 }
    pico.set.color.clear(0xCC, 0xCC, 0xCC)
    pico.output.clear()
    pico.check("clip-02")
end

do
    print("red centered under gray")
    local r2 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, up=r1}
    pico.set.color.draw(0xFF, 0x00, 0x00)
    pico.output.draw.rect(r2)
    pico.check("clip-03")
end

do
    print("blue centered under gray")
    local p3 = {'%', x=0.5, y=0.5, up=r1}
    pico.set.color.draw(0x00, 0x00, 0xFF)
    pico.output.draw.pixel(p3)
    pico.check("clip-04")
end

do
    print("yellow clipped under gray bottom right")
    local r4 = {'%', x=1, y=1, w=0.5, h=0.5, up=r1}
    pico.set.color.draw(0xFF, 0xFF, 0x00)
    pico.output.draw.rect(r4)
    pico.check("clip-05")
end

pico.init(false)
