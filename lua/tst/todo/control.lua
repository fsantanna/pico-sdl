local pico = require 'pico'

pico.init(true)
pico.set.window { title="Control" }

local r = {'%', x=0.5, y=0.5, w=0, h=0.2, anc='C'}

while true do
    pico.output.draw.text("Hello!", r)
    pico.input.event()
    pico.output.clear()
end

pico.init(false)
