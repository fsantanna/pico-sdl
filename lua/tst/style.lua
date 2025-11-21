local pico = require 'pico'

pico.init(true)

pico.set.title "Style"
pico.set.size.window({x=200,y=200}, {x=50,y=50})
pico.set.anchor.draw('center', 'middle')

-- stroke
do
    print "30/30 - stroke"
    pico.set.style 'stroke'
    local pt = pico.pos { x=30, y=30 }
    local rct = { x=pt.x, y=pt.y, w=10, h=10 }
    pico.output.draw.rect(rct)
end

-- fill
do
    print "80/80 - fill"
    pico.set.style 'fill'
    local pt = pico.pos { x=80, y=80 }
    local rct = { x=pt.x, y=pt.y, w=5, h=5 }
    pico.output.draw.rect(rct)
end

pico.input.event('key.dn');

-- error
print(pcall(pico.set.style,'xxx'))
print(pcall(pico.set.style,true))

pico.init(false)
