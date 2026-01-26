require 'pico.check'

pico.init(true)
pico.set.title("Expert")

local r = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
pico.output.draw.rect(r)
pico.check("expert-01")

pico.set.expert(true)
pico.output.clear()
pico.check("expert-02")

pico.output.present()
pico.check("expert-03")

local a = pico.get.ticks();
pico.input.delay(100);
local b = pico.get.ticks();
assert(b>=a+100 and b<=a+110);  -- 110 b/c of valgrind

pico.init(false)
