require 'pico.check'

pico.init(true)
pico.set.window { title="Expert FPS" }

-- test 1: returns ms
local ms = pico.set.expert(true, 40)
assert(ms == 25)

-- test 2: fps=0 returns -1 (block forever)
local ms0 = pico.set.expert(true, 0)
assert(ms0 == -1)

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
assert(ms1 == -1)

-- test 5: fps=true (as fast as possible) returns ms=0
local ms2 = pico.set.expert(true, true)
assert(ms2 == 0)

-- test 6: fps=false returns ms=-1
local ms3 = pico.set.expert(true, false)
assert(ms3 == -1)

-- test 7: fps=true returns immediately, evt=nil
pico.set.expert(true, true)
local e1, dt1 = pico.input.event()
assert(e1 == nil)
assert(dt1 <= 5)

-- test 8: evt,dt from fps=40 timeout
pico.set.expert(true, 40)
local e2, dt2 = pico.input.event()
assert(e2 == nil)
assert(dt2>=25 and dt2<=30)

pico.init(false)
