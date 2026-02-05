-- TODO: TGT=0 (phy)

pico.init(true)

print "pico.cv.pos [up]: pct->raw"
do
    print('', 1)
    local up  = {'!', x=25, y=25, w=50, h=50}
    local pct = {'%', x=0.5, y=0.5, anc='C'}
    local raw = pico.cv.pos(pct, up)
    print(raw.x, raw)
    assert(raw.x==50 and raw.y==50)

    print('', 2)
    local up  = {'!', x=25, y=25, w=50, h=50}
    local pct = {'%', x=0.5, y=0.5, anc='NW'}
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==50 and raw.y==50)

    print('', 3)
    local up  = {'!', x=25, y=25, w=50, h=50}
    local pct = {'%', x=0.5, y=0.5, anc='SE'}
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==49 and raw.y==49)
end

print "pico.cv.rect [up]: pct->raw"
do
    print('', 1)
    local pct = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C'}
    local raw = pico.cv.rect(pct)
    assert(raw.x==25 and raw.y==25 and raw.w==50 and raw.h==50)

    print('', 2)
    local up  = {'!', x=20, y=20, w=60, h=60}
    local pct = {'%', x=0.25, y=0.25, w=0.5, h=0.25, anc='C'}
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==20 and raw.y==28 and raw.w==30 and raw.h==15)

    print('', 3)
    local up  = {'!', x=20, y=20, w=60, h=60}
    local pct = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='NE'}
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==20 and raw.y==50 and raw.w==30 and raw.h==30)

    print('', 4)
    local up  = {'!', x=20, y=20, w=60, h=60}
    local pct = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='SW'}
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==50 and raw.y==20 and raw.w==30 and raw.h==30)
end

local log = {'!', w=100, h=100}
pico.set.view { dim=log }

print "pico.cv.pos [log]: pct->raw"
do
    print('', 1)
    local pct = {'%', x=0.4, y=0.7, anc='C'}
    local raw = pico.cv.pos(pct)
    assert(raw.x==40 and raw.y==70)

    print('', 2)
    local pct = {'%', x=0.55, y=0.45, anc='NW'}
    local raw = pico.cv.pos(pct)
    assert(raw.x==55 and raw.y==45)

    print('', 3)
    local pct = {'%', x=0.5, y=0.5, anc='SE'}
    local raw = pico.cv.pos(pct)
    assert(raw.x==49 and raw.y==49)

    print('', 4)
    local p1 = {'%', x=0.0, y=0.0, anc='NW'}
    local p2 = {'%', x=1.0, y=1.0, anc='NW'}
    local r1 = pico.cv.pos(p1)
    local r2 = pico.cv.pos(p2)
    assert(r1.x==0   and r1.y==0)
    assert(r2.x==100 and r2.y==100)

    print('', 5)
    local p1 = {'%', x=0.0, y=0.0, anc='C'}
    local p2 = {'%', x=1.0, y=1.0, anc='C'}
    local r1 = pico.cv.pos(p1)
    local r2 = pico.cv.pos(p2)
    assert(r1.x==-1  and r1.y==-1)
    assert(r2.x==100 and r2.y==100)
end

print "pico.cv.rect [log]: pct->raw"
do
    print('', 1)
    local pct = {'%', x=0.5, y=0.5, w=0.4, h=0.4, anc='C'}
    local raw = pico.cv.rect(pct)
    assert(raw.x==30 and raw.y==30 and raw.w==40 and raw.h==40)

    print('', 2)
    local pct = {'%', x=0.5, y=0.5, w=0.3, h=0.3, anc='NW'}
    local raw = pico.cv.rect(pct)
    assert(raw.x==50 and raw.y==50 and raw.w==30 and raw.h==30)
end

print "pico.cv.dim [log]: raw->abs"
do
    print('', 1)
    local dim = {'!', w=50, h=30}
    local raw = pico.cv.dim(dim)
    assert(raw.w==50 and raw.h==30)
end

print "pico.cv.dim [log]: pct->abs"
do
    print('', 1)
    local dim = {'%', w=0.5, h=0.3}
    local raw = pico.cv.dim(dim)
    assert(raw.w==50 and raw.h==30)

    print('', 2)
    local up  = {'!', x=10, y=10, w=80, h=60}
    local dim = {'%', w=0.5, h=0.5, up=up}
    local raw = pico.cv.dim(dim)
    assert(raw.w==40 and raw.h==30)
end

print "pico.cv.dim [log]: tile->abs"
do
    local log  = {'#', w=4, h=4}
    local tile = {w=4, h=4}
    pico.set.view { dim=log, tile=tile }

    print('', 1)
    local dim = {'#', w=2, h=3}
    local raw = pico.cv.dim(dim)
    assert(raw.w==8 and raw.h==12)

    print('', 2)
    local dim = {'#', w=1, h=1}
    local raw = pico.cv.dim(dim)
    assert(raw.w==4 and raw.h==4)

    local log = {'!', w=100, h=100}
    pico.set.view { dim=log }
end

pico.init(false)
