local pico = require "pico"

pico.init(true)
pico.set.window { title="Get-Set", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- alpha
print("alpha")
do
    pico.set.alpha(128)
    assert(pico.get.alpha() == 128)
    pico.set.alpha(0)
    assert(pico.get.alpha() == 0)
    pico.set.alpha(255)
    assert(pico.get.alpha() == 255)
end

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

print("OK")
pico.init(false)
