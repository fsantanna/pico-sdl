local pico = require 'pico'

pico.init(true)

pico.set.window { title="Moving Around" }

local pos = pico.pos(50, 50)
while true do
    pico.set.draw { color='white' }
    pico.output.draw.pixel(pos)

    pico.input.delay(200)

    --pico.set.draw { color='black' }
    --pico.output.draw.pixel(pos)

    pos.x = pos.x + math.random(3) - 2
    pos.y = pos.y + math.random(3) - 2
end

pico.init(false)
