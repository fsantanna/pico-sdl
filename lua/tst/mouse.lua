package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Mouse"
pico.set.size({x=500,y=500}, {x=50,y=50})

function draw ()
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.pixel { x=0, y=0 }

    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.pixel { x=12, y=12 }

    pico.set.color.draw(0x00,0xFF,0x00)
    pico.output.draw.pixel { x=25, y=25 }

    pico.set.color.draw(0xAA,0xAA,0xAA,0xAA)
    pico.output.draw.pixel { x=37, y=37 }

    pico.set.color.draw(0x00,0x00,0xFF)
    pico.output.draw.pixel { x=49, y=49 }
end

pico.output.clear()
draw()
do
    local e = pico.input.event('mouse.button.dn')
    print(">>> ", e.x, e.y)
end

pico.set.zoom{x=200,y=200}
pico.output.clear()
draw()
do
    local e = pico.input.event('mouse.button.dn')
    print(">>> ", e.x, e.y)
end

pico.set.zoom{x=50,y=50}
pico.output.clear()
draw()
do
    local e = pico.input.event('mouse.button.dn')
    print(">>> ", e.x, e.y)
end

pico.init(false)
