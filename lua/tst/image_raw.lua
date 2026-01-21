require 'pico.check'

pico.init(true)
pico.set.title("Image - Size - Crop")
pico.set.color.clear(0xFF, 0xFF, 0xFF)

-- pico.get.image
do
    local dim = pico.get.image("open.png")
    assert(dim.w==48 and dim.h==48)
end
do
    local dim = pico.get.image("open.png", {w=0, h=24})
    assert(dim.w==24 and dim.h==24)
end
do
    local dim = pico.get.image("open.png", {w=48, h=0})
    assert(dim.w==48 and dim.h==48)
end
do
    local dim = pico.get.image("open.png", {w=0, h=0})
    assert(dim.w==48 and dim.h==48)
end

do
    print("show top-left from center")
    local r = {x=50, y=50, w=0, h=0}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-01")
end

do
    print("show small centered")
    local r = {x=50-5, y=50-5, w=10, h=10}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-02")
end

do
    print("show small/medium distorted")
    local r = {x=50-5, y=50-10, w=10, h=20}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-03")
end

do
    print("show medium normal")
    pico.output.clear()
    local r = {x=50-10, y=50-10, w=20, h=0}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-04")
end

do
    print("show small normal")
    pico.output.clear()
    local r = {x=50-5, y=50-5, w=0, h=10}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-05")
end

do
    print("show big centered")
    pico.output.clear()
    local r = {x=50-24, y=50-24, w=0, h=0}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-06")
end

pico.init(false)
