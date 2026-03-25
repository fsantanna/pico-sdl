require 'pico.check'

pico.init(true)
pico.set.window { title="Expert" }

local r = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
pico.output.draw.rect(r)
pico.check("expert-01")

pico.set.expert(true)
pico.output.clear()
pico.check("expert-02")

pico.output.present()
pico.check("expert-03")

-- test: delay returns dt
local dt = pico.input.delay(100)
assert(dt>=100 and dt<=110)  -- 110 b/c of valgrind

pico.init(false)
