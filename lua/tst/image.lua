local pico = require 'pico'

pico.init(true)

pico.set.title "Image - Size - Crop"

local cnt = pico.pos(50, 50)
pico.set.color.clear(0xFF,0xFF,0xFF)

pico.output.clear()
pico.output.draw.image(cnt,"open.png")
print("show big centered")
pico.input.event('key.dn')
--_pico_check("img_big_center")

local dim = pico.get.size.image("open.png")
assert(dim.x==48 and dim.y==48)

pico.output.clear()
pico.output.draw.image(cnt,"open.png",{x=10,y=10})
print("show small centered")
pico.input.event('key.dn')
--_pico_check("img_small_center")

pico.output.clear()
pico.output.draw.image(cnt,"open.png",{x=10,y=15})
print("show small/medium distorted")
pico.input.event('key.dn')
--_pico_check("img_smallmedium_distorted")

pico.output.clear()
pico.output.draw.image(cnt,"open.png",{x=15,y=0})
print("show medium normal")
pico.input.event('key.dn')
--_pico_check("img_medium_normal")

pico.output.clear()
pico.output.draw.image(cnt,"open.png",{x=0,y=10})
print("show small normal")
pico.input.event('key.dn')
--_pico_check("img_small_normal")

pico.output.clear()
pico.output.draw.image(cnt,"open.png",{x=0,y=0})
print("show big centered")
pico.input.event('key.dn')
--_pico_check("img_big_center2")

pico.output.clear()
pico.set.crop({x=9,y=9,w=30,h=30})
pico.output.draw.image(cnt,"open.png",{x=30,y=30})
print("show big croped")
pico.input.event('key.dn')
--_pico_check("img_big_cropped")

pico.output.clear()
pico.set.crop()
pico.output.draw.image(cnt,"open.png",{x=0,y=15})
print("show medium normal")
pico.input.event('key.dn')
--_pico_check("img_medium_normal2")

pico.init(false)
