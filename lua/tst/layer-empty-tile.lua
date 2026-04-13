require 'pico.check'

pico.init(true)

pico.set.dim {'!', w=480, h=480}
pico.set.show { grid=false }

-- Layer "map": 20x15 cols/rows of 16x16 tiles -> 320x240 px texture.
-- view.tile must be set at birth so '#'-mode works without pico.set.view.
do
    print("layer map: 20x15 tiles of 16x16")
    pico.layer.empty(nil, "map", {w=20, h=15}, {w=16, h=16})
    pico.set.layer("map")
    pico.set.show { grid=true }
    pico.output.clear()
    pico.set.color.draw('red')
    pico.output.draw.rect {'#', x=1, y=1, w=1, h=1, anchor='NW'}
    pico.set.color.draw('green')
    pico.output.draw.rect {'#', x=6, y=4, w=4, h=2, anchor='NW'}
    pico.set.color.draw('blue')
    pico.output.draw.rect {'#', x=20, y=15, w=1, h=1, anchor='NW'}
    pico.set.layer(nil)
end

-- Test 1: draw layer "map" 1:1 on main
do
    print("draw map 1:1")
    pico.output.clear()
    pico.output.draw.layer("map", {'!', x=0, y=0, w=320, h=240, anchor='NW'})
    pico.check("layer-empty-tile-01")
end

-- Test 2: draw layer "map" scaled up
do
    print("draw map scaled")
    pico.output.clear()
    pico.output.draw.layer("map", {'!', x=0, y=0, w=480, h=360, anchor='NW'})
    pico.check("layer-empty-tile-02")
end

-- Layer "fx": no tile arg -> dim is pixels, view.tile stays {0,0}
do
    print("layer fx: plain pixel layer (no tile)")
    pico.layer.empty(nil, "fx", {w=64, h=64})
    pico.set.layer("fx")
    pico.output.clear()
    pico.set.color.draw('yellow')
    pico.output.draw.rect {'!', x=8, y=8, w=48, h=48, anchor='NW'}
    pico.set.layer(nil)
end

-- Test 3: draw both layers
do
    print("draw map + fx")
    pico.output.clear()
    pico.output.draw.layer("map", {'!', x=0,   y=0, w=320, h=240, anchor='NW'})
    pico.output.draw.layer("fx",  {'!', x=340, y=0, w=128, h=128, anchor='NW'})
    pico.check("layer-empty-tile-03")
end

pico.init(false)
