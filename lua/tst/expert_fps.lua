require 'pico.check'

pico.init(true)
pico.set.window { title="Expert FPS" }

-- test 1: returns ms
local ms = pico.set.expert(true, 40)
assert(ms == 25)

-- test 2: fps=0 returns 0
local ms0 = pico.set.expert(true, 0)
assert(ms0 == 0)

-- test 3: frame timing
pico.set.expert(true, 40)
local t0 = pico.get.now()
for i = 1, 4 do
    pico.input.event()
end
local elapsed = pico.get.now() - t0
assert(elapsed>=100 and elapsed<=110)

-- test 4: backward compat (no fps arg)
local ms1 = pico.set.expert(true)
assert(ms1 == 0)

pico.init(false)
