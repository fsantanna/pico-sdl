pico.init(true)
pico.set.scene { dim={'!', w=100, h=100} }

-- Helpers: resolve any rel to integer pixels in cur via _to(nil, ...).
local function abs_pos (rel)
    local out = {'!', anchor='NW'}
    pico.cv.pos.to(nil, rel, out)
    return out
end
local function abs_rect (rel)
    local out = {'!', anchor='NW'}
    pico.cv.rect.to(nil, rel, out)
    return out
end
local function abs_dim (rel)
    local out = {'!'}
    pico.cv.dim.to(nil, rel, out)
    return out
end

print "pico.cv.pos.to(nil): resolution"
do
    print('', 1)
    local r = abs_pos {'%', x=0.5, y=0.5, anchor='C'}
    assert(r.x==50 and r.y==50)

    print('', 2)
    local r = abs_pos {'%', x=0.5, y=0.5, anchor='NW'}
    assert(r.x==50 and r.y==50)

    print('', 3)
    local r = abs_pos {'%', x=0.5, y=0.5, anchor='SE'}
    assert(r.x==49 and r.y==49)

    print('', 4)
    local r = abs_pos {'!', x=10, y=15, anchor='NW'}
    assert(r.x==10 and r.y==15)
end

print "pico.cv.rect.to(nil): resolution"
do
    print('', 1)
    local r = abs_rect {'%', x=0.5, y=0.5, w=0.4, h=0.4, anchor='C'}
    assert(r.x==30 and r.y==30 and r.w==40 and r.h==40)

    print('', 2)
    local r = abs_rect {'%', x=0.5, y=0.5, w=0.3, h=0.3, anchor='NW'}
    assert(r.x==50 and r.y==50 and r.w==30 and r.h==30)
end

print "pico.cv.dim.to(nil): resolution"
do
    print('', 1)
    local r = abs_dim {'!', w=50, h=30}
    assert(r.w==50 and r.h==30)

    print('', 2)
    local r = abs_dim {'%', w=0.5, h=0.75}
    assert(r.w==50 and r.h==75)
end

print "pico.cv.pos.to: round-trip (mode/anchor sweep)"
do
    local anchors = {'NW','N','NE','W','C','E','SW','S','SE'}
    local modes = {'!', '%'}
    local starts = {
        {0,0}, {50,50}, {25,75}, {100,100}, {10,90},
    }
    local total = 0
    for _, s in ipairs(starts) do
        local orig = {'!', x=s[1], y=s[2], anchor='NW'}
        for _, m in ipairs(modes) do
            for _, a in ipairs(anchors) do
                local mid = {m, anchor=a}
                pico.cv.pos.to(nil, orig, mid)
                local back = abs_pos(mid)
                assert(math.abs(back.x - s[1]) <= 1,
                       string.format("pos x: %d vs %d (mode=%s anchor=%s)",
                                     back.x, s[1], m, a))
                assert(math.abs(back.y - s[2]) <= 1)
                total = total + 1
            end
        end
    end
    print('', 'passed:', total, 'combos')
end

print "pico.cv.rect.to: round-trip (mode/anchor sweep)"
do
    local anchors = {'NW','N','NE','W','C','E','SW','S','SE'}
    local modes = {'!', '%'}
    local starts = {
        {0,0,10,10}, {25,25,50,50}, {10,20,30,40}, {0,0,100,100},
    }
    local total = 0
    for _, s in ipairs(starts) do
        local orig = {'!', x=s[1], y=s[2], w=s[3], h=s[4], anchor='NW'}
        for _, m in ipairs(modes) do
            for _, a in ipairs(anchors) do
                local mid = {m, anchor=a}
                pico.cv.rect.to(nil, orig, mid)
                local back = abs_rect(mid)
                assert(math.abs(back.x - s[1]) <= 1)
                assert(math.abs(back.y - s[2]) <= 1)
                assert(math.abs(back.w - s[3]) <= 1)
                assert(math.abs(back.h - s[4]) <= 1)
                total = total + 1
            end
        end
    end
    print('', 'passed:', total, 'combos')
end

print "pico.cv.pos.to / pos_from: world <-> window"
do
    -- world 100x100, window 500x500 -> 5x scale
    print('', 1)
    local out = {'!', anchor='NW'}
    pico.cv.pos.to('window', {'!', x=50, y=50, anchor='NW'}, out)
    assert(out.x==250 and out.y==250)

    print('', 2)
    local out = {'!', anchor='NW'}
    pico.cv.pos.from('window', {'!', x=250, y=250, anchor='NW'}, out)
    assert(out.x==50 and out.y==50)

    print('', 3)
    local win = {'!', anchor='NW'}
    pico.cv.pos.to('window', {'!', x=37, y=89, anchor='NW'}, win)
    local back = {'!', anchor='NW'}
    pico.cv.pos.from('window', win, back)
    assert(back.x==37 and back.y==89)
end

print "pico.cv.* sub-layer 2-hop walk"
do
    pico.layer.empty('world', 'sub_cv', {w=50, h=50})
    pico.set.layer 'sub_cv'

    print('', 'pos sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.pos.to('world', {'!', x=10, y=20, anchor='NW'}, w)
    assert(w.x==20 and w.y==40)

    print('', 'pos world -> sub')
    local s = {'!', anchor='NW'}
    pico.cv.pos.from('world', {'!', x=20, y=40, anchor='NW'}, s)
    assert(s.x==10 and s.y==20)

    print('', 'pos sub -> window (2 hops)')
    local win = {'!', anchor='NW'}
    pico.cv.pos.to('window', {'!', x=10, y=20, anchor='NW'}, win)
    assert(win.x==100 and win.y==200)

    print('', 'rect sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.rect.to('world', {'!', x=10, y=20, w=5, h=10, anchor='NW'}, w)
    assert(w.x==20 and w.y==40 and w.w==10 and w.h==20)

    print('', 'dim sub -> world')
    local w = {'!'}
    pico.cv.dim.to('world', {'!', w=5, h=10}, w)
    assert(w.w==10 and w.h==20)

    pico.set.layer 'world'
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
