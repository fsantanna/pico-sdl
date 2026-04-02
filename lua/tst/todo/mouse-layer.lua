require 'pico'

pico.init(true)

-- Create layer 50x50, draw centered circle
pico.layer.empty('!', "A", {w=50, h=50})
pico.set.layer("A")
pico.set.color.clear('red')
pico.output.clear()
pico.output.draw.oval({'%', x=0.3, y=0.6, w=0.5, h=0.5, anc='C'})
pico.set.layer()

-- Draw layer distorted 2:1 on main
local r = {'%', x=0.6, y=0.3, w=0.8, h=0.4, anc='C'}
pico.set.color.clear('black')
pico.output.clear()
pico.output.draw.layer("A", r)

-- Event loop
while true do
    local e = pico.input.event()
    if e.tag == 'quit' then
        break
    elseif e.tag == 'mouse.button.dn' then
        local win = pico.get.mouse('w')
        local raw = pico.get.mouse('!', r)
        local pct = pico.get.mouse('%', r)
        print(string.format(
            ">>> w %4.0f %4.0f | ! %6.1f %6.1f | %% %5.3f %5.3f",
            win.x, win.y, raw.x, raw.y, pct.x, pct.y
        ))
    end
end

pico.init(false)
