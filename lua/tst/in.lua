pico.init(true)
pico.set.scene { dim = {'!', w=100, h=100} }

print "pico.xin.rect smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
    local ret = pico.xin.rect(out, child)
    assert(ret[1] == '%')
    assert(ret.anchor.x == 0.5 and ret.anchor.y == 0.5)
    local abs = pico.cv.rect(nil, '!', nil, ret)
    assert(math.floor(abs.x+0.5)==50 and math.floor(abs.y+0.5)==50)
    assert(math.floor(abs.w+0.5)==30 and math.floor(abs.h+0.5)==30)
end

print "pico.xin.pos smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60}
    local child = {'%', x=0.5, y=0.5}
    local ret = pico.xin.pos(out, child)
    assert(ret[1] == '%')
    local abs = pico.cv.pos(nil, '!', nil, ret)
    assert(abs.x==20.5 and abs.y==20.5)
end

print "pico.xin.dim smoke"
do
    local out   = {'!', x=20, y=20, w=60, h=60, anchor='NW'}
    local child = {'%', w=0.5, h=0.5}
    local ret = pico.xin.dim(out, child)
    assert(ret[1] == '%')
    local abs = pico.cv.dim(nil, '!', nil, ret)
    assert(math.floor(abs.w+0.5)==30 and math.floor(abs.h+0.5)==30)
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
