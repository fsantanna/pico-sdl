require 'pico.check'

pico.init(true)
pico.set.window { title="Clip x Expert" }
pico.set.pencil { style='fill' }

local full = {'%', x=0.5, y=0.5, w=1.0, h=1.0, anchor='C'}
local box  = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}

-- PLAIN: red full, then blue full clipped to a centered half-box
pico.set.pencil { color='red' }
pico.output.draw.rect(full)
pico.set.scene { clip=box }
pico.set.pencil { color='blue' }
pico.output.draw.rect(full)
pico.check("clip-expert-01")   -- expected: red with a centered blue box

-- EXPERT: identical sequence, app-style manual clear/present
pico.set.expert(true, 0)
pico.set.scene { clip=full }
pico.output.clear()
pico.set.pencil { color='red' }
pico.output.draw.rect(full)
pico.set.scene { clip=box }
pico.set.pencil { color='blue' }
pico.output.draw.rect(full)
pico.output.present()
pico.check("clip-expert-02")   -- BUG: full blue -> clip not applied

pico.init(false)
