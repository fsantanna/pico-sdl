pico.init(true)

-- Layer 120x50 with 3 centered buttons equally spaced
local btn1 = {'%', x=0.25, y=0.50, w=0.15, h=0.30, anc='C'}
local btn2 = {'%', x=0.50, y=0.50, w=0.15, h=0.30, anc='C'}
local btn3 = {'%', x=0.75, y=0.50, w=0.15, h=0.30, anc='C'}

pico.layer.empty('!', "A", {w=120, h=50})
pico.set.layer("A")
pico.set.color.clear('navy')
pico.output.clear()
pico.set.color.draw('white')
pico.output.draw.rect(btn1)
pico.output.draw.rect(btn2)
pico.output.draw.rect(btn3)
pico.set.layer()

-- Draw layer at bottom-right, 35%x35% of screen (distorted)
local r = {'%', x=0.99, y=0.99, w=0.35, h=0.35, anchor='SE'}
pico.set.color.clear('black')
pico.output.clear()
pico.output.draw.layer("A", r)

-- no collision
print("no collision (394,355)")
do
    pico.set.mouse({'w', x=394, y=355})
    local pct = pico.get.mouse('%', r)
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos_rect(pos, btn1))
    assert(not pico.vs.pos_rect(pos, btn2))
    assert(not pico.vs.pos_rect(pos, btn3))
end

-- click 3
print("click 3 (457,431)")
do
    pico.set.mouse({'w', x=457, y=431})
    local pct = pico.get.mouse('%', r)
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos_rect(pos, btn1))
    assert(not pico.vs.pos_rect(pos, btn2))
    assert(    pico.vs.pos_rect(pos, btn3))
end

-- click 1
print("click 1 (362,405)")
do
    pico.set.mouse({'w', x=362, y=405})
    local pct = pico.get.mouse('%', r)
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(    pico.vs.pos_rect(pos, btn1))
    assert(not pico.vs.pos_rect(pos, btn2))
    assert(not pico.vs.pos_rect(pos, btn3))
end

-- click 2
print("click 2 (418,392)")
do
    pico.set.mouse({'w', x=418, y=392})
    local pct = pico.get.mouse('%', r)
    local pos = {'%', x=pct.x, y=pct.y}
    print(string.format("  pct %5.3f %5.3f", pct.x, pct.y))
    assert(not pico.vs.pos_rect(pos, btn1))
    assert(    pico.vs.pos_rect(pos, btn2))
    assert(not pico.vs.pos_rect(pos, btn3))
end

pico.init(false)
