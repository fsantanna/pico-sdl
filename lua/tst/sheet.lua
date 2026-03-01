require 'pico.check'

pico.init(true)

-- Create a 4x4 "sprite sheet" layer with colored quadrants
pico.layer.empty("sheet", {w=4, h=4})
pico.set.layer("sheet")
pico.set.color.clear('black')
pico.output.clear()

-- Top-left: red
pico.set.color.draw('red')
pico.output.draw.rect({'!', x=0, y=0, w=2, h=2, anchor='NW'})
-- Top-right: green
pico.set.color.draw('green')
pico.output.draw.rect({'!', x=2, y=0, w=2, h=2, anchor='NW'})
-- Bottom-left: blue
pico.set.color.draw('blue')
pico.output.draw.rect({'!', x=0, y=2, w=2, h=2, anchor='NW'})
-- Bottom-right: white
pico.set.color.draw('white')
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
pico.layer.sub("tl", "sheet",
    {'!', x=0, y=0, w=2, h=2, anchor='NW'})
pico.layer.sub("tr", "sheet",
    {'!', x=2, y=0, w=2, h=2, anchor='NW'})
pico.layer.sub("bl", "sheet",
    {'!', x=0, y=2, w=2, h=2, anchor='NW'})
pico.layer.sub("br", "sheet",
    {'!', x=2, y=2, w=2, h=2, anchor='NW'})

-- Draw sub-layers swapped: TL->BR, TR->BL, BL->TR, BR->TL
pico.set.color.clear('black')
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
pico.set.color.clear('black')
pico.output.clear()
pico.set.color.draw('red')
pico.output.draw.rect({'!', x=0, y=0, w=2, h=2, anchor='NW'})
pico.set.color.draw('green')
pico.output.draw.rect({'!', x=2, y=0, w=2, h=2, anchor='NW'})
pico.output.screenshot("../../tst/out/sheet-grid.png")

local names = pico.layer.images("grid",
    "../../tst/out/sheet-grid.png",
    {'#', w=2, h=1})
assert(#names == 2)
assert(names[1] == "grid-1")
assert(names[2] == "grid-2")

pico.set.color.clear('black')
pico.output.clear()
pico.output.draw.layer("grid-1",
    {'%', x=0.75, y=0.5, w=0.5, h=1, anchor='C'})
pico.output.draw.layer("grid-2",
    {'%', x=0.25, y=0.5, w=0.5, h=1, anchor='C'})
pico.check("sheet-03")

-- Test 4: pico.layer.images (explicit form)
print("Test 4: explicit form")
local names2 = pico.layer.images("expl",
    "../../tst/out/sheet-grid.png", {
    '!',
    left  = {'!', x=0, y=0, w=2, h=2,
             anchor='NW'},
    right = {'!', x=2, y=0, w=2, h=2,
             anchor='NW'},
})
assert(#names2 == 2)
table.sort(names2)
assert(names2[1] == "expl-left")
assert(names2[2] == "expl-right")

pico.set.color.clear('black')
pico.output.clear()
pico.output.draw.layer("expl-left",
    {'%', x=0.75, y=0.5, w=0.5, h=1, anchor='C'})
pico.output.draw.layer("expl-right",
    {'%', x=0.25, y=0.5, w=0.5, h=1, anchor='C'})
pico.check("sheet-04")

pico.init(false)
