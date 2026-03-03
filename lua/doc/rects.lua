pico.init(true)
pico.set.window { dim={'!', w=200, h=200}, title="2x Rects" }
pico.set.view   { dim={'!', w=10,  h=10}  }
pico.set.expert(true)

local mx, my = 5, 5              -- mouse pixel (centered)
local kx, ky = 4, 4              -- arrow-key pixel
local pos = {'!', x=0, y=0}      -- reusable position for mouse query
local spd = 1                     -- arrow-key speed per frame

while true do
    pico.get.mouse(pos)
    mx, my = pos.x, pos.y

    pico.output.clear()
    pico.set.color.draw 'red'
    pico.output.draw.pixel {'!', x=mx, y=my}
    pico.set.color.draw 'blue'
    pico.output.draw.pixel {'!', x=kx, y=ky}
    pico.output.present()

    local e = pico.input.event()
    if e then
        if e.tag == 'quit' then
            break
        elseif e.tag == 'key.dn' then
            if     e.key == 'Up'    then ky = ky - spd
            elseif e.key == 'Down'  then ky = ky + spd
            elseif e.key == 'Left'  then kx = kx - spd
            elseif e.key == 'Right' then kx = kx + spd
            end
        end
    end
end

pico.init(false)
