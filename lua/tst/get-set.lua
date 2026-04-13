local pico = require "pico"

pico.init(true)
pico.set.window { title="Get-Set", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- color_clear
print("color_clear")
do
    pico.set.color.clear('red')
    local c = pico.get.color.clear()
    assert(c.r == 255 and c.g == 0 and c.b == 0 and c.a == 255)
    pico.set.color.clear('blue')
    c = pico.get.color.clear()
    assert(c.r == 0 and c.g == 0 and c.b == 255 and c.a == 255)
    pico.set.color.clear('black')
    c = pico.get.color.clear()
    assert(c.r == 0 and c.g == 0 and c.b == 0 and c.a == 255)
end

-- color_clear with alpha
print("color_clear_alpha")
do
    pico.set.color.clear({'!', r=0, g=0, b=255, a=128})
    local c = pico.get.color.clear()
    assert(c.r == 0 and c.g == 0 and c.b == 255 and c.a == 128)
    pico.set.color.clear({'!', r=255, g=0, b=0, a=0})
    c = pico.get.color.clear()
    assert(c.r == 255 and c.g == 0 and c.b == 0 and c.a == 0)
end

-- color_draw
print("color_draw")
do
    pico.set.color.draw('green')
    local c = pico.get.color.draw()
    assert(c.r == 0 and c.g == 255 and c.b == 0)
    pico.set.color.draw('yellow')
    c = pico.get.color.draw()
    assert(c.r == 255 and c.g == 255 and c.b == 0)
    pico.set.color.draw('white')
    c = pico.get.color.draw()
    assert(c.r == 255 and c.g == 255 and c.b == 255)
end

-- style
print("style")
do
    pico.set.style('fill')
    assert(pico.get.style() == 'fill')
    pico.set.style('stroke')
    assert(pico.get.style() == 'stroke')
    pico.set.style('fill')
    assert(pico.get.style() == 'fill')
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
    assert(v.source.x == 0.6 and v.source.y == 0.4)

    -- restore
    pico.set.view {
        target={'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
        source={'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
    }
end

print("OK")
pico.init(false)
