require 'pico.check'

pico.init(true)

-- helper: window-pixel mouse set (NW anchor)
local function mouse_w (x, y)
    local prev = pico.set.layer("window")
    pico.set.mouse({'!', x=x, y=y, anchor='NW'})
    pico.set.layer(prev)
end

-- 4x4 grid of 4x4 pixel tiles = 16x16 logical world
-- 160x160 physical window
local phy  = {'!', w=160, h=160}
local log  = {'#', w=4, h=4}
local tile = {w=4, h=4}
pico.set.window { title="Tiles" }
pico.set.layer("window")
pico.set.scene { dim=phy }
pico.set.layer("world")
pico.set.scene { dim=log, tile=tile }

-- 4x4 pixel white tile
local white = {}
for y = 1, 4 do
    white[y] = {}
    for x = 1, 4 do
        white[y][x] = { r=0xFF, g=0xFF, b=0xFF, a=0xFF }
    end
end

-- Test 1: tile (1,1) with NW anchor - top-left corner
do
    print("tile (1,1) NW anchor")
    pico.output.clear()
    local r = {'#', x=1, y=1, w=1, h=1, anchor='NW'}
    pico.output.draw.pixmap("buf", white, r)
    pico.check("tiles-01")
end

-- Test 2: tile (2,2) with C anchor - centered in tile
do
    print("tile (2,2) C anchor")
    pico.output.clear()
    local r = {'#', x=2, y=2, w=1, h=1, anchor='C'}
    pico.output.draw.pixmap("buf", white, r)
    pico.check("tiles-02")
end

-- Test 3: 2x2 tiles at (1,1) with NW anchor
do
    print("2x2 tiles (1,1) NW anchor")
    pico.output.clear()
    local r = {'#', x=1, y=1, w=2, h=2, anchor='NW'}
    pico.output.draw.pixmap("buf", white, r)
    pico.check("tiles-03")
end

-- Test 4: 2x2 tiles centered at (2.5, 2.5)
do
    print("2x2 tiles (2.5,2.5) C anchor")
    pico.output.clear()
    local r = {'#', x=2.5, y=2.5, w=2, h=2, anchor='C'}
    pico.output.draw.pixmap("buf", white, r)
    pico.check("tiles-04")
end

-- Test 5: mouse position in tile mode
-- helper: each call site is exercised in both forms
--   * table form `{'#', anchor='NW'}` — preserves legacy NW numbers
--   * mode-string form — new C-anchor default; result shifted by -0.5
--     per axis in tile units (for '#': `flt/tile + 1 - anchor.x`)

-- phy (0,0) -> log (0,0) -> tile (1,1)
do
    print("mouse tile (1,1)")
    mouse_w(0, 0)
    local pos = pico.get.mouse({'#', anchor='NW'})
    assert(pos.x==1 and pos.y==1)
    local pos = pico.get.mouse(nil, {'#', anchor='NW'})
    assert(pos.x==1 and pos.y==1)
    local pos = pico.get.mouse('#')
    assert(pos.anchor.x==0.5 and pos.anchor.y==0.5)
    assert(pos.x==0.5 and pos.y==0.5)
end

-- phy (40,40) -> log (4,4) -> tile (2,2)
do
    print("mouse tile (2,2)")
    mouse_w(40, 40)
    local pos = pico.get.mouse({'#', anchor='NW'})
    assert(pos.x==2 and pos.y==2)
    local pos = pico.get.mouse('#')
    assert(pos.x==1.5 and pos.y==1.5)
end

-- phy (80,120) -> log (8,12) -> tile (3,4)
do
    print("mouse tile (3,4)")
    mouse_w(80, 120)
    local pos = pico.get.mouse({'#', anchor='NW'})
    assert(pos.x==3 and pos.y==4)
    local pos = pico.get.mouse('#')
    assert(pos.x==2.5 and pos.y==3.5)
end

pico.init(false)
