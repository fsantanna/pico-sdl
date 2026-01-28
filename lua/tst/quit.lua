pico.init(true)
pico.set.expert(true)

pico.quit()

local e = pico.input.event('quit', 100)
assert(e)
assert(e.tag == 'quit')

pico.init(false)
