pico.init(true)
pico.set.scene { dim={'!', w=100, h=100} }

print "pico.cv.pos: world <-> window"
do
    -- world 100x100, window 500x500 -> 5x scale
    print('', 1)
    local out = {'!', anchor='NW'}
    pico.cv.pos('window', out, nil, {'!', x=50, y=50, anchor='NW'})
    assert(out.x==250 and out.y==250)
    pico.cv.pos('window', out, {'!', x=50, y=50, anchor='NW'})
    assert(out.x==250 and out.y==250)

    print('', 2)
    local out = {'!', anchor='NW'}
    pico.cv.pos(nil, out, 'window', {'!', x=250, y=250, anchor='NW'})
    assert(out.x==50 and out.y==50)
    pico.cv.pos(out, 'window', {'!', x=250, y=250, anchor='NW'})
    assert(out.x==50 and out.y==50)

    print('', 3)
    local win = {'!', anchor='NW'}
    pico.cv.pos('window', win, {'!', x=37, y=89, anchor='NW'})
    local back = {'!', anchor='NW'}
    pico.cv.pos(nil, back, 'window', win)
    assert(back.x==37 and back.y==89)
end

do
    local v = pico.cv.pos('%', {'!',x=50,y=99})
    assert(v.x*100//1==50 and v.y*100//98)

    local v = pico.cv.rect(
        {'!',anchor='NW'},
        {'%',x=1,y=1,w=0.5,h=0.5,anchor='SE'}
    )
    assert(v.x==50 and v.y==50)
end

print "pico.cv.* sub-layer 2-hop walk"
do
    pico.layer.empty('world', 'sub_cv', true, {'!', w=50, h=50})
    pico.set.layer 'sub_cv'

    print('', 'pos sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.pos('world', w, nil, {'!', x=10, y=20, anchor='NW'})
    assert(w.x==20 and w.y==40)
    w = pico.cv.pos('world', w, {'!', x=10, y=20, anchor='NW'})
    assert(w.x==20 and w.y==40)

    print('', 'pos world -> sub')
    local s = {'!', anchor='NW'}
    s = pico.cv.pos(nil, s, 'world', {'!', x=20, y=40, anchor='NW'})
    assert(s.x==10 and s.y==20)
    pico.cv.pos(s, 'world', {'!', x=20, y=40, anchor='NW'})
    assert(s.x==10 and s.y==20)

    print('', 'pos sub -> window (2 hops)')
    local win = {'!', anchor='NW'}
    pico.cv.pos('window', win, {'!', x=10, y=20, anchor='NW'})
    assert(win.x==100 and win.y==200)

    print('', 'rect sub -> world')
    local w = {'!', anchor='NW'}
    pico.cv.rect('world', w, nil, {'!', x=10, y=20, w=5, h=10, anchor='NW'})
    assert(w.x==20 and w.y==40 and w.w==10 and w.h==20)
    w = pico.cv.rect('world', w, {'!', x=10, y=20, w=5, h=10, anchor='NW'})
    assert(w.x==20 and w.y==40 and w.w==10 and w.h==20)

    print('', 'dim sub -> world')
    local v = pico.cv.dim('world', '!', nil, {'!', w=5, h=10})
    assert(v.w==10 and v.h==20)

    pico.set.layer 'world'
end

print "pico.cv.* descendant target/source (bidirectional)"
do
    -- cur=world, target=sub_cv (descendant). sub_cv is 50x50 in world 100x100.
    print('', 'pos world -> sub (target=descendant)')
    local s = {'!', anchor='NW'}
    s = pico.cv.pos('sub_cv', s, nil, {'!', x=20, y=40, anchor='NW'})
    assert(s.x==10 and s.y==20)
    pico.cv.pos('sub_cv', s, {'!', x=20, y=40, anchor='NW'})
    assert(s.x==10 and s.y==20)

    print('', 'pos sub -> world (source=descendant)')
    local w = {'!', anchor='NW'}
    pico.cv.pos(nil, w, 'sub_cv', {'!', x=10, y=20, anchor='NW'})
    assert(w.x==20 and w.y==40)

    print('', 'rect world -> sub (target=descendant)')
    local s = {'!', anchor='NW'}
    pico.cv.rect('sub_cv', s, nil, {'!', x=20, y=40, w=10, h=20, anchor='NW'})
    assert(s.x==10 and s.y==20 and s.w==5 and s.h==10)

    print('', 'dim world -> sub (target=descendant)')
    local v = pico.cv.dim('sub_cv', '!', nil, {'!', w=10, h=20})
    assert(v.w==5 and v.h==10)
end

print "pico.cv.* sibling projection (via cur)"
do
    -- cur=world, sub_a and sub_b both children of world.
    -- Each 50x50 in world 100x100 (scale 2x).
    -- Default scene.dst fills cur, so they overlap entirely.
    -- A point at (5,10) in sub_a maps to (10,20) in world maps to (5,10) in sub_b.
    pico.layer.empty('world', 'sib_a', true, {'!', w=50, h=50})
    pico.layer.empty('world', 'sib_b', true, {'!', w=50, h=50})

    print('', 'pos sib_a -> sib_b')
    local p = pico.cv.pos('sib_b', '!', 'sib_a', {'!', x=5, y=10})
    assert(p.x==5 and p.y==10)
end

print "pico.cv.* deep 4-layer (window > world > A > B), default anchor C"
do
    -- All anchors default to C. Constructor rect '%' 0.5,0.5,0.5,0.5 places
    -- each layer centered in its parent at 50%x50%:
    --   world: 100x100 (default), fills window -> 5x scale.
    --   A: dim 50x50, dst center=(50,50) -> NW (25,25)..SE (75,75) in world.
    --   B: dim 25x25, dst center=(25,25) -> NW (12.5,12.5)..SE (37.5,37.5) in A.
    -- Input pos '!' x=10,y=20 with anchor C means center-of-cell -> -0.5 shift
    -- absolute. Output '!' anchor C echoes the +0.5 back. Net: pos B(10,20)
    -- abs is 9.5,19.5 in B; through A (+12.5) -> 22,32 in A abs; through world
    -- (+25) -> 47,57; through window (5x) -> 235,285. Reported as 235.5,285.5
    -- after the +0.5 C echo.
    pico.layer.empty('world', 'A', true,
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.layer.empty('A', 'B', true,
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.layer 'B'

    print('', 'pos B -> A')
    local p = pico.cv.pos('A', '!', nil, {'!', x=10, y=20})
    assert(p.x==22.5 and p.y==32.5)

    print('', 'pos B -> world (2 hops)')
    local p = pico.cv.pos('world', '!', nil, {'!', x=10, y=20})
    assert(p.x==47.5 and p.y==57.5)

    print('', 'pos B -> window (3 hops)')
    local p = pico.cv.pos('window', '!', nil, {'!', x=10, y=20})
    assert(p.x==235.5 and p.y==285.5)

    print('', 'pos window -> B (3 hops back)')
    local p = pico.cv.pos(nil, '!', 'window', {'!', x=235.5, y=285.5})
    assert(p.x==10 and p.y==20)

    print('', 'rect B -> A')
    local r = pico.cv.rect('A', '!', nil, {'!', x=10, y=10, w=5, h=10})
    assert(r.x==22.5 and r.y==22.5 and r.w==5 and r.h==10)

    print('', 'rect B -> world')
    local r = pico.cv.rect('world', '!', nil, {'!', x=10, y=10, w=5, h=10})
    assert(r.x==47.5 and r.y==47.5 and r.w==5 and r.h==10)

    print('', 'rect B -> window')
    local r = pico.cv.rect('window', '!', nil, {'!', x=10, y=10, w=5, h=10})
    assert(r.x==237.5 and r.y==237.5 and r.w==25 and r.h==50)

    print('', 'dim B -> A')
    local d = pico.cv.dim('A', '!', nil, {'!', w=10, h=20})
    assert(d.w==10 and d.h==20)

    print('', 'dim B -> world')
    local d = pico.cv.dim('world', '!', nil, {'!', w=10, h=20})
    assert(d.w==10 and d.h==20)

    print('', 'dim B -> window')
    local d = pico.cv.dim('window', '!', nil, {'!', w=10, h=20})
    assert(d.w==50 and d.h==100)

    pico.set.layer 'world'
end

print "pico.cv.* mode-string as to (returns fresh table)"
do
    print('', "pos with '!'")
    local out = pico.cv.pos('window', '!', nil, {'!', x=50, y=50})
    assert(out and out.x==248 and out.y==248 and out[1]=='!')

    print('', "rect with '%'")
    local out = pico.cv.rect('%', 'window', {'!', x=250, y=250, w=50, h=50, anchor='NW'})
    assert(out and out[1]=='%')

    print('', "dim with '#'")
    local out = pico.cv.dim('window', '#', {'!', w=10, h=20})
    assert(out and out[1]=='#')

    print('', "invalid mode-string 'z' errors")
    assert(not pcall(pico.cv.pos, 'window', 'z', nil, {'!', x=0, y=0, anchor='NW'}))
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
