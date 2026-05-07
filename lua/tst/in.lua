pico.init(true)
pico.set.scene { dim = {'!', w=100, h=100} }

print "pico.xin.rect - ! NW"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'!', x=10, y=10, w=20, h=20, anchor='NW'}
    local ret = pico.xin.rect(out, child)
    local abs = pico.cv.rect(ret)
    assert(abs.x==30 and abs.y==30 and abs.w==20 and abs.h==20)
    assert(ret[1] == '!')
end

print "pico.xin.rect - % C in ! NW"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
    local ret = pico.xin.rect(out, child)
    local abs = pico.cv.rect(ret)
    assert(abs.x==35 and abs.y==35 and abs.w==30 and abs.h==30)
    assert(ret[1] == '%')
end

print "pico.xin.pos - ! NW"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'!', x=10, y=10, anchor='NW'}
    local ret = pico.xin.pos(out, child)
    local abs = pico.cv.pos(ret)
    assert(abs.x==30 and abs.y==30)
end

print "pico.xin.pos - % NW"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, anchor='NW'}
    local ret = pico.xin.pos(out, child)
    local abs = pico.cv.pos(ret)
    assert(abs.x==50 and abs.y==50)
end

print "pico.xin.dim - !"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'!', w=30, h=20}
    local ret = pico.xin.dim(out, child)
    local abs = pico.cv.dim(ret)
    assert(abs.w==30 and abs.h==20)
end

print "pico.xin.dim - %"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', w=0.5, h=0.5}
    local ret = pico.xin.dim(out, child)
    local abs = pico.cv.dim(ret)
    assert(abs.w==30 and abs.h==30)
end

print "mode and anchor preserved"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
    local ret = pico.xin.rect(out, child)
    assert(ret[1] == '%')
    assert(ret.anchor.x == 0.5 and ret.anchor.y == 0.5)
end

print "equivalence: xin matches cv with out_abs as base"
do
    local out   = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
    local child = {'!', x=5, y=5, w=10, h=10, anchor='NW'}
    local out_abs = pico.cv.rect(out)
    local old_way = pico.cv.rect(child, nil, out_abs)
    local ret = pico.xin.rect(out, child)
    local new_way = pico.cv.rect(ret)
    assert(old_way.x == new_way.x and old_way.y == new_way.y
       and old_way.w == new_way.w and old_way.h == new_way.h)
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
