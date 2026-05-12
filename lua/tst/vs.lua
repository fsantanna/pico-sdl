pico.init(true)

local phy = {'!', w=200, h=200}
local log = {'!', w=100, h=100}
pico.set.layer("window")
pico.set.scene { dim=phy }
pico.set.layer("world")
pico.set.scene { dim=log }

print "pico.vs.pos.rect (cur vs cur, 2-arg)"
do
    print('', 1)
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}
    local p = {'!', x=50, y=50, anchor='NW'}
    assert(pico.vs.pos.rect(p, r))

    print('', 2)
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}
    local p = {'!', x=10, y=10, anchor='NW'}
    assert(not pico.vs.pos.rect(p, r))
end

print "pico.vs.rect.rect (cur vs cur, 2-arg)"
do
    print('', 1)
    local r1 = {'!', x=20, y=20, w=40, h=40, anchor='NW'}
    local r2 = {'!', x=30, y=30, w=40, h=40, anchor='NW'}
    assert(pico.vs.rect.rect(r1, r2))

    print('', 2)
    local r1 = {'!', x=10, y=10, w=30, h=30, anchor='NW'}
    local r2 = {'!', x=50, y=50, w=30, h=30, anchor='NW'}
    assert(not pico.vs.rect.rect(r1, r2))
end

print "pico.vs.pos.pos (cur vs cur, 2-arg)"
do
    print('', 1)
    local p1 = {'!', x=50, y=50, anchor='NW'}
    local p2 = {'%', x=0.5, y=0.5, anchor='NW'}
    assert(pico.vs.pos.pos(p1, p2))

    print('', 2)
    local p1 = {'!', x=50, y=50, anchor='NW'}
    local p2 = {'!', x=51, y=50, anchor='NW'}
    assert(not pico.vs.pos.pos(p1, p2))
end

print "child layer projection (4-arg)"
do
    pico.layer.empty('world', 'sub_vs', true, {'!', w=50, h=50})

    print('', 'pos in sub vs rect in cur')
    -- sub:10,20 -> cur:20,40 (sub 50x50 fills cur 100x100, scale 2x)
    local p1 = {'!', x=10, y=20, anchor='NW'}
    local r2 = {'!', x=15, y=35, w=20, h=20, anchor='NW'}
    assert(pico.vs.pos.rect('sub_vs', p1, nil, r2))

    print('', 'pos in cur vs sub bounds (r2=nil)')
    local p1 = {'%', x=0.5, y=0.5, anchor='NW'}
    assert(pico.vs.pos.rect(nil, p1, 'sub_vs', nil))

    print('', 'pos_pos across layers')
    local p1 = {'!', x=10, y=20, anchor='NW'}
    local p2 = {'!', x=20, y=40, anchor='NW'}
    assert(pico.vs.pos.pos('sub_vs', p1, nil, p2))
end

print "rect_rect bounds (2-string)"
do
    -- two direct children of world; cur is world
    pico.layer.empty('world', 'sub_a', true, {'!', w=50, h=50})
    pico.layer.empty('world', 'sub_b', true, {'!', w=50, h=50})
    -- both default scene.dst fills world; bounds overlap
    assert(pico.vs.rect.rect('sub_a', 'sub_b'))
end

print "flexible dispatch -- pos_pos"
do
    local p1 = {'!', x=10, y=10, anchor='NW'}
    local p2 = {'!', x=10, y=10, anchor='NW'}
    local q  = {'!', x=99, y=99, anchor='NW'}             -- mismatched pixel
    assert(    pico.vs.pos.pos(p1, p2))                   -- 2-arg
    assert(not pico.vs.pos.pos(p1, q))
    assert(    pico.vs.pos.pos(nil, p1, nil, p2))         -- explicit nils
    assert(not pico.vs.pos.pos(nil, p1, nil, q))
    assert(    pico.vs.pos.pos(nil, p1, p2))              -- trailing L2 omitted
    assert(not pico.vs.pos.pos(nil, p1, q))
    -- p1 in sub_vs (50x50 in cur 100x100, scale 2x) -> cur (20,40)
    local s1 = {'!', x=10, y=20, anchor='NW'}
    local s2 = {'!', x=20, y=40, anchor='NW'}
    assert(    pico.vs.pos.pos('sub_vs', s1, s2))         -- (L1, v1, v2)
    assert(not pico.vs.pos.pos('sub_vs', s1, q))
end

print "flexible dispatch -- pos_rect (defaults)"
do
    local p_in  = {'!', x=50,  y=50,  anchor='NW'}        -- inside cur 100x100
    local p_out = {'!', x=200, y=200, anchor='NW'}        -- outside cur
    assert(    pico.vs.pos.rect(p_in))                    -- r2 default = cur bounds
    assert(not pico.vs.pos.rect(p_out))
    assert(    pico.vs.pos.rect(p_in,  nil))
    assert(not pico.vs.pos.rect(p_out, nil))
    assert(    pico.vs.pos.rect(nil, p_in,  nil, nil))
    assert(not pico.vs.pos.rect(nil, p_out, nil, nil))
end

print "flexible dispatch -- rect_pos"
do
    local p_in  = {'!', x=50,  y=50,  anchor='NW'}
    local p_out = {'!', x=200, y=200, anchor='NW'}
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}  -- [25..75]
    assert(    pico.vs.rect.pos(r, p_in))                 -- (r, p)
    assert(not pico.vs.rect.pos(r, p_out))
    -- r1 default = cur bounds; explicit nils required because
    -- a lone table would fill the r1 slot (Pos and Rect both
    -- look like tables to the dispatcher).
    assert(    pico.vs.rect.pos(nil, nil, p_in))
    assert(not pico.vs.rect.pos(nil, nil, p_out))
    assert(    pico.vs.rect.pos(nil, r, nil, p_in))       -- canonical 4-arg
    assert(not pico.vs.rect.pos(nil, r, nil, p_out))
    assert(    pico.vs.rect.pos('sub_vs', nil, nil, p_in))-- r1 = sub's bounds
    assert(not pico.vs.rect.pos('sub_vs', nil, nil, p_out))
end

print "flexible dispatch -- rect_rect (defaults)"
do
    -- rect_rect() and rect_rect(nil,nil,nil,nil) are always
    -- true: both sides default to cur bounds and overlap
    -- themselves; no negative companion exists.
    assert(pico.vs.rect.rect())
    assert(pico.vs.rect.rect(nil, nil, nil, nil))
    local r_in  = {'!', x=10,  y=10,  w=20, h=20, anchor='NW'}
    local r_out = {'!', x=200, y=200, w=10, h=10, anchor='NW'}
    assert(    pico.vs.rect.rect(r_in))                   -- one rect + cur default
    assert(not pico.vs.rect.rect(r_out))                  -- r outside cur
end

print "argument errors"
do
    assert(not pcall(pico.vs.pos.pos))                    -- missing p1+p2
    assert(not pcall(pico.vs.pos.rect))                   -- missing p1
    assert(not pcall(pico.vs.rect.pos))                   -- missing p2
end

print "% mode out of [0,1] -> outside cur bounds"
do
    local p_over  = {'%', x= 1.5, y= 1.5, anchor='NW'}    -- over 1.0
    local p_under = {'%', x=-0.1, y=-0.1, anchor='NW'}    -- under 0.0
    -- pos_rect with default r2 (cur bounds)
    assert(not pico.vs.pos.rect(p_over))
    assert(not pico.vs.pos.rect(p_under))
    -- rect_rect with default r2 (cur bounds)
    local r_over  = {'%', x= 1.5, y= 1.5, w=0.1, h=0.1, anchor='NW'}
    local r_under = {'%', x=-0.5, y=-0.5, w=0.2, h=0.2, anchor='NW'}
    assert(not pico.vs.rect.rect(r_over))
    assert(not pico.vs.rect.rect(r_under))
    -- pos_pos: in-range vs out-of-range
    local p_in = {'%', x=0.5, y=0.5, anchor='NW'}
    assert(not pico.vs.pos.pos(p_in, p_over))
    assert(not pico.vs.pos.pos(p_in, p_under))
    -- rect_pos with p2 out of range
    local r = {'!', x=25, y=25, w=50, h=50, anchor='NW'}
    assert(not pico.vs.rect.pos(r, p_over))
    assert(not pico.vs.rect.pos(r, p_under))
end

print "deep descendant (grandchild)"
do
    pico.layer.empty('sub_vs', 'sub_sub_vs', true, {'!', w=25, h=25})
    -- sub_sub:10,10 -> sub_vs:20,20 -> world:40,40 (scale 4x)
    local p1 = {'!', x=10, y=10, anchor='NW'}
    local p2 = {'!', x=40, y=40, anchor='NW'}
    assert(    pico.vs.pos.pos('sub_sub_vs', p1, nil, p2))
    local q  = {'!', x=50, y=50, anchor='NW'}
    assert(not pico.vs.pos.pos('sub_sub_vs', p1, nil, q))
end

pico.init(false)
