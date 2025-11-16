local pico = require 'pico'

pico.init(true)

pico.set.title "Dimensions"

local p = pico.pos(50, 50)

for i=0, 100, 25 do
    pico.output.clear()
    local d = pico.dim(i, i)
    pico.output.draw.rect {x=p.x,y=p.y,w=d.x,h=d.y}
    print("size",i)
    --_pico_check(fmt)
    pico.input.event 'key.dn'
end

for i=0, 120, 20 do
    pico.output.clear()
    local r = {x=p.x,y=p.y,w=50,h=20}
    pico.set.color.draw(255,255,255)
    pico.output.draw.rect(r)

    local d
    if i%2 == 0 then
        d = pico.dim({x=i,y=i}, {x=r.w,y=r.h})
    else
        d = pico.dim(i,i, {x=r.w,y=r.h})
    end
    pico.set.color.draw(255,0,0,150)
    pico.output.draw.rect {x=p.x,y=p.y,w=d.x,h=d.y}
    print("size",i)
    --_pico_check(fmt)
    pico.input.event 'key.dn'
end

print("ASSERT ERROR EXPECTED:")
pico.dim(-1, -1)

pico.init(false)
