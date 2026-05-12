pico.init(true)
pico.set.scene { dim={'!', w=100, h=100} }

print "pico.cv.pos.to / pos.from: world <-> window"
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
