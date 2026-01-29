local pico = require 'pico'

pico.init(true)

pico.set.view { title="Ticks" }

local t0 = pico.get.ticks()
pico.input.delay(1000)
local t1 = pico.get.ticks()

print(t0, t1, t1-t0)
assert(t1-t0 >= 1000)

pico.init(false)
