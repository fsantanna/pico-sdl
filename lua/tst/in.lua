pico.init(true)
pico.set.scene { dim = {'!', w=100, h=100} }

print "pico.xin.rect smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
    local ret = pico.xin.rect(out, child)
    assert(ret[1] == '%')
    assert(ret.anchor.x == 0.5 and ret.anchor.y == 0.5)
    local abs = {'!', anchor='NW'}
    pico.cv.rect.to(nil, ret, abs)
    assert(math.floor(abs.x+0.5)==35 and math.floor(abs.y+0.5)==35)
    assert(math.floor(abs.w+0.5)==30 and math.floor(abs.h+0.5)==30)
end

print "pico.xin.pos smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, anchor='NW'}
    local ret = pico.xin.pos(out, child)
    assert(ret[1] == '%')
    local abs = {'!', anchor='NW'}
    pico.cv.pos.to(nil, ret, abs)
    assert(math.floor(abs.x+0.5)==50 and math.floor(abs.y+0.5)==50)
end

print "pico.xin.dim smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', w=0.5, h=0.5}
    local ret = pico.xin.dim(out, child)
    assert(ret[1] == '%')
    local abs = {'!'}
    pico.cv.dim.to(nil, ret, abs)
    assert(math.floor(abs.w+0.5)==30 and math.floor(abs.h+0.5)==30)
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
