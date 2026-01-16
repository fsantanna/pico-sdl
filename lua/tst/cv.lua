local pico = require 'pico'

-- TODO: TGT=0 (phy)

print "pico.cv.pos: pct->raw"
do
    print('', 1)
    local pct = { 'C', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct)
    assert(raw.x==50 and raw.y==50)

    print('', 2)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'C', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==50 and raw.y==50)

    print('', 3)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'NW', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==50 and raw.y==50)

    print('', 4)
    local up  = {x=25, y=25, w=50, h=50}
    local pct = { 'SE', x=0.5, y=0.5 }
    local raw = pico.cv.pos(pct, up)
    assert(raw.x==49 and raw.y==49)
end

print "pico.cv.rect: pct->raw"
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
pico.set.view(-1, nil, nil, log, nil, nil)

-- LOG - POS - PCT->RAW
do
    print("log - pos - pct->raw - C")
    local pct = {x=0.4, y=0.7, anchor=pico.ANCHOR_C}
    local raw = pico.cv_pos_pct_raw(pct)
    print(string.format("pos: (%d, %d)", raw.x, raw.y))
    assert(raw.x==40 and raw.y==70)
end
do
    print("log - pos - pct->raw - NW")
    local pct = {x=0.55, y=0.45, anchor=pico.ANCHOR_NW}
    local raw = pico.cv_pos_pct_raw(pct)
    print(string.format("pos: (%d, %d)", raw.x, raw.y))
    assert(raw.x==55 and raw.y==45)
end
do
    print("log - pos - pct->raw - SE")
    local pct = {x=0.5, y=0.5, anchor=pico.ANCHOR_SE}
    local raw = pico.cv_pos_pct_raw(pct)
    print(string.format("pos: (%d, %d)", raw.x, raw.y))
    assert(raw.x==49 and raw.y==49)
end
do
    print("log - pos - pct->raw - corners")
    local p1 = {x=0.0, y=0.0, anchor=pico.ANCHOR_NW}
    local p2 = {x=1.0, y=1.0, anchor=pico.ANCHOR_NW}
    local r1 = pico.cv_pos_pct_raw(p1)
    local r2 = pico.cv_pos_pct_raw(p2)
    assert(r1.x==0   and r1.y==0)
    assert(r2.x==100 and r2.y==100)
end
do
    print("log - pos - pct->raw - corners")
    local p1 = {x=0.0, y=0.0, anchor=pico.ANCHOR_C}
    local p2 = {x=1.0, y=1.0, anchor=pico.ANCHOR_C}
    local r1 = pico.cv_pos_pct_raw(p1)
    local r2 = pico.cv_pos_pct_raw(p2)
    assert(r1.x==-1  and r1.y==-1)
    assert(r2.x==100 and r2.y==100)
end

-- LOG - RECT - PCT->RAW
do
    print("log - rect - pct->raw - C")
    local pct = {x=0.5, y=0.5, w=0.4, h=0.4, anchor=pico.ANCHOR_C}
    local raw = pico.cv_rect_pct_raw(pct)
    print(string.format("rect: (%d, %d, %d, %d)", raw.x, raw.y, raw.w, raw.h))
    assert(raw.x==30 and raw.y==30 and raw.w==40 and raw.h==40)
end
do
    print("log - rect - pct->raw - NW")
    local pct = {x=0.5, y=0.5, w=0.3, h=0.3, anchor=pico.ANCHOR_NW}
    local raw = pico.cv_rect_pct_raw(pct)
    print(string.format("rect: (%d, %d, %d, %d)", raw.x, raw.y, raw.w, raw.h))
    assert(raw.x==50 and raw.y==50 and raw.w==30 and raw.h==30)
end

pico.init(false)
