pico.init(true)
pico.set.scene { dim={'!', w=100, h=100} }

print "pico.cv.pos.to / pos.from: world <-> window"
do
    -- world 100x100, window 500x500 -> 5x scale
    print('', 1)
    local out = {'!', anchor='NW'}
    pico.cv.pos.to(out, {'!', x=50, y=50, anchor='NW'}, 'window')
    assert(out.x==250 and out.y==250)

    print('', 2)
    local out = {'!', anchor='NW'}
    pico.cv.pos.from(out, {'!', x=250, y=250, anchor='NW'}, 'window')
    assert(out.x==50 and out.y==50)

    print('', 3)
    local win = {'!', anchor='NW'}
    pico.cv.pos.to(win, {'!', x=37, y=89, anchor='NW'}, 'window')
    local back = {'!', anchor='NW'}
    pico.cv.pos.from(back, win, 'window')
    assert(back.x==37 and back.y==89)
end

print "pico.cv.* sub-layer 2-hop walk"
do
    pico.layer.empty('world', 'sub_cv', true, {'!', w=50, h=50})
    pico.set.layer 'sub_cv'

    print('', 'pos sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.pos.to(w, {'!', x=10, y=20, anchor='NW'}, 'world')
    assert(w.x==20 and w.y==40)

    print('', 'pos world -> sub')
    local s = {'!', anchor='NW'}
    pico.cv.pos.from(s, {'!', x=20, y=40, anchor='NW'}, 'world')
    assert(s.x==10 and s.y==20)

    print('', 'pos sub -> window (2 hops)')
    local win = {'!', anchor='NW'}
    pico.cv.pos.to(win, {'!', x=10, y=20, anchor='NW'}, 'window')
    assert(win.x==100 and win.y==200)

    print('', 'rect sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.rect.to(w, {'!', x=10, y=20, w=5, h=10, anchor='NW'}, 'world')
    assert(w.x==20 and w.y==40 and w.w==10 and w.h==20)

    print('', 'dim sub -> world')
    local v = pico.cv.dim.to('!', {'!', w=5, h=10}, 'world')
    assert(v.w==10 and v.h==20)

    pico.set.layer 'world'
end

print "pico.cv.* descendant target/source (bidirectional)"
do
    -- cur=world, target=sub_cv (descendant). sub_cv is 50x50 in world 100x100.
    print('', 'pos world -> sub (target=descendant)')
    local s = {'!', anchor='NW'}
    pico.cv.pos.to(s, {'!', x=20, y=40, anchor='NW'}, 'sub_cv')
    assert(s.x==10 and s.y==20)

    print('', 'pos sub -> world (source=descendant)')
    local w = {'!', anchor='NW'}
    pico.cv.pos.from(w, {'!', x=10, y=20, anchor='NW'}, 'sub_cv')
    assert(w.x==20 and w.y==40)

    print('', 'rect world -> sub (target=descendant)')
    local s = {'!', anchor='NW'}
    pico.cv.rect.to(s, {'!', x=20, y=40, w=10, h=20, anchor='NW'}, 'sub_cv')
    assert(s.x==10 and s.y==20 and s.w==5 and s.h==10)

    print('', 'dim world -> sub (target=descendant)')
    local v = pico.cv.dim.to('!', {'!', w=10, h=20}, 'sub_cv')
    assert(v.w==5 and v.h==10)
end

print "pico.cv.* mode-string as to (returns fresh table)"
do
    -- mode-string in the to slot returns a fresh table; no layer = cur.
    print('', "pos.to with '!'")
    local out = pico.cv.pos.to('!', {'!', x=50, y=50, anchor='NW'}, 'window')
    assert(out and out.x==250 and out.y==250 and out[1]=='!')

    print('', "rect.from with '%'")
    local out = pico.cv.rect.from('%', {'!', x=250, y=250, w=50, h=50, anchor='NW'}, 'window')
    assert(out and out[1]=='%')

    print('', "dim.to with '#'")
    local out = pico.cv.dim.to('#', {'!', w=10, h=20}, 'window')
    assert(out and out[1]=='#')

    print('', "invalid mode-string 'z' errors")
    assert(not pcall(pico.cv.pos.to, 'z', {'!', x=0, y=0, anchor='NW'}, 'window'))
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
