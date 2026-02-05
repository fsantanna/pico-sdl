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

-- push/pop: draw color and alpha
do
    print("push/pop: draw color and alpha")
    pico.output.clear()

    pico.set.color.draw('white')
    pico.set.alpha(0xFF)
    pico.output.draw.rect(
        {'%', x=0.25, y=0.5, w=0.3, h=0.8})

    pico.push {
        alpha = 0x80,
        color = { draw='red' },
    }
    pico.output.draw.rect(
        {'%', x=0.75, y=0.5, w=0.3, h=0.8})
    pico.pop()

    -- after pop: back to white + 0xFF
    pico.output.draw.rect(
        {'%', x=0.5, y=0.5, w=0.1, h=0.1})
    pico.check("set-03")
end

-- nested push/pop
do
    print("nested push/pop")
    pico.output.clear()

    pico.set.color.draw('white')
    pico.output.draw.rect(
        {'%', x=0.25, y=0.25, w=0.3, h=0.3})

    pico.push { color={ draw='red' } }
    pico.output.draw.rect(
        {'%', x=0.75, y=0.25, w=0.3, h=0.3})

    pico.push { color={ draw='blue' } }
    pico.output.draw.rect(
        {'%', x=0.25, y=0.75, w=0.3, h=0.3})
    pico.pop()

    -- back to red
    pico.output.draw.rect(
        {'%', x=0.75, y=0.75, w=0.3, h=0.3})
    pico.pop()

    pico.check("set-04")
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
    pico.check("set-05")
end

pico.init(false)
