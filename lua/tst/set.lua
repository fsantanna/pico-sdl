require 'pico.check'

pico.init(true)
pico.set.window { title="Set Test",
                   dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- set all: alpha and color
do
    print("set all: alpha and color")
    pico.output.clear()
    pico.set {
        alpha = 0x80,
        color = { draw='red' },
    }
    pico.output.draw.rect(
        {'%', x=0.5, y=0.5, w=0.8, h=0.8})
    pico.set { alpha=0xFF, color={draw='white'} }
    pico.check("set-01")
end

-- set all: style
do
    print("set all: style")
    pico.output.clear()
    pico.set {
        style  = 'stroke',
        color  = { draw='white' },
    }
    pico.output.draw.rect(
        {'%', x=0.5, y=0.5, w=0.8, h=0.8})
    pico.set { style='fill' }
    pico.check("set-02")
end

-- individual setters still work
do
    print("individual setters unchanged")
    pico.output.clear()
    pico.set.color.draw('green')
    pico.set.alpha(0xCC)
    pico.output.draw.rect(
        {'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.set.alpha(0xFF)
    pico.set.color.draw('white')
    pico.check("set-03")
end

pico.init(false)
