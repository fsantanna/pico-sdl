require 'pico.check'

pico.init(true)
pico.set.window { title="Image - Size - Crop" }
pico.set.color.clear('white')

-- pico.get.image
do
    local dim = pico.get.image("open.png")
    assert(dim.w==48 and dim.h==48)
end
do
    local dim = pico.get.image("open.png", {'!', h=24})
    assert(dim.w==24 and dim.h==24)
end
do
    local dim = pico.get.image("open.png", {'!', w=48, h=nil})
    assert(dim.w==48 and dim.h==48)
end
do
    local dim = pico.get.image("open.png", {'!'})
    assert(dim.w==48 and dim.h==48)
end

do
    print("show top-left from center")
    local r = {'!', x=50, y=50, w=0, h=0, anc='NW'}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-01")
end

do
    print("show small centered")
    local r = {'!', x=50-5, y=50-5, w=10, h=10, anc='NW'}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-02")
end

do
    print("show small/medium distorted")
    local r = {'!', x=50-5, y=50-10, w=10, h=20, anc='NW'}
    pico.output.clear()
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-03")
end

do
    print("show medium normal")
    pico.output.clear()
    local r = {'!', x=50-10, y=50-10, w=20, h=0, anc='NW'}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-04")
end

do
    print("show small normal")
    pico.output.clear()
    local r = {'!', x=50-5, y=50-5, w=0, h=10, anc='NW'}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-05")
end

do
    print("show big centered")
    pico.output.clear()
    local r = {'!', x=50-24, y=50-24, w=0, h=0, anc='NW'}
    pico.output.draw.image("open.png", r)
    pico.check("image_raw-06")
end

-- CROP
do
    print "show big croped"
    pico.output.clear();
    pico.set.crop {'!', x=9, y=9, w=30, h=30, anc='NW'}
    local r1 = {'!', x=50-24, y=50-24, w=0, h=0, anc='NW'}
    pico.output.draw.image("open.png", r1)
    pico.check("image_raw-07")

    print "show medium normal"
    pico.output.clear()
    pico.set.crop()
    local r2 = {'!', x=50-10, y=50-10, w=20, h=0, anc='NW'}
    pico.output.draw.image("open.png", r2)
    pico.check("image_raw-08")
end

pico.init(false)
