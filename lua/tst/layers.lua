require 'pico.check'

pico.init(true)

-- get_layer returns nil (main layer)
print("get_layer returns nil initially")
local layer = pico.get.layer()
assert(layer == "world")

-- set_layer(nil) switches to main
print("set_layer(nil) keeps main layer")
pico.set.layer("world")
layer = pico.get.layer()
assert(layer == "world")

-- create bg layer (32x32)
print("create and switch to layer")
pico.layer.empty { mode='!', key="background", dim={'!', w=32, h=32} }
pico.set.layer("background")
layer = pico.get.layer()
assert(layer == "background")

-- create ui layer (48x48)
print("switch to another layer")
pico.layer.empty { mode='!', key="ui", dim={'!', w=48, h=48} }
pico.set.layer("ui")
layer = pico.get.layer()
assert(layer == "ui")

-- draw on bg layer (red background)
print("draw on layer (no auto-present)")
pico.set.layer("background")
pico.set.effect { color={'!', r=0x80, g=0x00, b=0x00} }
pico.output.clear()
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.set.layer("world")
pico.output.clear()
pico.output.draw.layer("background", {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-01")

-- draw on ui layer (blue background)
print("draw on ui layer")
pico.set.layer("ui")
pico.set.effect { color={'!', r=0x00, g=0x00, b=0x80} }
pico.output.clear()
pico.set.pencil { color='green' }
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.set.layer("world")
pico.output.clear()
pico.output.draw.layer("ui", {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-02")

-- switch back to main
print("switch back to main")
pico.set.layer("world")
layer = pico.get.layer()
assert(layer == "world")

-- composite layers onto main
print("draw layers onto main")
pico.set.effect { color='black' }
pico.output.clear()
pico.output.draw.layer("background", {'%', x=1.0/3, y=1.0/3, h=1.0/3, anchor='C'})
pico.output.draw.layer("ui", {'%', x=2.0/3, y=2.0/3, w=1.0/3, anchor='C'})
pico.check("layers-03")

-- present works on main
print("present works on main")
pico.output.present()

-- rect-as-dim shortcut: 4th arg with x/y sets both dim and scene.target
print("layer.empty: rect-as-dim shortcut sets dim + target")
do
    -- world is 100x100; '%' w=0.5,h=0.5 -> 50x50 layer
    local r = {'%', x=0.25, y=0.75, w=0.5, h=0.5, anchor='C'}
    pico.layer.empty { up="world", key="rect_shortcut", clear=true, target=r }
    pico.set.layer("rect_shortcut")
    local s = pico.get.scene()
    -- dim derived from w/h (rect's '%' resolved against parent world)
    assert(s.dim.w == 50 and s.dim.h == 50)
    -- target set to the full rect (mode + x/y/w/h preserved)
    assert(s.target[1]  == '%')
    assert(s.target.x == 0.25 and s.target.y == 0.75)
    assert(s.target.w == 0.5  and s.target.h == 0.5)
    pico.set.layer("world")
end

-- trailing rect on other creators: pixmap with target rect set in one call
print("layer.pixmap: trailing rect sets scene.target")
do
    local r = {'%', x=0.5, y=0.5, w=0.25, h=0.25, anchor='C'}
    local px = {{ {'!',r=0xFF,g=0,b=0}, {'!',r=0,g=0xFF,b=0} },
                { {'!',r=0,g=0,b=0xFF}, {'!',r=0xFF,g=0xFF,b=0} }}
    pico.layer.pixmap { up="world", key="px_with_target", pix=px, target=r }
    pico.set.layer("px_with_target")
    local s = pico.get.scene()
    assert(s.target[1] == '%')
    assert(s.target.x == 0.5 and s.target.y == 0.5)
    assert(s.target.w == 0.25 and s.target.h == 0.25)
    pico.set.layer("world")
end

pico.init(false)
