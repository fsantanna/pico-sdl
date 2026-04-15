require 'pico.check'

pico.init(true)
pico.set.window { dim={'!', w=640, h=360} }
pico.set.view { dim={'!', w=64, h=36} }

-- red background on main
pico.set.show { color={'!', r=0xFF, g=0x00, b=0x00} }
pico.output.clear()

-- overlay layer: blue clear (semi-transparent), white rect
pico.layer.empty('!', nil, "overlay", {w=64, h=36})
pico.set.layer("overlay")
pico.set.show { color={'!', r=0x00, g=0x00, b=0xFF, a=0x80} }
pico.output.clear()
pico.set.draw { color='white' }
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})

-- composite overlay on main
pico.set.layer(nil)
pico.output.draw.layer("overlay", {'%', x=0.5, y=0.5, w=0.75, h=0.75, anchor='C'})
pico.check("clear_alpha-01")

pico.init(false)
