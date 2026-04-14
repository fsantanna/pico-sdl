pico.init(true)

pico.set {
    expert = true,
    window = { dim={'!', w=200, h=200}, title="2x Rects" },
    view   = { dim={'!', w=10,  h=10}  },
    alpha  = 0xCC,
}

local k = {'!', x=4, y=4}        -- arrow-key pixel (centered)
local m = {'!', x=5, y=5}        -- mouse pixel

while true do
    pico.output.clear()
    pico.set.draw { color='red' }
    pico.output.draw.pixel(m)
    pico.set.draw { color='blue' }
    pico.output.draw.pixel(k)
    pico.output.present()

    local e = pico.input.event()
    if e then
        if e.tag == 'quit' then
            break
        elseif e.tag == 'mouse.motion' then
            m.x, m.y = e.x, e.y
        elseif e.tag == 'key.dn' then
            if     e.key == 'Up'    then k.y = k.y - 1
            elseif e.key == 'Down'  then k.y = k.y + 1
            elseif e.key == 'Left'  then k.x = k.x - 1
            elseif e.key == 'Right' then k.x = k.x + 1
            end
        end
    end
end

pico.init(false)
