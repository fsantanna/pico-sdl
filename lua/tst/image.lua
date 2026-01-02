local pico = require 'pico'

pico.init(true)

pico.set.title "Image - Size - Crop"

pico.set.color.clear(0xFF,0xFF,0xFF)

-- Test 1: Show big centered image using _pct
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
    "open.png"
)
print("show big centered (original size)")
pico.input.event('key.dn')

-- Test 2: Get image dimensions
local dim = pico.get.dim.image("open.png")
assert(dim.x==48 and dim.y==48)

-- Test 3: Show small centered (10x10 logical pixels)
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0.15, h=0.28, anchor=pico.anchor.C},
    "open.png"
)
print("show small centered")
pico.input.event('key.dn')

-- Test 4: Show small/medium distorted
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0.15, h=0.42, anchor=pico.anchor.C},
    "open.png"
)
print("show small/medium distorted")
pico.input.event('key.dn')

-- Test 5: Show medium normal (proportional, half width)
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0.5, h=0, anchor=pico.anchor.C},
    "open.png"
)
print("show medium normal (proportional)")
pico.input.event('key.dn')

-- Test 6: Show small normal (proportional, 0.15 height)
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0, h=0.28, anchor=pico.anchor.C},
    "open.png"
)
print("show small normal (proportional)")
pico.input.event('key.dn')

-- Test 7: Show full size centered
pico.output.clear()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
    "open.png"
)
print("show big centered")
pico.input.event('key.dn')

-- Test 8: Show with crop
pico.output.clear()
pico.set.crop({x=9,y=9,w=30,h=30})
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0.47, h=0.83, anchor=pico.anchor.C},
    "open.png"
)
print("show big cropped")
pico.input.event('key.dn')

-- Test 9: Show medium normal after clearing crop
pico.output.clear()
pico.set.crop()
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0, h=0.42, anchor=pico.anchor.C},
    "open.png"
)
print("show medium normal")
pico.input.event('key.dn')

pico.init(false)
