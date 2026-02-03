require 'pico.check'

pico.init(true)
pico.set.window { title="Colors Test", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

do
    print("predefined colors")
    pico.output.clear()

    pico.set.color.draw('red')
    pico.output.draw.rect({'%', x=0.5, y=0.1, w=0.8, h=0.05})

    pico.set.color.draw('green')
    pico.output.draw.rect({'%', x=0.5, y=0.2, w=0.8, h=0.05})

    pico.set.color.draw('blue')
    pico.output.draw.rect({'%', x=0.5, y=0.3, w=0.8, h=0.05})

    pico.set.color.draw('yellow')
    pico.output.draw.rect({'%', x=0.5, y=0.4, w=0.8, h=0.05})

    pico.set.color.draw('cyan')
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.05})

    pico.set.color.draw('magenta')
    pico.output.draw.rect({'%', x=0.5, y=0.6, w=0.8, h=0.05})

    pico.set.color.draw('orange')
    pico.output.draw.rect({'%', x=0.5, y=0.7, w=0.8, h=0.05})

    pico.set.color.draw('purple')
    pico.output.draw.rect({'%', x=0.5, y=0.8, w=0.8, h=0.05})

    pico.set.color.draw('pink')
    pico.output.draw.rect({'%', x=0.5, y=0.9, w=0.8, h=0.05})

    pico.check("colors-01")
end

do
    print("darker function")
    pico.output.clear()
    for i = 0, 8 do
        local c = pico.color.darker('red', i*0.1)
        pico.set.color.draw(c)
        pico.output.draw.rect({'%', x=0.5, y=0.1+i*0.1, w=0.8, h=0.05})
    end
    pico.check("colors-02")
end

do
    print("lighter function")
    pico.output.clear()
    for i = 0, 8 do
        local c = pico.color.lighter('blue', i*0.1)
        pico.set.color.draw(c)
        pico.output.draw.rect({'%', x=0.5, y=0.1+i*0.1, w=0.8, h=0.05})
    end
    pico.check("colors-03")
end

do
    print("negative percentages")
    pico.output.clear()

    -- -dark -> light
    pico.set.color.draw(pico.color.darker('green', -0.50))
    pico.output.draw.rect({'%', x=0.5, y=0.25, w=0.5, h=0.15})

    -- green
    pico.set.color.draw('green')
    pico.output.draw.rect({'%', x=0.5, y=0.50, w=0.5, h=0.15})

    -- -light -> dark
    pico.set.color.draw(pico.color.lighter('green', -0.50))
    pico.output.draw.rect({'%', x=0.5, y=0.75, w=0.5, h=0.15})

    pico.check("colors-04")
end

pico.init(false)
