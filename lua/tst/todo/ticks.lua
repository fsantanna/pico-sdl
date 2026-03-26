local pico = require 'pico'

pico.init(true)

pico.set.window { title="Ticks" }

local t0 = pico.get.now()
pico.input.delay(1000)
local t1 = pico.get.now()

print(t0, t1, t1-t0)
assert(t1-t0 >= 1000)

pico.init(false)
