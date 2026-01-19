require 'pico.check'

pico.init(true)
pico.set.view {
    world = {w=1000, h=1000}
}

-- draw
do
    -- X at center
    pico.output.draw.line(
        {'C', x=0.45, y=0.45},
        {'C', x=0.55, y=0.55}
    )
    pico.output.draw.line(
        {'C', x=0.55, y=0.45},
        {'C', x=0.45, y=0.55}
    )

    -- RECT at center
    pico.set.style('stroke')
    pico.output.draw.rect({'C', x=0.5, y=0.5, w=0.25, h=0.25})
end

--[[
-- navigate: 3 up, 3 left, 3 minus, 2 right, 4 down, 2 plus
pico.check("navigate-01")
for i = 1, 3 do pico.input.push_key('up',    'ctrl'); pico.input.event() end
pico.check("navigate-02")
for i = 1, 3 do pico.input.push_key('left',  'ctrl'); pico.input.event() end
pico.check("navigate-03")
for i = 1, 3 do pico.input.push_key('minus', 'ctrl'); pico.input.event() end
pico.check("navigate-04")
for i = 1, 2 do pico.input.push_key('right', 'ctrl'); pico.input.event() end
pico.check("navigate-05")
for i = 1, 4 do pico.input.push_key('down',  'ctrl'); pico.input.event() end
pico.check("navigate-06")
for i = 1, 2 do pico.input.push_key('equals', 'ctrl'); pico.input.event() end
pico.check("navigate-07")
]]

while true do
    pico.input.delay(100)
end

pico.init(false)
