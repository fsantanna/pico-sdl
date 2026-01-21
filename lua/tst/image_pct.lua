require 'pico.check'

pico.init(true)
pico.set.title("Image - Size - Crop")
pico.set.color.clear(0xFF, 0xFF, 0xFF)

-- pico.get.image: NULL ref (world 100x100, image 48x48)
do
    local dim = pico.get.image("open.png", {'%', w=0, h=0.24})
    assert(dim.w == 0.24 and dim.h == 0.24)
end
do
    local dim = pico.get.image("open.png", {'%', w=0.48, h=0})
    assert(dim.w == 0.48 and dim.h == 0.48)
end
do
    local dim = pico.get.image("open.png", {'%', w=0, h=0})
    assert(dim.w == 0.48 and dim.h == 0.48)
end
-- pico.get.image: with ref (ref 50x50, image 48x48 -> 0.96x0.96)
do
    local ref = {'NW', x=0, y=0, w=0.5, h=0.5}
    local dim = pico.get.image("open.png", {'%', w=0, h=0, up=ref})
    assert(dim.w == 0.96 and dim.h == 0.96)
end
do
    local ref = {'NW', x=0, y=0, w=0.5, h=0.5}
    local dim = pico.get.image("open.png", {'%', w=0, h=0.48, up=ref})
    assert(dim.w == 0.48 and dim.h == 0.48)
end

do
    print("show original centered")
    pico.output.clear()
    local r = {'C', x=0.5, y=0.5, w=0, h=0}
    pico.output.draw.image("open.png", r)
    pico.check("image_pct-01")
end

do
    print("show big centered")
    pico.output.clear()
    local r = {'C', x=0.5, y=0.5, w=1.0, h=1.0}
    pico.output.draw.image("open.png", r)
    pico.check("image_pct-02")
end

do
    print("show small centered")
    pico.output.clear()
    local r = {'C', x=0.5, y=0.5, w=0.2, h=0.2}
    pico.output.draw.image("open.png", r)
    pico.check("image_pct-03")
end

do
    print("show w-half proportional")
    pico.output.clear()
    local r = {'C', x=0.5, y=0.5, w=0.5, h=0}
    pico.output.draw.image("open.png", r)
    pico.check("image_pct-04")
end

do
    print("show w-half h-quart distorted")
    pico.output.clear()
    local r = {'C', x=0.5, y=0.5, w=0.5, h=0.25}
    pico.output.draw.image("open.png", r)
    pico.check("image_pct-05")
end

pico.init(false)
