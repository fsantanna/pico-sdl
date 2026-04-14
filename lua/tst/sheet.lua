require 'pico.check'

pico.init(true)

-- Create a 4x4 "sprite sheet" layer with colored quadrants
pico.layer.empty('!', nil, "sheet", {w=4, h=4})
pico.set.layer("sheet")
pico.set.show { color='black' }
pico.output.clear()

-- Top-left: red
pico.set.draw { color='red' }
pico.output.draw.rect({'!', x=0, y=0, w=2, h=2, anchor='NW'})
-- Top-right: green
pico.set.draw { color='green' }
pico.output.draw.rect({'!', x=2, y=0, w=2, h=2, anchor='NW'})
-- Bottom-left: blue
pico.set.draw { color='blue' }
pico.output.draw.rect({'!', x=0, y=2, w=2, h=2, anchor='NW'})
-- Bottom-right: white
pico.set.draw { color='white' }
pico.output.draw.rect({'!', x=2, y=2, w=2, h=2, anchor='NW'})

pico.set.layer(nil)

-- Shot 1: draw full layer (normal quadrants)
print("Test 1: full layer")
pico.output.clear()
pico.output.draw.layer("sheet",
    {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("sheet-01")

-- Create sub-layers for each quadrant
print("Test 2: explicit sub-layers (swapped)")
pico.layer.sub('!', nil, "tl", "sheet",
    {'!', x=0, y=0, w=2, h=2, anchor='NW'})
pico.layer.sub('!', nil, "tr", "sheet",
    {'!', x=2, y=0, w=2, h=2, anchor='NW'})
pico.layer.sub('!', nil, "bl", "sheet",
    {'!', x=0, y=2, w=2, h=2, anchor='NW'})
pico.layer.sub('!', nil, "br", "sheet",
    {'!', x=2, y=2, w=2, h=2, anchor='NW'})

-- Draw sub-layers swapped: TL->BR, TR->BL, BL->TR, BR->TL
pico.set.show { color='black' }
pico.output.clear()
pico.output.draw.layer("tl",
    {'%', x=0.75, y=0.75, w=0.5, h=0.5, anchor='C'})
pico.output.draw.layer("tr",
    {'%', x=0.25, y=0.75, w=0.5, h=0.5, anchor='C'})
pico.output.draw.layer("bl",
    {'%', x=0.75, y=0.25, w=0.5, h=0.5, anchor='C'})
pico.output.draw.layer("br",
    {'%', x=0.25, y=0.25, w=0.5, h=0.5, anchor='C'})
pico.check("sheet-02")

-- Test 3: pico.layer.images (grid form)
print("Test 3: grid form")
pico.set.show { color='black' }
pico.output.clear()
pico.set.draw { color='red' }
pico.output.draw.rect(
    {'%', x=0, y=0, w=0.5, h=1, anchor='NW'})
pico.set.draw { color='green' }
pico.output.draw.rect(
    {'%', x=0.5, y=0, w=0.5, h=1, anchor='NW'})
pico.output.screenshot("../../tst/out/sheet-grid.png")

local names = pico.layer.images(nil, "grid",
    "../../tst/out/sheet-grid.png",
    {'#', w=2, h=1})
assert(#names == 2)
assert(names[1] == "grid-01")
assert(names[2] == "grid-02")

pico.set.show { color='black' }
pico.output.clear()
pico.output.draw.layer("grid-01",
    {'%', x=0.75, y=0.5, w=0.5, h=1, anchor='C'})
pico.output.draw.layer("grid-02",
    {'%', x=0.25, y=0.5, w=0.5, h=1, anchor='C'})
pico.check("sheet-03")

-- Test 4: pico.layer.images (explicit form)
print("Test 4: explicit form")
local names2 = pico.layer.images(nil, "expl",
    "../../tst/out/sheet-grid.png", {
    '!',
    left  = {'%', x=0, y=0, w=0.5, h=1,
             anchor='NW'},
    right = {'%', x=0.5, y=0, w=0.5, h=1,
             anchor='NW'},
})
assert(#names2 == 2)
table.sort(names2)
assert(names2[1] == "expl-left")
assert(names2[2] == "expl-right")

pico.set.show { color='black' }
pico.output.clear()
pico.output.draw.layer("expl-left",
    {'%', x=0.25, y=0.5, w=0.5, h=1, anchor='C'})
pico.output.draw.layer("expl-right",
    {'%', x=0.75, y=0.5, w=0.5, h=1, anchor='C'})
pico.check("sheet-04")

pico.init(false)
