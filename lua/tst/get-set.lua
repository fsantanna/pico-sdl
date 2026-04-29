local pico = require "pico"

require "pico.check"

pico.init(true)
pico.set.window { title="Get-Set", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- color_clear
print("color_clear")
do
    pico.set.show { color='red' }
    local c = pico.get.show().color
    assert(c.r == 255 and c.g == 0 and c.b == 0 and c.a == 255)
    pico.set.show { color='blue' }
    c = pico.get.show().color
    assert(c.r == 0 and c.g == 0 and c.b == 255 and c.a == 255)
    pico.set.show { color='black' }
    c = pico.get.show().color
    assert(c.r == 0 and c.g == 0 and c.b == 0 and c.a == 255)
end

-- color_clear with alpha
print("color_clear_alpha")
do
    pico.set.show { color={'!', r=0, g=0, b=255, a=128} }
    local c = pico.get.show().color
    assert(c.r == 0 and c.g == 0 and c.b == 255 and c.a == 128)
    pico.set.show { color={'!', r=255, g=0, b=0, a=0} }
    c = pico.get.show().color
    assert(c.r == 255 and c.g == 0 and c.b == 0 and c.a == 0)
end

-- color_draw
print("color_draw")
do
    pico.set.draw { color='green' }
    local c = pico.get.draw().color
    assert(c.r == 0 and c.g == 255 and c.b == 0)
    pico.set.draw { color='yellow' }
    c = pico.get.draw().color
    assert(c.r == 255 and c.g == 255 and c.b == 0)
    pico.set.draw { color='white' }
    c = pico.get.draw().color
    assert(c.r == 255 and c.g == 255 and c.b == 255)
end

-- style
print("style")
do
    pico.set.draw { style='fill' }
    assert(pico.get.draw().style == 'fill')
    pico.set.draw { style='stroke' }
    assert(pico.get.draw().style == 'stroke')
    pico.set.draw { style='fill' }
    assert(pico.get.draw().style == 'fill')
end

-- view (get defaults, set, get back)
print("view")
do
    local v = pico.get.view()
    assert(v.dim.w == 64 and v.dim.h == 48)
    assert(v.tile.w == 0 and v.tile.h == 0)
    assert(v.target[1] == '%' and v.target.x == 0.5 and v.target.y == 0.5 and v.target.w == 1 and v.target.h == 1)
    assert(v.source[1] == '%' and v.source.x == 0.5 and v.source.y == 0.5 and v.source.w == 1 and v.source.h == 1)
    assert(v.clip[1] == '%' and v.clip.x == 0.5 and v.clip.y == 0.5 and v.clip.w == 1 and v.clip.h == 1)

    pico.set.view { target={'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'} }
    v = pico.get.view()
    assert(v.target.w == 0.5 and v.target.h == 0.5)

    pico.set.view { source={'%', x=0.6, y=0.4, w=0.8, h=0.8, anchor='C'} }
    v = pico.get.view()
    assert(pico.equal(v.source.x,0.6) and pico.equal(v.source.y,0.4))

    -- restore
    pico.set.view {
        target={'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
        source={'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
    }
end

-- window roundtrip: get -> set -> get must be idempotent
print("window roundtrip")
do
    pico.set.window { color={'!', r=0x12, g=0x34, b=0x56, a=0x78} }
    local w1 = pico.get.window()
    pico.set.window(w1)
    local w2 = pico.get.window()
    assert(w2.dim.w   == w1.dim.w   and w2.dim.h == w1.dim.h)
    assert(w2.color.r == w1.color.r and w2.color.g == w1.color.g)
    assert(w2.color.b == w1.color.b and w2.color.a == w1.color.a)
end

-- view roundtrip: get -> set -> get must be idempotent
print("view roundtrip")
do
    local v1 = pico.get.view()
    pico.set.view(v1)
    local v2 = pico.get.view()
    assert(v2.dim.w == v1.dim.w and v2.dim.h == v1.dim.h)
    assert(v2.target[1] == v1.target[1])
    assert(v2.source[1] == v1.source[1])
    assert(v2.clip[1]   == v1.clip[1])
end

print("OK")
pico.init(false)
