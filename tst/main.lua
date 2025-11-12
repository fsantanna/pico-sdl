package.cpath = package.cpath .. ";../src/lua/?.so"

local pico = require 'pico'

pico.init(true)

local pt = pico.pos { x=50, y=50 }
assert(pt.x==32 and pt.y==18)
print "shows dark screen"

--[[

print "waits any key press"
pico.input.event('key.up')

-- TITLE
print 'changes window title to "Testing..."'
pico.set.title "Testing..."
pico.input.delay(2000)

-- SOUND
print "plays sound"
pico.output.sound "start.wav"
pico.input.delay(2000)

-- CLEAR
pico.set.color.clear { r=0xFF, g=0xFF, b=0xFF, a=0xFF }
pico.output.clear();

print "shows white screen"
pico.input.delay(2000)

-- DRAW_IMAGE
pico.set.anchor.draw { x='center', y='middle' }
pico.output.draw.image(pt, "open.png")

print "shows centered image"
pico.input.delay(2000)

-- DRAW_PIXEL/RECT/OVAL
pico.set.color.clear { r=0x00, g=0x00, b=0x00, a=0xFF }
pico.set.color.draw { r=0xFF, g=0xFF, b=0xFF, a=0xFF }
pico.output.clear();
pico.output.draw.pixel(pt);
local rct = pico.pos { x=75, y=25 }
pico.output.draw.rect { x=rct.x, y=rct.y, w=10, h=5 }
local ova = pico.pos { x=25, y=75 }
pico.output.draw.oval { x=ova.x, y=ova.y, w=5, h=10 }

print "shows oval -> pixel -> rect"
pico.input.delay(2000)

-- DRAW_TEXT
pico.output.draw.text(pt, "Hello!");

print "shows centered \"Hello!\" (on top of shapes)"
pico.input.delay(2000)
pico.output.clear()

]]

-- WRITE
local up = pico.pos {x=10, y=10}
pico.set.cursor(up)
pico.output.write "1 "
pico.input.delay(200)
pico.output.write "2 "
pico.input.delay(200)
pico.output.writeln "3"
pico.input.delay(200)
pico.output.writeln ""
pico.input.delay(200)
pico.output.writeln "pico"

print "shows 1 2 3 \\n \\n pico"
pico.input.delay(2000)

pico.init(false)
