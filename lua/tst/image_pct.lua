require 'pico.check'

pico.init(true)
pico.set.title("Image - Size - Crop")
pico.set.color.clear(0xFF, 0xFF, 0xFF)

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
