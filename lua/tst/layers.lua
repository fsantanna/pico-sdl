require 'pico.check'

pico.init(true)

-- create bg layer (32x32)
print("create background layer")
pico.layer.empty('!', nil, "background", {w=32, h=32})

-- create ui layer (48x48)
print("create ui layer")
pico.layer.empty('!', nil, "ui", {w=48, h=48})

-- draw on bg layer (red background)
print("draw on background layer directly")
pico.set.effect { "background", color={'!', r=0x80, g=0x00, b=0x00} }
pico.output.clear("background")
pico.set.pencil { "background", color='white' }
pico.output.draw.rect("background", {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.output.clear()
pico.output.draw.layer("background", {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-01")

-- draw on ui layer (blue background)
print("draw on ui layer")
pico.set.effect { "ui", color={'!', r=0x00, g=0x00, b=0x80} }
pico.output.clear("ui")
pico.set.pencil { "ui", color='green' }
pico.output.draw.rect("ui", {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})
pico.output.clear()
pico.output.draw.layer("ui", {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
pico.check("layers-02")

-- composite layers onto main
print("draw layers onto main")
pico.set.effect { color='black' }
pico.output.clear()
pico.output.draw.layer("background", {'%', x=1.0/3, y=1.0/3, h=1.0/3, anchor='C'})
pico.output.draw.layer("ui", {'%', x=2.0/3, y=2.0/3, w=1.0/3, anchor='C'})
pico.check("layers-03")

-- present works
print("present works")
pico.output.present()

pico.init(false)
