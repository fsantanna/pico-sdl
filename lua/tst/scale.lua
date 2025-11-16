local pico = require 'pico'

pico.init(true)

pico.set.title "Scale"
pico.set.grid(false)
pico.set.size({x=500,y=500}, {x=100,y=100})

local pt   = pico.pos(50, 50)
local rect = {x=pt.x, y=pt.y, w=30, h=30}

do
    pico.output.clear()
    pico.output.draw.rect(rect)
    print("RECT")
    pico.input.event('key.dn')
    --.pico.check("rect.scale.normal")
end

do
    pico.set.scale{x=150,y=150}
    pico.output.clear()
    pico.output.draw.rect(rect)
    print("RECT - scale up")
    pico.input.event('key.dn')
    --.pico.check("rect.scale.up")
end

do
    pico.set.scale(50,50)
    pico.output.clear()
    pico.output.draw.rect(rect)
    print("RECT - scale dn")
    pico.input.event('key.dn')
    --.pico.check("rect.scale.dn")
end

pico.set.anchor.draw('right', 'bottom')

do
    pico.output.clear()
    pico.set.scale(100,100)
    pico.output.draw.rect(rect)
    print("RECT (bottom-right)")
    pico.input.event('key.dn')
    --.pico.check("rect.br.scale.normal")
end

do
    pico.set.scale(150,150)
    pico.output.clear()
    pico.output.draw.rect(rect)
    print("RECT (bottom-right) - scale up")
    pico.input.event('key.dn')
    --.pico.check("rect.br.scale.up")
end

do
    pico.set.scale(50,50)
    pico.output.clear()
    pico.output.draw.rect(rect)
    print("RECT (bottom-right) - scale dn")
    pico.input.event('key.dn')
    --.pico.check("rect.br.scale.dn")
end

pico.init(false)
