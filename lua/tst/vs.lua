pico.init(true)

local phy = {'!', w=200, h=200}
local log = {'!', w=100, h=100}
pico.set.layer("window")
pico.set.scene { dim=phy }
pico.set.layer("world")
pico.set.scene { dim=log }

print "pico.vs.pos_rect (cur vs cur, 2-arg)"
do
    print('', 1)
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}
    local p = {'!', x=50, y=50, anchor='NW'}
    assert(pico.vs.pos_rect(p, r))

    print('', 2)
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}
    local p = {'!', x=10, y=10, anchor='NW'}
    assert(not pico.vs.pos_rect(p, r))
end

print "pico.vs.rect_rect (cur vs cur, 2-arg)"
do
    print('', 1)
    local r1 = {'!', x=20, y=20, w=40, h=40, anchor='NW'}
    local r2 = {'!', x=30, y=30, w=40, h=40, anchor='NW'}
    assert(pico.vs.rect_rect(r1, r2))

    print('', 2)
    local r1 = {'!', x=10, y=10, w=30, h=30, anchor='NW'}
    local r2 = {'!', x=50, y=50, w=30, h=30, anchor='NW'}
    assert(not pico.vs.rect_rect(r1, r2))
end

print "pico.vs.pos_pos (cur vs cur, 2-arg)"
do
    print('', 1)
    local p1 = {'!', x=50, y=50, anchor='NW'}
    local p2 = {'%', x=0.5, y=0.5, anchor='NW'}
    assert(pico.vs.pos_pos(p1, p2))

    print('', 2)
    local p1 = {'!', x=50, y=50, anchor='NW'}
    local p2 = {'!', x=51, y=50, anchor='NW'}
    assert(not pico.vs.pos_pos(p1, p2))
end

print "child layer projection (4-arg)"
do
    pico.layer.empty('world', 'sub_vs', true, {'!', w=50, h=50})

    print('', 'pos in sub vs rect in cur')
    -- sub:10,20 -> cur:20,40 (sub 50x50 fills cur 100x100, scale 2x)
    local p1 = {'!', x=10, y=20, anchor='NW'}
    local r2 = {'!', x=15, y=35, w=20, h=20, anchor='NW'}
    assert(pico.vs.pos_rect('sub_vs', p1, nil, r2))

    print('', 'pos in cur vs sub bounds (r2=nil)')
    local p1 = {'%', x=0.5, y=0.5, anchor='NW'}
    assert(pico.vs.pos_rect(nil, p1, 'sub_vs', nil))

    print('', 'pos_pos across layers')
    local p1 = {'!', x=10, y=20, anchor='NW'}
    local p2 = {'!', x=20, y=40, anchor='NW'}
    assert(pico.vs.pos_pos('sub_vs', p1, nil, p2))
end

print "rect_rect bounds (2-string)"
do
    -- two direct children of world; cur is world
    pico.layer.empty('world', 'sub_a', true, {'!', w=50, h=50})
    pico.layer.empty('world', 'sub_b', true, {'!', w=50, h=50})
    -- both default scene.dst fills world; bounds overlap
    assert(pico.vs.rect_rect('sub_a', 'sub_b'))
end

pico.init(false)
