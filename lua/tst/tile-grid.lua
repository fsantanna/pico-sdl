require 'pico.check'

pico.init(true)

local phy = {'!', w=480, h=480}
pico.set.window { title="Grid", dim=phy }

-- Test 1: main layer, no tile mode, pixel grid only
do
    print("pixel grid only (no tile)")
    pico.set.show { grid=true }
    pico.set.view { dim={'!', w=16, h=16} }
    pico.output.clear()
    pico.set.draw { color='red' }
    pico.output.draw.rect {'!', x=4, y=4, w=8, h=8, anchor='NW'}
    pico.check("tile-grid-01")
end

-- Test 2: main layer, 4x4 tiles of 4x4 pixels, pixel + tile grid
do
    print("pixel + tile grid (4x4 tiles)")
    pico.set.show { grid=true }
    pico.set.view { dim={'#', w=4, h=4}, tile={w=4, h=4} }
    pico.output.clear()
    pico.set.draw { color='green' }
    pico.output.draw.rect {'#', x=1, y=1, w=2, h=2, anchor='NW'}
    pico.check("tile-grid-02")
end

-- Test 3: main layer, 8x8 tiles of 2x2 pixels, pixel + tile grid
do
    print("pixel + tile grid (8x8 tiles)")
    pico.set.show { grid=true }
    pico.set.view { dim={'#', w=8, h=8}, tile={w=2, h=2} }
    pico.output.clear()
    pico.set.draw { color='blue' }
    pico.output.draw.rect {'#', x=3, y=3, w=2, h=2, anchor='NW'}
    pico.check("tile-grid-03")
end

-- Test 4: main layer, non-square tiles (4x2), pixel + tile grid
do
    print("pixel + tile grid (non-square 4x2 tiles)")
    pico.set.show { grid=true }
    pico.set.view { dim={'#', w=4, h=8}, tile={w=4, h=2} }
    pico.output.clear()
    pico.set.draw { color='yellow' }
    pico.output.draw.rect {'#', x=2, y=4, w=1, h=2, anchor='NW'}
    pico.check("tile-grid-04")
end

-- Test 5: main layer, grid disabled, no grid lines
do
    print("grid disabled")
    pico.set.show { grid=false }
    pico.set.view { dim={'#', w=4, h=4}, tile={w=4, h=4} }
    pico.output.clear()
    pico.set.draw { color='red' }
    pico.output.draw.rect {'#', x=2, y=2, w=1, h=1, anchor='C'}
    pico.check("tile-grid-05")
end

-- Reset main to simple view for layer tests, grid off
pico.set.show { grid=false }
    pico.set.view { dim={'!', w=480, h=480} }

-- Layer A: 4x4 tiles of 4x4 pixels, grid enabled
do
    print("layer A: 4x4 tiles, grid on")
    pico.layer.empty(nil, "layerA", {w=16, h=16})
    pico.set.layer("layerA")
    pico.set.show { grid=true }
    pico.set.view { dim={'#', w=4, h=4}, tile={w=4, h=4} }
    pico.output.clear()
    pico.set.draw { color='red' }
    pico.output.draw.rect {'#', x=1, y=1, w=2, h=2, anchor='NW'}
    pico.set.layer(nil)
end

-- Layer B: 8x4 non-square tiles, grid enabled
do
    print("layer B: 8x4 non-square tiles, grid on")
    pico.layer.empty(nil, "layerB", {w=16, h=16})
    pico.set.layer("layerB")
    pico.set.show { grid=true }
    pico.set.view { dim={'#', w=8, h=4}, tile={w=2, h=4} }
    pico.output.clear()
    pico.set.draw { color='green' }
    pico.output.draw.rect {'#', x=5, y=2, w=2, h=1, anchor='NW'}
    pico.set.layer(nil)
end

-- Layer C: 4x4 tiles of 4x4 pixels, grid disabled
do
    print("layer C: 4x4 tiles, grid off")
    pico.layer.empty(nil, "layerC", {w=16, h=16})
    pico.set.layer("layerC")
    pico.set.show { grid=false }
    pico.set.view { dim={'#', w=4, h=4}, tile={w=4, h=4} }
    pico.output.clear()
    pico.set.draw { color='blue' }
    pico.output.draw.rect {'#', x=3, y=3, w=1, h=1, anchor='NW'}
    pico.set.layer(nil)
end

-- Test 6: draw all 3 layers side by side on main
do
    print("3 layers: A(grid on) B(grid on) C(grid off)")
    pico.output.clear()
    pico.output.draw.layer("layerA", {'!', x=0,   y=0, w=150, h=150, anchor='NW'})
    pico.output.draw.layer("layerB", {'!', x=160, y=0, w=150, h=150, anchor='NW'})
    pico.output.draw.layer("layerC", {'!', x=320, y=0, w=150, h=150, anchor='NW'})
    pico.check("tile-grid-06")
end

-- Test 7: layers at different scales
do
    print("3 layers: different scales")
    pico.output.clear()
    pico.output.draw.layer("layerA", {'!', x=0,   y=170, w=120, h=120, anchor='NW'})
    pico.output.draw.layer("layerB", {'!', x=140, y=170, w=200, h=200, anchor='NW'})
    pico.output.draw.layer("layerC", {'!', x=0,   y=310, w=240, h=120, anchor='NW'})
    pico.check("tile-grid-07")
end

pico.init(false)
