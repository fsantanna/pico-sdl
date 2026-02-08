require 'pico.check'

pico.init(true)

-- get_layer returns nil (main layer)
print("get_layer returns nil initially")
local layer = pico.get.layer()
assert(layer == nil)

-- set_layer(nil) switches to main
print("set_layer(nil) keeps main layer")
pico.set.layer(nil)
layer = pico.get.layer()
assert(layer == nil)

-- create bg layer (32x32)
print("create and switch to layer")
local bg = pico.layer.empty("background", {w=32, h=32})
assert(bg == "background")
pico.set.layer(bg)
layer = pico.get.layer()
assert(layer == bg)
assert(layer == "background")

-- create ui layer (48x48)
print("switch to another layer")
local ui = pico.layer.empty("ui", {w=48, h=48})
pico.set.layer(ui)
layer = pico.get.layer()
assert(layer == ui)
assert(layer == "ui")

-- draw on bg layer (red background)
print("draw on layer (no auto-present)")
pico.set.layer(bg)
pico.set.color.clear({'!', r=0x80, g=0x00, b=0x00})
pico.output.clear()
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.set.layer()
pico.output.clear()
pico.output.draw.layer(bg, {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-01")

-- draw on ui layer (blue background)
print("draw on ui layer")
pico.set.layer(ui)
pico.set.color.clear({'!', r=0x00, g=0x00, b=0x80})
pico.output.clear()
pico.set.color.draw('green')
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.set.layer(nil)
pico.output.clear()
pico.output.draw.layer(ui, {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-02")

-- switch back to main
print("switch back to main")
pico.set.layer(nil)
layer = pico.get.layer()
assert(layer == nil)

-- composite layers onto main
print("draw layers onto main")
pico.set.color.clear('black')
pico.output.clear()
pico.output.draw.layer(bg, {'%', x=1.0/3, y=1.0/3, w=1.0/3, h=1.0/3, anchor='C'})
pico.output.draw.layer(ui, {'%', x=2.0/3, y=2.0/3, w=1.0/3, h=1.0/3, anchor='C'})
pico.check("layers-03")

-- present works on main
print("present works on main")
pico.output.present()

pico.init(false)
