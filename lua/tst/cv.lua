-- TODO: TGT=0 (phy)

pico.init(true)

print "pico.cv.pos [up]: pct->raw (rel_abs)"
do
    print('', 1)
    local base = {x=25, y=25, w=50, h=50}
    local pct  = {'%', x=0.5, y=0.5, anc='C'}
    local raw  = pico.cv.pos(pct, nil, base)
    assert(raw.x==50 and raw.y==50)

    print('', 2)
    local base = {x=25, y=25, w=50, h=50}
    local pct  = {'%', x=0.5, y=0.5, anc='NW'}
    local raw  = pico.cv.pos(pct, nil, base)
    assert(raw.x==50 and raw.y==50)

    print('', 3)
    local base = {x=25, y=25, w=50, h=50}
    local pct  = {'%', x=0.5, y=0.5, anc='SE'}
    local raw  = pico.cv.pos(pct, nil, base)
    assert(raw.x==49 and raw.y==49)
end

print "pico.cv.rect [up]: pct->raw (rel_abs)"
do
    print('', 1)
    local pct = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C'}
    local raw = pico.cv.rect(pct)
    assert(raw.x==25 and raw.y==25 and raw.w==50 and raw.h==50)

    print('', 2)
    local base = {x=20, y=20, w=60, h=60}
    local pct  = {'%', x=0.25, y=0.25, w=0.5, h=0.25, anc='C'}
    local raw  = pico.cv.rect(pct, nil, base)
    assert(raw.x==20 and raw.y==28 and raw.w==30 and raw.h==15)

    print('', 3)
    local base = {x=20, y=20, w=60, h=60}
    local pct  = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='NE'}
    local raw  = pico.cv.rect(pct, nil, base)
    assert(raw.x==20 and raw.y==50 and raw.w==30 and raw.h==30)

    print('', 4)
    local base = {x=20, y=20, w=60, h=60}
    local pct  = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='SW'}
    local raw  = pico.cv.rect(pct, nil, base)
    assert(raw.x==50 and raw.y==20 and raw.w==30 and raw.h==30)
end

local log = {'!', w=100, h=100}
pico.set.view { dim=log }

print "pico.cv.pos [log]: pct->raw (rel_abs)"
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

print "pico.cv.rect [log]: pct->raw (rel_abs)"
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

print "pico.cv.pos [log]: abs->pct (abs_rel)"
do
    print('', 1)
    local abs = {x=50, y=75}
    local to = {'%', anc='NW'}
    pico.cv.pos(abs, to)
    assert(to.x == 0.5 and to.y == 0.75)

    print('', 2)
    local abs = {x=0, y=0}
    local to = {'%', anc='NW'}
    pico.cv.pos(abs, to)
    assert(to.x == 0.0 and to.y == 0.0)

    print('', 3)
    local abs = {x=100, y=100}
    local to = {'%', anc='NW'}
    pico.cv.pos(abs, to)
    assert(to.x == 1.0 and to.y == 1.0)

    -- round-trip: abs -> pct -> abs
    print('', 4)
    local abs = {x=33, y=67}
    local to = {'%', anc='C'}
    pico.cv.pos(abs, to)
    local back = pico.cv.pos(to)
    assert(back.x == 33 and back.y == 67)
end

print "pico.cv.rect [log]: abs->pct (abs_rel)"
do
    print('', 1)
    local abs = {x=50, y=50, w=25, h=50}
    local to = {'%', anc='NW'}
    pico.cv.rect(abs, to)
    assert(to.x == 0.5 and to.y == 0.5)
    assert(to.w == 0.25 and to.h == 0.5)

    -- round-trip: abs -> pct -> abs
    print('', 2)
    local abs = {x=20, y=30, w=40, h=50}
    local to = {'%', anc='C'}
    pico.cv.rect(abs, to)
    local back = pico.cv.rect(to)
    assert(back.x == 20 and back.y == 30)
    assert(back.w == 40 and back.h == 50)
end

print "pico.cv.pos [log]: rel->rel (rel_rel, canonical form)"
do
    -- pct C -> pxl NW
    print('', 1)
    local fr = {'%', x=0.4, y=0.7, anc='C'}
    local to = {'!', anc='NW'}
    local abs_fr = pico.cv.pos(fr)
    pico.cv.pos(fr, to)
    local abs_to = pico.cv.pos(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)

    -- pct SE -> pxl C
    print('', 2)
    local fr = {'%', x=0.3, y=0.8, anc='SE'}
    local to = {'!', anc='C'}
    local abs_fr = pico.cv.pos(fr)
    pico.cv.pos(fr, to)
    local abs_to = pico.cv.pos(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)

    -- pct NW -> pct SE (same mode, different anchor)
    print('', 3)
    local fr = {'%', x=0.5, y=0.5, anc='NW'}
    local to = {'%', anc='SE'}
    local abs_fr = pico.cv.pos(fr)
    pico.cv.pos(fr, to)
    local abs_to = pico.cv.pos(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)
end

print "pico.cv.rect [log]: rel->rel (rel_rel, canonical form)"
do
    -- pct C -> pxl C
    print('', 1)
    local fr = {'%', x=0.5, y=0.5, w=0.4, h=0.4, anc='C'}
    local to = {'!', anc='C'}
    local abs_fr = pico.cv.rect(fr)
    pico.cv.rect(fr, to)
    local abs_to = pico.cv.rect(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)
    assert(abs_fr.w == abs_to.w and abs_fr.h == abs_to.h)

    -- pct NW -> pxl SE
    print('', 2)
    local fr = {'%', x=0.3, y=0.2, w=0.5, h=0.6, anc='NW'}
    local to = {'!', anc='SE'}
    local abs_fr = pico.cv.rect(fr)
    pico.cv.rect(fr, to)
    local abs_to = pico.cv.rect(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)
    assert(abs_fr.w == abs_to.w and abs_fr.h == abs_to.h)

    -- pct NW -> pct SE
    print('', 3)
    local fr = {'%', x=0.4, y=0.6, w=0.3, h=0.2, anc='NW'}
    local to = {'%', anc='SE'}
    local abs_fr = pico.cv.rect(fr)
    pico.cv.rect(fr, to)
    local abs_to = pico.cv.rect(to)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)
    assert(abs_fr.w == abs_to.w and abs_fr.h == abs_to.h)
end

print "pico.cv.pos [base]: abs->pct (abs_rel with base)"
do
    print('', 1)
    local base = {x=25, y=25, w=50, h=50}
    local abs  = {x=50, y=50}
    local to   = {'%', anc='NW'}
    pico.cv.pos(abs, to, base)
    assert(to.x == 0.5 and to.y == 0.5)

    -- round-trip with base
    print('', 2)
    local base = {x=25, y=25, w=50, h=50}
    local fr   = {'%', x=0.5, y=0.5, anc='C'}
    local abs  = pico.cv.pos(fr, nil, base)
    local to   = {'%', anc='C'}
    pico.cv.pos(abs, to, base)
    local back = pico.cv.pos(to, nil, base)
    assert(back.x == abs.x and back.y == abs.y)
end

print "pico.cv.rect [base]: rel->rel (rel_rel with base)"
do
    print('', 1)
    local base = {x=20, y=20, w=60, h=60}
    local fr   = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anc='C'}
    local to   = {'!', anc='NW'}
    local abs_fr = pico.cv.rect(fr, nil, base)
    pico.cv.rect(fr, to, base)
    local abs_to = pico.cv.rect(to, nil, base)
    assert(abs_fr.x == abs_to.x and abs_fr.y == abs_to.y)
    assert(abs_fr.w == abs_to.w and abs_fr.h == abs_to.h)
end

pico.init(false)
