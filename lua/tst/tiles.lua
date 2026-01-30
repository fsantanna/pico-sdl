require 'pico.check'

pico.init(true)

-- 4x4 grid of 4x4 pixel tiles = 16x16 logical world
-- 160x160 physical window
local phy  = {'!', w=160, h=160}
local log  = {'#', w=4, h=4}
local tile = {w=4, h=4}
pico.set.view {
    title  = "Tiles",
    window = phy,
    world  = log,
    tile   = tile,
}

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
    local r = {'#', x=1, y=1, w=1, h=1, anc='NW'}
    pico.output.draw.buffer(white, r)
    pico.check("tiles-01")
end

-- Test 2: tile (2,2) with C anchor - centered in tile
do
    print("tile (2,2) C anchor")
    pico.output.clear()
    local r = {'#', x=2, y=2, w=1, h=1, anc='C'}
    pico.output.draw.buffer(white, r)
    pico.check("tiles-02")
end

-- Test 3: 2x2 tiles at (1,1) with NW anchor
do
    print("2x2 tiles (1,1) NW anchor")
    pico.output.clear()
    local r = {'#', x=1, y=1, w=2, h=2, anc='NW'}
    pico.output.draw.buffer(white, r)
    pico.check("tiles-03")
end

-- Test 4: 2x2 tiles centered at (2.5, 2.5)
do
    print("2x2 tiles (2.5,2.5) C anchor")
    pico.output.clear()
    local r = {'#', x=2.5, y=2.5, w=2, h=2, anc='C'}
    pico.output.draw.buffer(white, r)
    pico.check("tiles-04")
end

-- Test 5: mouse position in tile mode
-- Note: cannot warp mouse from Lua, but verify get_mouse accepts '#' mode
do
    print("mouse tile mode check")
    local pos = {'#', x=0, y=0}
    pico.get.mouse(pos)
    assert(type(pos.x) == 'number' and type(pos.y) == 'number')
end

pico.init(false)
