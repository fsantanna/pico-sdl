-- TODO: TGT=0 (phy)

print "pico.cv.pos [ref]: pct->raw"
do
    print('', 1)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'C', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==50 and raw.y==50)

    print('', 2)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'NW', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==50 and raw.y==50)

    print('', 3)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'SE', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==49 and raw.y==49)
end

print "pico.cv.rect [ref]: pct->raw"
do
    print('', 1)
    local pct = { 'C', x=0.5, y=0.5, w=0.5, h=0.5 }
    local raw = pico.cv.rect(pct)
    assert(raw.x==25 and raw.y==25 and raw.w==50 and raw.h==50)

    print('', 2)
    local up  = {x=20, y=20, w=60, h=60}
    local pct = { 'C', x=0.25, y=0.25, w=0.5, h=0.25 }
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==20 and raw.y==28 and raw.w==30 and raw.h==15)

    print('', 3)
    local up  = {x=20, y=20, w=60, h=60}
    local pct = { 'NE', x=0.5, y=0.5, w=0.5, h=0.5 }
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==20 and raw.y==50 and raw.w==30 and raw.h==30)

    print('', 4)
    local up  = {x=20, y=20, w=60, h=60}
    local pct = { 'SW', x=0.5, y=0.5, w=0.5, h=0.5 }
    local raw = pico.cv.rect(pct, up)
    assert(raw.x==50 and raw.y==20 and raw.w==30 and raw.h==30)
end

pico.init(true)
local log = {w=100, h=100}
pico.set.view { world=log }

print "pico.cv.pos [log]: pct->raw"
do
    print('', 1)
    local pct = { 'C', x=0.4, y=0.7 }
    local raw = pico.cv.pos(pct)
    assert(raw.x==40 and raw.y==70)

    print('', 2)
    local pct = { 'NW', x=0.55, y=0.45 }
    local raw = pico.cv.pos(pct)
    assert(raw.x==55 and raw.y==45)

    print('', 3)
    local pct = { 'SE', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct)
    assert(raw.x==49 and raw.y==49)

    print('', 4)
    local p1 = { 'NW', x=0.0, y=0.0 }
    local p2 = { 'NW', x=1.0, y=1.0 }
    local r1 = pico.cv.pos(p1)
    local r2 = pico.cv.pos(p2)
    assert(r1.x==0   and r1.y==0)
    assert(r2.x==100 and r2.y==100)

    print('', 5)
    local p1 = { 'C', x=0.0, y=0.0 }
    local p2 = { 'C', x=1.0, y=1.0 }
    local r1 = pico.cv.pos(p1)
    local r2 = pico.cv.pos(p2)
    assert(r1.x==-1  and r1.y==-1)
    assert(r2.x==100 and r2.y==100)
end

print "pico.cv.rect [log]: pct->raw"
do
    print('', 1)
    local pct = { 'C', x=0.5, y=0.5, w=0.4, h=0.4 }
    local raw = pico.cv.rect(pct)
    assert(raw.x==30 and raw.y==30 and raw.w==40 and raw.h==40)

    print('', 2)
    local pct = { 'NW', x=0.5, y=0.5, w=0.3, h=0.3 }
    local raw = pico.cv.rect(pct)
    assert(raw.x==50 and raw.y==50 and raw.w==30 and raw.h==30)
end

pico.init(false)
