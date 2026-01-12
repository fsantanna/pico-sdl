local pico = require 'pico'

pico.init(true)

pico.set.title "Screenshots"

pico.output.clear()

print("entire physical screen")
pico.output.draw.rect(10, 10, 20, 20)
assert(pico.output.screenshot())

print("entire physical screen - second.png")
pico.set.color.draw(200, 0, 0)
pico.output.draw.rect(30, 30, 10, 10)
assert(pico.output.screenshot("second.png") == "second.png")

print("part of physical screen (raw)")
pico.set.color.draw(0, 200, 0)
pico.output.draw.rect(40, 5, 10, 10)
assert(pico.output.screenshot_raw(nil, {x=0, y=0, w=250, h=150}))

print("part of physical screen (pct)")
pico.set.color.draw(0, 0, 200)
pico.output.draw.rect(50, 50, 10, 10)
assert(pico.output.screenshot_pct(nil, {x=0, y=0, w=0.5, h=0.3}))

pico.init(false)
print("All tests passed!")
