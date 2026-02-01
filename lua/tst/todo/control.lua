local pico = require 'pico'

pico.init(true)

pico.set.window { title="Control" }

local pt = pico.pos(50, 50)
while true do
    pico.output.draw.text(pt, "Hello!")
    pico.input.event()
    pico.output.clear()
end

pico.init(false)
