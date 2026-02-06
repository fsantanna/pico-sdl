require 'pico.check'

pico.init(true)
pico.set.window { title="Pct-To-Pos" }

do
    print("white centered rect")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("pos-01")
end

do
    pico.output.clear()

    print("white rect at 30%")
    local r1 = {'%', x=0.3, y=0.3, w=0.5, h=0.5}
    pico.output.draw.rect(r1)

    print("red centered under white")
    local r2 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, up=r1}
    pico.set.color.draw('red')
    pico.output.draw.rect(r2)

    pico.check("pos-02")
end

do
    pico.output.clear()

    print("rect at 50% anchored by bottom-right")
    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='SE'}
    pico.set.color.draw('white')
    pico.output.draw.rect(r1)

    print("red anchored by top-left under 0% of white")
    local r2 = {'%', x=0, y=0, w=0.5, h=0.5, anc='NW', up=r1}
    pico.set.color.draw('red')
    pico.output.draw.rect(r2)

    pico.check("pos-03")
end

do
    print("rect at -10/-10 top-left (4x7 rect on top)")
    pico.output.clear()
    local r = {'%', x=-0.1, y=-0.1, w=0.15, h=0.3, anc='NW'}
    pico.set.color.draw('white')
    pico.output.draw.rect(r)
    pico.check("pos-04")
end

do
    print("rect at 110/110 bottom-right (symmetric to previous)")
    pico.output.clear()
    local r = {'%', x=1.1, y=1.1, w=0.15, h=0.3, anc='SE'}
    pico.set.color.draw('white')
    pico.output.draw.rect(r)
    pico.check("pos-05")
end

do
    pico.output.clear()

    print("rect at 50% anchored by center")
    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    pico.set.color.draw('white')
    pico.output.draw.rect(r1)

    print("red anchored by top-left under -10% of white")
    local r2 = {'%', x=-0.1, y=-0.1, w=0.5, h=0.5, anc='NW', up=r1}
    pico.set.color.draw('red')
    pico.output.draw.rect(r2)

    pico.check("pos-06")
end

do
    pico.output.clear()

    print("rect at 50%")
    local r1 = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    pico.set.color.draw('white')
    pico.output.draw.rect(r1)

    print("red anchored by top-left under 110% of white (symmetric to previous)")
    local r2 = {'%', x=1.1, y=1.1, w=0.5, h=0.5, anc='SE', up=r1}
    pico.set.color.draw('red')
    pico.output.draw.rect(r2)

    pico.check("pos-07")
end

pico.init(false)
