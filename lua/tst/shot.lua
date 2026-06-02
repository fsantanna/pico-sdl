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
    pico.output.draw.rect({'!', x=10, y=10, w=20, h=20, anchor='NW'})
    pico.set.layer("window")
    local f = pico.output.screenshot(nil)
    pico.set.layer("world")
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-01.png")
    assert(os.remove(f))
end

do
    print("entire screen - 02")
    pico.set.pencil { color={'!', r=200, g=0, b=0} }
    pico.output.draw.rect({'!', x=30, y=30, w=10, h=10, anchor='NW'})
    pico.set.layer("window")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-02.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-02.png")
    check(f, "../../tst/asr/shot-02.png")
end

do
    print("part of screen (raw)")
    pico.set.pencil { color={'!', r=0, g=200, b=0} }
    pico.output.draw.rect({'!', x=40, y=5, w=10, h=10, anchor='NW'})
    pico.set.layer("window")
    local f = pico.output.screenshot(nil, {'!', x=0, y=0, w=250, h=150, anchor='NW'})
    pico.set.layer("world")
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-03.png")
    assert(os.remove(f))
end

do
    print("part of screen (pct)")
    pico.set.pencil { color={'!', r=0, g=0, b=200} }
    pico.output.draw.rect({'!', x=50, y=50, w=10, h=10, anchor='NW'})
    pico.set.layer("window")
    local f = pico.output.screenshot(nil, {'%', x=0, y=0, w=0.5, h=0.3, anchor='NW'})
    pico.set.layer("world")
    assert(f ~= nil)
    check(f, "../../tst/asr/shot-04.png")
    assert(os.remove(f))
end

do
    print("world layer")
    pico.set.effect { color={'!', r=0x10, g=0x10, b=0x10} }
    pico.output.clear()
    pico.set.pencil { color={'!', r=0xFF, g=0x00, b=0x00} }
    pico.output.draw.rect({'!', x=10, y=10, w=30, h=30, anchor='NW'})
    local f = pico.output.screenshot(nil, "../../tst/out/shot-world.png")
    assert(f == "../../tst/out/shot-world.png")
    check(f, "../../tst/asr/shot-world.png")
end

do
    print("empty layer")
    pico.layer.empty(nil, "empty1", false, {'!', w=64, h=32})
    pico.set.layer("empty1")
    pico.set.effect { color={'!', r=0x00, g=0x80, b=0x00} }
    pico.output.clear()
    pico.set.pencil { color={'!', r=0xFF, g=0xFF, b=0x00} }
    pico.output.draw.rect({'!', x=5, y=5, w=10, h=10, anchor='NW'})
    local f = pico.output.screenshot(nil, "../../tst/out/shot-empty.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-empty.png")
    check(f, "../../tst/asr/shot-empty.png")
end

do
    print("pixmap layer")
    pico.layer.pixmap(nil, "pmap1", {
        {{r=255, g=  0, b=  0, a=255}, {r=  0, g=255, b=  0, a=255}},
        {{r=  0, g=  0, b=255, a=255}, {r=255, g=255, b=  0, a=255}},
    })
    pico.set.layer("pmap1")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-pixmap.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-pixmap.png")
    check(f, "../../tst/asr/shot-pixmap.png")
end

do
    print("sub layer")
    pico.layer.sub(nil, "sub1", "empty1",
        {'!', x=0, y=0, w=32, h=16, anchor='NW'})
    pico.set.layer("sub1")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-sub.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-sub.png")
    check(f, "../../tst/asr/shot-sub.png")
end

do
    print("image layer")
    pico.layer.image(nil, "img1", "../../res/open.png")
    pico.set.layer("img1")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-image.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-image.png")
    check(f, "../../tst/asr/shot-image.png")
end

do
    print("text layer")
    pico.set.pencil { color={'!', r=0xFF, g=0xFF, b=0xFF} }
    pico.layer.text(nil, "txt1", 16, "hello")
    pico.set.layer("txt1")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-text.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-text.png")
    check(f, "../../tst/asr/shot-text.png")
end

do
    print("video layer")
    pico.layer.video(nil, "vid1", "video.y4m")
    pico.set.layer("vid1")
    assert(pico.set.video("vid1", 0) == true)
    local f = pico.output.screenshot(nil, "../../tst/out/shot-video.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-video.png")
    check(f, "../../tst/asr/shot-video.png")
end

-- PICO.LAYER.SCREENSHOT

-- 1. capture an existing layer into a new layer; reuses empty1's reference
do
    print("screenshot layer - reuse empty1")
    pico.layer.screenshot(nil, "snap_empty", "empty1")
    pico.set.layer("snap_empty")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-snap-empty.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-snap-empty.png")
    check(f, "../../tst/asr/shot-empty.png")
end

-- 2. compose a multi-layer scene, capture it, clear, redraw, compare
do
    print("screenshot layer - scene round-trip")

    pico.layer.empty(nil, "red", true, {'!', w=20, h=20})
    pico.set.layer("red")
    pico.set.effect { color={'!', r=0xFF, g=0, b=0} }
    pico.output.clear()

    pico.layer.empty(nil, "blue", true, {'!', w=20, h=20})
    pico.set.layer("blue")
    pico.set.effect { color={'!', r=0, g=0, b=0xFF} }
    pico.output.clear()

    pico.set.layer("world")
    pico.set.effect { color={'!', r=0x20, g=0x20, b=0x20} }
    pico.output.clear()
    pico.output.draw.layer("red", {'!', x=20, y=20, w=20, h=20, anchor='NW'})
    pico.output.draw.layer("blue", {'!', x=50, y=40, w=20, h=20, anchor='NW'})

    local f1 = pico.output.screenshot("world", "../../tst/out/shot-scene.png")
    check(f1, "../../tst/asr/shot-scene.png")

    pico.layer.screenshot(nil, "snap_scene", "world")
    pico.set.layer("world")
    pico.output.clear()
    pico.output.draw.layer("snap_scene")
    local f2 = pico.output.screenshot("world", "../../tst/out/shot-scene-2.png")
    check(f2, "../../tst/asr/shot-scene.png")
    pico.set.layer("world")
end

-- 3. capture a relative region; resolves against the source layer (64x32)
do
    print("screenshot layer - region (pct)")
    pico.layer.screenshot(nil, "snap_half", "empty1",
        {'%', x=0, y=0, w=0.5, h=1.0, anchor='NW'})
    pico.set.layer("snap_half")
    local f = pico.output.screenshot(nil, "../../tst/out/shot-snap-half.png")
    pico.set.layer("world")
    assert(f == "../../tst/out/shot-snap-half.png")
    check(f, "../../tst/asr/shot-snap-half.png")
end

pico.init(false)
