require 'pico.check'

local function check (out, asr)
    print("Testing: " .. asr)
    local f1 = io.open(out, "rb")
    local f2 = io.open(asr, "rb")
    assert(f1, "cannot open " .. out)
    assert(f2, "cannot open " .. asr)
    local c1 = f1:read("*a")
    local c2 = f2:read("*a")
    f1:close()
    f2:close()
    assert(c1 == c2, "files differ: " .. out .. " vs " .. asr)
end

pico.init(true)
pico.output.clear()

do
    print("entire screen - 01")
    pico.output.draw.rect({x=10, y=10, w=20, h=20})
    local f = pico.output.screenshot()
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-01.png")
    assert(os.remove(f))
end

do
    print("entire screen - 02")
    pico.set.color.draw(200, 0, 0)
    pico.output.draw.rect({x=30, y=30, w=10, h=10})
    local f = pico.output.screenshot("../../tst/out/shot-02.png")
    assert(f == "../../tst/out/shot-02.png")
    check(f, "../../tst/asr/shot-02.png")
end

do
    print("part of screen (raw)")
    pico.set.color.draw(0, 200, 0)
    pico.output.draw.rect({x=40, y=5, w=10, h=10})
    local f = pico.output.screenshot(nil, {x=0, y=0, w=250, h=150})
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-03.png")
    assert(os.remove(f))
end

do
    print("part of screen (pct)")
    pico.set.color.draw(0, 0, 200)
    pico.output.draw.rect({x=50, y=50, w=10, h=10})
    local f = pico.output.screenshot(nil, {'NW', x=0, y=0, w=0.5, h=0.3})
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-04.png")
    assert(os.remove(f))
end

pico.init(false)
