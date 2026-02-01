local pico = require 'pico'

pico.init(true)

pico.set.window { title="Pct-To-Pos" }

do
    print("centered rect")
    local pt  = pico.pos(50, 50)
    local rct = {x=pt.x, y=pt.y, w=32, h=18}
    pico.output.clear()
    pico.output.draw.rect(rct)
    pico.input.event('key.dn')
    --_pico_check("pct_rect50")
end

do
    print("rect at 30%")
    pico.output.clear()

    local pt1  = pico.pos(30, 30)
    local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}
    pico.output.draw.rect(rct1)

    print("red centered under white")
    local pt2  = pico.pos({x=50,y=50}, rct1)
    local rct2 = {x=pt2.x, y=pt2.y, w=16, h=9}
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect(rct2)

    pico.input.event('key.dn')
    --_pico_check("pct_rect30_inner50")
end

do
    print("rect at 50% anchored by bottom-right")
    pico.output.clear()

    local pt1  = pico.pos(50, 50)
    local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}
    pico.set.anchor.pos('right', 'bottom')
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect(rct1)

    print("red anchored by top-left under 0% of white")
    local pt2  = pico.pos({x=0,y=0}, rct1)
    local rct2 = {x=pt2.x, y=pt2.y, w=16, h=9}
    pico.set.anchor.pos('left', 'top')
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect(rct2)

    pico.input.event('key.dn')
    --_pico_check("pct_rect50_inner0")
end

do
    print("rect at -10/-10 top-left (4x7 rect on top)")
    pico.output.clear()

    local pt  = pico.pos(-10, -10)
    local rct = {x=pt.x, y=pt.y, w=10, h=10}
    pico.set.anchor.pos('left', 'top')
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect(rct)

    pico.input.event('key.dn')
    --_pico_check("pct_rect-10")
end

do
    print("rect at 110/110 bottom-right (symmetric to previous)")
    pico.output.clear()

    local pt  = pico.pos(110, 110)
    local rct = {x=pt.x, y=pt.y, w=10, h=10}
    pico.set.anchor.pos('right', 'bottom')
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect(rct)

    pico.input.event('key.dn')
    --_pico_check("pct_rect110")
end

do
    print("rect at 50% anchored by bottom-right")
    pico.output.clear()

    local pt1  = pico.pos(50, 50)
    local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}
    pico.set.anchor.pos('center', 'middle')
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect(rct1)

    print("red anchored by top-left under -10% of white")
    local pt2  = pico.pos({x=-10,y=-10}, rct1)
    local rct2 = {x=pt2.x, y=pt2.y, w=16, h=9}
    pico.set.anchor.pos('left', 'top')
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect(rct2)

    pico.input.event('key.dn')
    --_pico_check("pct_rect50_inner-10")
end

do
    print("rect at 50%")
    pico.output.clear()

    local pt1  = pico.pos(50, 50)
    local rct1 = {x=pt1.x, y=pt1.y, w=32, h=18}
    pico.set.anchor.pos('center', 'middle')
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect(rct1)

    print("red anchored by top-left under 110% of white (symmetric to previous)")
    local pt2  = pico.pos({x=110,y=110}, rct1)
    local rct2 = {x=pt2.x, y=pt2.y, w=16, h=9}
    pico.set.anchor.pos('right', 'bottom')
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect(rct2)

    pico.input.event('key.dn')
    --_pico_check("pct_rect50_inner110")
end

pico.init(false)
