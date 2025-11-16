local pico = require 'pico'

pico.init(true)

pico.set.title "Font"

local pt = pico.pos(50, 50)
pico.output.draw.text(pt, "Hello!")
pico.input.delay(1000)

pico_init(false)
