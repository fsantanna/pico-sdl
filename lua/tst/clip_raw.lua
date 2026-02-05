require 'pico.check'

pico.init(true)
pico.set.window { title="Clip" }

do
    print("rect at 30%")
    pico.output.clear()
    local r1 = {'!', x=30-25, y=30-25, w=50, h=50, anc='NW'}
    pico.output.draw.rect(r1)
    pico.check("clip-01")
end

local r1 = {'!', x=30-25, y=30-25, w=50, h=50, anc='NW'}

do
    print("gray background")
    pico.set.view { clip=r1 }
    pico.set.color.clear({'!', r=0xCC, g=0xCC, b=0xCC})
    pico.output.clear()
    pico.check("clip-02")
end

do
    print("red centered under gray")
    local r2 = {'!', x=r1.x+r1.w/2-12, y=r1.y+r1.h/2-12, w=25, h=25, anc='NW'}
    pico.set.color.draw('red')
    pico.output.draw.rect(r2)
    pico.check("clip-03")
end

do
    print("blue centered under gray")
    local p3 = {'!', x=r1.x+r1.w/2, y=r1.y+r1.h/2, anc='NW'}
    pico.set.color.draw('blue')
    pico.output.draw.pixel(p3)
    pico.check("clip-04")
end

do
    print("yellow clipped under gray bottom right")
    local r4 = {'!', x=r1.x+r1.w-12, y=r1.y+r1.h-12, w=25, h=25, anc='NW'}
    pico.set.color.draw('yellow')
    pico.output.draw.rect(r4)
    pico.check("clip-05")
end

pico.init(false)
