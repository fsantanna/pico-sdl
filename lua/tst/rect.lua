require 'pico.check'

pico.init(true)
pico.set.view { title="Rect" }

do
    print("centered rect")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("rect-01")
end

do
    print("rect at 30%")
    pico.output.clear()
    local r1 = {'%', x=0.3, y=0.3, w=0.5, h=0.5}
    pico.output.draw.rect(r1)

    print("red centered under white")
    local r2 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, up=r1}
    pico.set.color.draw(0xFF, 0x00, 0x00)
    pico.output.draw.rect(r2)

    pico.check("rect-02")
end

do
    print("rect at 50% anchored by bottom-right")
    pico.output.clear()

    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='SE'}
    pico.set.color.draw(0xFF, 0xFF, 0xFF)
    pico.output.draw.rect(r1)

    print("red anchored by top-left under 0% of white")
    local r2 = {'%', x=0, y=0, w=0.5, h=0.5, anc='NW', up=r1}
    pico.set.color.draw(0xFF, 0x00, 0x00)
    pico.output.draw.rect(r2)

    pico.check("rect-03")
end

do
    print("rect at 50% anchored by bottom-right")
    pico.output.clear()

    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    pico.set.color.draw(0xFF, 0xFF, 0xFF)
    pico.output.draw.rect(r1)

    print("red anchored by top-left under -10% of white")
    local r2 = {'%', x=-0.1, y=-0.1, w=0.5, h=0.5, anc='NW', up=r1}
    pico.set.color.draw(0xFF, 0x00, 0x00)
    pico.output.draw.rect(r2)

    pico.check("rect-04")
end

do
    print("rect at 50%")
    pico.output.clear()

    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    pico.set.color.draw(0xFF, 0xFF, 0xFF)
    pico.output.draw.rect(r1)

    print("red anchored by top-left under 110% of white (symmetric to previous)")
    local r2 = {'%', x=1.1, y=1.1, w=0.5, h=0.5, anc='SE', up=r1}
    pico.set.color.draw(0xFF, 0x00, 0x00)
    pico.output.draw.rect(r2)

    pico.check("rect-05")
end

pico.init(false)
