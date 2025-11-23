local pico = require 'pico'

pico.init(true)

local phy_log = pico.get.size.window()
local phy = phy_log.phy
local log = phy_log.log
assert(phy.x==640 and phy.y==360);
assert(log.x==64  and log.y==36 );

local pt = pico.pos(50,50)
assert(pt.x==32 and pt.y==18)
print "shows dark screen"

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
pico.set.color.clear(0xFF, 0xFF, 0xFF)
pico.output.clear()

print "shows white screen"
pico.input.delay(2000)

-- DRAW_IMAGE
pico.set.anchor.draw { x='center', y='middle' }
pico.output.draw.image(pt, "open.png")

print "shows centered image"
pico.input.delay(2000)

-- DRAW_PIXEL/RECT/OVAL
pico.set.color.clear(pico.color.black)
pico.set.color.draw(pico.color.white)
pico.output.clear()
pico.output.draw.pixel(pt)
local rct = pico.pos { x=75, y=25 }
pico.output.draw.rect { x=rct.x, y=rct.y, w=10, h=5 }
local ova = pico.pos { x=25, y=75 }
pico.output.draw.oval { x=ova.x, y=ova.y, w=5, h=10 }

print "shows oval -> pixel -> rect"
pico.input.delay(2000)

-- DRAW_TEXT
pico.output.draw.text(pt, "Hello!")

print "shows centered \"Hello!\" (on top of shapes)"
pico.input.delay(2000)
pico.output.clear()

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

-- MOUSE
do
    print "waits mouse click"
    local e2 = pico.input.event 'mouse.button.dn'
    print "shows pixel over mouse"
    pico.output.draw.pixel { x=e2.x, y=e2.y }
    pico.input.delay(2000)
end

-- EVENT
    pico.input.delay(2000)
print "waits more 2 seconds"
local e3 = pico.input.event(2000);

-- GRID=0
pico.set.grid(false)
print "disables grid"
pico.input.delay(2000)

-- EXPERT

pico.output.clear()                     -- TODO: should restart cursor?
pico.set.expert(true)
pico.set.cursor(up)
pico.output.writeln("expert")
print("shows expert")
pico.output.present()
pico.input.delay(2000)
pico.set.expert(false)
pico.output.clear()

-- DRAW_RECT
print("shows lower-left X, center rect, center/up-right line")
print("increases zoom")
for i=1, 20 do
    log.x = log.x - 1
    log.y = log.y - 1
    pico.set.size.window(false, log)
    local ct = pico.pos { x=50, y=50 }
    pico.output.clear()
    pico.set.color.draw { r=0xFF,g=0xFF,b=0xFF,a=0xFF }
    pico.output.draw.rect { x=ct.x, y=ct.y, w=10, h=10 }
    pico.set.color.draw { r=0xFF,g=0x00,b=0x00,a=0xFF }
    pico.output.draw.text(pico.pos{x=25,y=75}, "X")
    pico.output.draw.line(ct, pico.pos{x=100,y=0})
    pico.input.delay(250)
end
print("decreases zoom")
for i=1, 20 do
    log.x = log.x + 1
    log.y = log.y + 1
    pico.set.size.window(false, log)
    local ct = pico.pos { x=50, y=50 }
    pico.output.clear()
    pico.set.color.draw { r=0xFF,g=0xFF,b=0xFF,a=0xFF }
    pico.output.draw.rect { x=ct.x, y=ct.y, w=10, h=10 }
    pico.set.color.draw { r=0xFF,g=0x00,b=0x00,a=0xFF }
    pico.output.draw.text(pico.pos{x=25,y=75}, "X")
    pico.output.draw.line(ct, pico.pos{x=100,y=0})
    pico.input.delay(250)
end
pico.set.color.draw { r=0xFF, g=0xFF, b=0xFF, a=0xFF }

-- PAN

print("scrolls right/down")
for i=0, 19 do
    pico.set.scroll { x=10-i, y=10-i }
    pico.output.clear()
    pico.output.draw.text(pt, "Uma frase bem grande...")
    pico.input.delay(250)
end

pico.init(false)
