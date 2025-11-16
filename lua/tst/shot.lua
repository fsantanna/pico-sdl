local pico = require 'pico'

pico.init(true)

pico.set.title "Screenshots"

pico.output.clear()

print("entire screen")
pico.output.draw.rect(10, 10, 20, 20)
assert(pico.output.screenshot())
pico.input.event 'key.dn'

print("entire screen - second.png")
pico.set.color.draw(200,0,0)
pico.output.draw.rect(30, 30, 10, 10)
assert(pico.output.screenshot("second.png") == "second.png")
pico.input.event 'key.dn'

print("part of screen")
pico.set.color.draw(0,200,0)
pico.output.draw.rect(40, 5, 10, 10)
assert(pico.output.screenshot({x=0, y=0, w=50, h=30}))
pico.input.event 'key.dn'

pico.set.zoom(200, 200)

print("zoomed screen")
pico.set.color.draw(0,200,0)
pico.output.draw.rect(40, 5, 10, 10)
assert(pico.output.screenshot())
pico.input.event 'key.dn'

print("part of zoomed screen")
pico.set.color.draw(0,200,0)
pico.output.draw.rect(0, 0, 10, 10)
assert(pico.output.screenshot({x=0, y=0, w=10, h=10}))
pico.input.event 'key.dn'

pico.init(false)
