local pico = require 'pico'

pico.init(true)

pico.set.size.window({x=160,y=160}, {x=16,y=16})
pico.output.clear()
pico.input.event 'key.up'
for i=1, 16 do
    pico.output.draw.pixel(i-1, i-1)
    pico.output.draw.pixel(16-i, i-1)
    pico.input.delay(100)
end
pico.input.delay(1000)

pico.init(false)
