pico.init(true)

local phy = {'!', w=200, h=200}
local log = {'!', w=100, h=100}
pico.set.view {
    window = phy,
    world  = log,
}

print "pico.vs.pos_rect (raw)"
do
    print('', 1)
    local r = {'!', x=25, y=25, w=50, h=50, anc='NW'}
    local p = {'!', x=50, y=50, anc='NW'}
    local ret = pico.vs.pos_rect(p, r)
    assert(ret)

    print('', 2)
    local r = {'!', x=25, y=25, w=50, h=50, anc='NW'}
    local p = {'!', x=10, y=10, anc='NW'}
    local ret = pico.vs.pos_rect(p, r)
    assert(not ret)
end

print "pico.vs.pos_rect (pct)"
do
    print('', 1)
    local r = {'%', x=0.5, y=0.5, w=0.5, h=0.5}
    local p = {'%', x=0.5, y=0.5}
    local ret = pico.vs.pos_rect(p, r)
    assert(ret)

    print('', 2)
    local r = {'%', x=0.5, y=0.5, w=0.3, h=0.3}
    local p = {'%', x=0.1, y=0.1}
    local ret = pico.vs.pos_rect(p, r)
    assert(not ret)
end

print "pico.vs.rect_rect (raw)"
do
    print('', 1)
    local r1 = {'!', x=20, y=20, w=40, h=40, anc='NW'}
    local r2 = {'!', x=30, y=30, w=40, h=40, anc='NW'}
    local ret = pico.vs.rect_rect(r1, r2)
    assert(ret)

    print('', 2)
    local r1 = {'!', x=10, y=10, w=30, h=30, anc='NW'}
    local r2 = {'!', x=50, y=50, w=30, h=30, anc='NW'}
    local ret = pico.vs.rect_rect(r1, r2)
    assert(not ret)
end

print "pico.vs.rect_rect (pct)"
do
    print('', 1)
    local r1 = {'%', x=0.3, y=0.3, w=0.4, h=0.4}
    local r2 = {'%', x=0.5, y=0.5, w=0.4, h=0.4}
    local ret = pico.vs.rect_rect(r1, r2)
    assert(ret)

    print('', 2)
    local r1 = {'%', x=0.2, y=0.2, w=0.2, h=0.2}
    local r2 = {'%', x=0.8, y=0.8, w=0.2, h=0.2}
    local ret = pico.vs.rect_rect(r1, r2)
    assert(not ret)
end

print "mix pct/raw"
do
    print('', 1)
    local r1 = {'%', x=0.3, y=0.3, w=0.4, h=0.4}
    local r2 = {'!', x=40, y=40, w=10, h=10, anc='NW'}
    local ret = pico.vs.rect_rect(r1, r2)
    assert(ret)

    print('', 2)
    local p1 = {'!', x=0, y=0, anc='NW'}
    local r2 = {'%', x=0.8, y=0.8, w=0.2, h=0.2}
    local ret = pico.vs.pos_rect(p1, r2)
    assert(not ret)
end

pico.init(false)
