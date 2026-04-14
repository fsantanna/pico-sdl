require 'pico'

pico.init(true)

-- Layer 120x50 with 3 centered buttons equally spaced
local btn1 = {'%', x=0.25, y=0.50, w=0.15, h=0.30, anc='C'}
local btn2 = {'%', x=0.50, y=0.50, w=0.15, h=0.30, anc='C'}
local btn3 = {'%', x=0.75, y=0.50, w=0.15, h=0.30, anc='C'}

pico.layer.empty('!', nil, "A", {w=120, h=50})
pico.set.layer("A")
pico.set.show { color='navy' }
pico.output.clear()
pico.set.color.draw('white')
pico.output.draw.rect(btn1)
pico.output.draw.rect(btn2)
pico.output.draw.rect(btn3)
pico.set.layer()

-- Draw layer at bottom-right, 35%x35% of screen (distorted)
local r = {'%', x=0.99, y=0.99, w=0.35, h=0.35, anchor='SE'}
pico.set.show { color='black' }
pico.output.clear()
pico.output.draw.layer("A", r)

-- Event loop
while true do
    local e = pico.input.event()
    if e.tag == 'quit' then
        break
    elseif e.tag == 'mouse.button.dn' then
        local win = pico.get.mouse('w')
        local pct = pico.get.mouse('%', r)
        local msg = string.format(">>> w %4.0f %4.0f | %% %5.3f %5.3f", win.x, win.y, pct.x, pct.y)

        local pos = {'%', x=pct.x, y=pct.y}
        if pico.vs.pos_rect(pos, btn1) then
            msg = msg .. " | click 1"
        elseif pico.vs.pos_rect(pos, btn2) then
            msg = msg .. " | click 2"
        elseif pico.vs.pos_rect(pos, btn3) then
            msg = msg .. " | click 3"
        end
        print(msg)
    end
end

pico.init(false)
