require 'pico.check'

pico.init(true)
pico.set.window { title="Push/Pop", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- push/pop restores color
do
    print("push/pop color")
    pico.output.clear()
    pico.set.color.draw('red')

    pico.push()
    pico.set.color.draw('blue')
    pico.output.draw.rect(
        {'%', x=0.5, y=0.25, w=0.8, h=0.3})
    pico.pop()

    -- should be red again
    pico.output.draw.rect(
        {'%', x=0.5, y=0.75, w=0.8, h=0.3})
    pico.check("push-01")
end

-- push/pop restores style
do
    print("push/pop style")
    pico.output.clear()
    pico.set.color.draw('white')
    pico.set.style('fill')

    pico.push()
    pico.set.style('stroke')
    pico.output.draw.rect(
        {'%', x=0.5, y=0.25, w=0.8, h=0.3})
    pico.pop()

    -- should be fill again
    pico.output.draw.rect(
        {'%', x=0.5, y=0.75, w=0.8, h=0.3})
    pico.check("push-02")
end

-- push/pop restores alpha
do
    print("push/pop alpha")
    pico.output.clear()
    pico.set.color.draw('red')
    pico.set.alpha(255)

    pico.push()
    pico.set.alpha(64)
    pico.output.draw.rect(
        {'%', x=0.5, y=0.25, w=0.8, h=0.3})
    pico.pop()

    -- should be fully opaque again
    pico.output.draw.rect(
        {'%', x=0.5, y=0.75, w=0.8, h=0.3})
    pico.check("push-03")
end

-- nested push/pop
do
    print("nested push/pop")
    pico.output.clear()
    pico.set.color.draw('red')

    pico.push()
    pico.set.color.draw('green')

    pico.push()
    pico.set.color.draw('blue')
    pico.output.draw.rect(
        {'%', x=0.5, y=0.17, w=0.8, h=0.2})
    pico.pop()

    -- should be green
    pico.output.draw.rect(
        {'%', x=0.5, y=0.50, w=0.8, h=0.2})
    pico.pop()

    -- should be red
    pico.output.draw.rect(
        {'%', x=0.5, y=0.83, w=0.8, h=0.2})
    pico.check("push-04")
end

pico.init(false)
