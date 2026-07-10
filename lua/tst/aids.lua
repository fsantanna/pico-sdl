pico.init(true)
assert(pico.get.aids() == true)

pico.set.aids(false)
assert(pico.get.aids() == false)

-- aids off: quit is not auto-handled even in non-expert mode
pico.quit()
local e = pico.input.event('quit', 100)
assert(e)
assert(e.tag == 'quit')

pico.set.aids(true)
assert(pico.get.aids() == true)

pico.init(false)
