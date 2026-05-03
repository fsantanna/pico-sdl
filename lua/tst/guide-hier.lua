require 'pico.check'

pico.init(true)
pico.set.window { title="guide-hier-01" }
pico.output.clear()

pico.set.draw { color='white', style='stroke' }
pico.output.draw.rect { '%', x=0.5,  y=0.5,  w=1,    h=1    }   -- R
pico.output.draw.rect { '%', x=0.25, y=0.25, w=0.4,  h=0.3  }   -- I
pico.output.draw.rect { '%', x=0.7,  y=0.7,  w=0.5,  h=0.4  }   -- P
pico.output.draw.rect { '%', x=0.7,  y=0.6,  w=0.25, h=0.15 }   -- T1
pico.output.draw.rect { '%', x=0.7,  y=0.8,  w=0.25, h=0.15 }   -- T2

pico.set.draw { color='white', style='fill' }
pico.output.draw.text('R',  {'%', x=0.1,  y=0.99, h=0.2,  anchor='SW'})
pico.output.draw.text('P',  {'%', x=0.94, y=0.9,  h=0.12, anchor='SE'})
pico.output.draw.text('I',  {'%', x=0.25, y=0.25, h=0.15, anchor='C' })
pico.output.draw.text('T1', {'%', x=0.7,  y=0.6,  h=0.1,  anchor='C' })
pico.output.draw.text('T2', {'%', x=0.7,  y=0.8,  h=0.1,  anchor='C' })

pico.check("guide-07-04-01")

pico.init(false)
