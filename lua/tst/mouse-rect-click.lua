require 'pico.check'

pico.init(true)

-- Layer 120x50 with 3 centered buttons equally spaced
local btn1 = {'%', x=0.25, y=0.50, w=0.15, h=0.30, anc='C'}
local btn2 = {'%', x=0.50, y=0.50, w=0.15, h=0.30, anc='C'}
local btn3 = {'%', x=0.75, y=0.50, w=0.15, h=0.30, anc='C'}

pico.layer.empty('!', nil, "A", true, {'!', w=120, h=50})
pico.set.layer("A")
pico.set.effect { color='navy' }
pico.output.clear()
pico.set.pencil { color='white' }
pico.output.draw.rect(btn1)
pico.output.draw.rect(btn2)
pico.output.draw.rect(btn3)
pico.set.layer("world")

-- Draw layer at bottom-right, 35%x35% of screen (distorted)
local r = {'%', x=0.99, y=0.99, w=0.35, h=0.35, anchor='SE'}
pico.set.effect { color='black' }
pico.output.clear()
pico.output.draw.layer("A", r)
pico.check("mouse-rect-click-01")

-- re-express buttons within r (layer's composite rect) for collision
-- against mouse sampled in world-pct coords
local b1 = pico.xin.rect(r, btn1)
local b2 = pico.xin.rect(r, btn2)
local b3 = pico.xin.rect(r, btn3)

-- no collision
print("no collision (394,355)")
do
    pico.set.layer("window")
    pico.set.mouse({'!', x=394, y=355, anchor='C'})
    pico.set.layer("world")
    local pct = pico.get.mouse('%')
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos.rect(nil, pos, nil, b1))
    assert(not pico.vs.pos.rect(pos, nil, b2))
    assert(not pico.vs.pos.rect(nil, pos, b3))
    -- equivalent via explicit layer args (no set_layer dance)
    pico.set.mouse("window", {'!', x=394, y=355, anchor='C'})
    local pct2 = pico.get.mouse("world", '%')
    assert(math.abs(pct2.x - pct.x) < 0.001 and math.abs(pct2.y - pct.y) < 0.001)
    pico.set.pencil { color='red' }
    pico.output.draw.pixel(pos)
    pico.check("mouse-rect-click-02")
end

-- click 3
print("click 3 (457,431)")
do
    pico.set.layer("window")
    pico.set.mouse({'!', x=457, y=431, anchor='C'})
    pico.set.layer("world")
    local pct = pico.get.mouse('%')
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos.rect(pos, b1))
    assert(not pico.vs.pos.rect(nil, pos, b2))
    assert(    pico.vs.pos.rect(pos, nil, b3))
    pico.set.pencil { color='green' }
    pico.output.draw.pixel(pos)
    pico.check("mouse-rect-click-03")
end

-- click 1
print("click 1 (362,405)")
do
    pico.set.layer("window")
    pico.set.mouse({'!', x=362, y=405, anchor='C'})
    pico.set.layer("world")
    local pct = pico.get.mouse('%')
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(    pico.vs.pos.rect(pos, nil, b1))
    assert(not pico.vs.pos.rect(nil, pos, b2))
    assert(not pico.vs.pos.rect(pos, b3))
    pico.set.pencil { color='green' }
    pico.output.draw.pixel(pos)
    pico.check("mouse-rect-click-04")
end

-- click 2
print("click 2 (419,392)")
do
    pico.set.layer("window")
    pico.set.mouse({'!', x=419, y=392})
    pico.set.layer("world")
    local pct = pico.get.mouse('%')
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos.rect(pos, b1))
    assert(    pico.vs.pos.rect(pos, b2))
    assert(not pico.vs.pos.rect(pos, b3))
    pico.set.pencil { color='green' }
    pico.output.draw.pixel(pos)
    pico.check("mouse-rect-click-05")
end

pico.init(false)
