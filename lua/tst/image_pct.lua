require 'pico.check'

function round (v)
    return v>=0 and math.floor(v+0.5) or math.ceil(v-0.5)
end

pico.init(true)
pico.set.title("Image - Size - Crop")
pico.set.color.clear(0xFF, 0xFF, 0xFF)

-- pico.get.image
do
    local pct = pico.get.image("open.png", {'%', h=0.24})
    assert(round(pct.w*100)==24 and round(pct.h*100)==24)

    local pct = pico.get.image("open.png", {'%', w=0.48})
    assert(round(pct.w*100)==48 and round(pct.h*100)==48)

    local pct = pico.get.image("open.png", {'%'})
    assert(round(pct.w*100)==48 and round(pct.h*100)==48)

    local ref = {'NW', x=0, y=0, w=0.5, h=0.5}
    local pct = pico.get.image("open.png", {'%', w=nil, h=nil}, ref)
    assert(round(pct.w*100)==96 and round(pct.h*100)==96)

    local ref = {'NW', x=0, y=0, w=0.5, h=0.5}
    local pct = pico.get.image("open.png", {'%',h=0.48}, ref)
    assert(round(pct.w*100)==48 and round(pct.h*100)==48)
end

do
    print("show original centered")
    pico.output.clear()
    local r = {'C', h=0.5, y=0.5}
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
    local r = {'C', x=0.5, y=0.5, w=0.5}
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
