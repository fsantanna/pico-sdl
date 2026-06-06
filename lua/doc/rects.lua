pico.init(true)

pico.set.expert(true)
pico.set.window { title="2x Rects" }
pico.set.layer("window")
pico.set.scene { dim={'!', w=200, h=200} }
pico.set.layer("world")
pico.set.scene  { dim={'!', w=10,  h=10}  }

local R = pico.color.alpha('red',  0xCC)
local B = pico.color.alpha('blue', 0xCC)

local k = {'!', x=4, y=4}        -- arrow-key pixel (centered)
local m = {'!', x=5, y=5}        -- mouse pixel

while true do
    pico.output.clear()
    pico.set.pencil { color=R }
    pico.output.draw.pixel(m)
    pico.set.pencil { color=B }
    pico.output.draw.pixel(k)
    pico.output.present()

    local e = pico.input.event()
    assert(e, "no FPS set here")
    if e.tag == 'quit' then
        break
    elseif e.tag == 'mouse.motion' then
        m = pico.get.mouse '!'
    elseif e.tag == 'key.dn' then
        if     e.key == 'Up'    then k.y = k.y - 1
        elseif e.key == 'Down'  then k.y = k.y + 1
        elseif e.key == 'Left'  then k.x = k.x - 1
        elseif e.key == 'Right' then k.x = k.x + 1
        end
    end
end

pico.init(false)
