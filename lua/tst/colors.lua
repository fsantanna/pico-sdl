require 'pico.check'

pico.init(true)
pico.set.window { title="Colors Test" }
pico.set.layer("window")
pico.set.scene { dim={'!', w=640, h=480} }
pico.set.layer("world")
pico.set.scene { dim={'!', w=64, h=48} }

do
    print("predefined colors")
    pico.output.clear()

    pico.set.pencil { color='red' }
    pico.output.draw.rect({'%', x=0.5, y=0.1, w=0.8, h=0.05})

    pico.set.pencil { color='green' }
    pico.output.draw.rect({'%', x=0.5, y=0.2, w=0.8, h=0.05})

    pico.set.pencil { color='blue' }
    pico.output.draw.rect({'%', x=0.5, y=0.3, w=0.8, h=0.05})

    pico.set.pencil { color='yellow' }
    pico.output.draw.rect({'%', x=0.5, y=0.4, w=0.8, h=0.05})

    pico.set.pencil { color='cyan' }
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.05})

    pico.set.pencil { color='magenta' }
    pico.output.draw.rect({'%', x=0.5, y=0.6, w=0.8, h=0.05})

    pico.set.pencil { color='orange' }
    pico.output.draw.rect({'%', x=0.5, y=0.7, w=0.8, h=0.05})

    pico.set.pencil { color='purple' }
    pico.output.draw.rect({'%', x=0.5, y=0.8, w=0.8, h=0.05})

    pico.set.pencil { color='pink' }
    pico.output.draw.rect({'%', x=0.5, y=0.9, w=0.8, h=0.05})

    pico.check("colors-01")
end

do
    print("darker function")
    pico.output.clear()
    for i = 0, 8 do
        local c = pico.color.darker('red', i*0.1)
        pico.set.pencil { color=c }
        pico.output.draw.rect({'%', x=0.5, y=0.1+i*0.1, w=0.8, h=0.05})
    end
    pico.check("colors-02")
end

do
    print("lighter function")
    pico.output.clear()
    for i = 0, 8 do
        local c = pico.color.lighter('blue', i*0.1)
        pico.set.pencil { color=c }
        pico.output.draw.rect({'%', x=0.5, y=0.1+i*0.1, w=0.8, h=0.05})
    end
    pico.check("colors-03")
end

do
    print("negative percentages")
    pico.output.clear()

    -- -dark -> light
    pico.set.pencil { color=pico.color.darker('green', -0.50) }
    pico.output.draw.rect({'%', x=0.5, y=0.25, w=0.5, h=0.15})

    -- green
    pico.set.pencil { color='green' }
    pico.output.draw.rect({'%', x=0.5, y=0.50, w=0.5, h=0.15})

    -- -light -> dark
    pico.set.pencil { color=pico.color.lighter('green', -0.50) }
    pico.output.draw.rect({'%', x=0.5, y=0.75, w=0.5, h=0.15})

    pico.check("colors-04")
end

do
    print("mix function")
    pico.output.clear()

    -- red + blue = purple
    local c1 = pico.color.mix('red', 'blue')
    pico.set.pencil { color=c1 }
    pico.output.draw.rect({'%', x=0.5, y=0.15, w=0.8, h=0.1})

    -- red + green = olive
    local c2 = pico.color.mix('red', 'green')
    pico.set.pencil { color=c2 }
    pico.output.draw.rect({'%', x=0.5, y=0.35, w=0.8, h=0.1})

    -- blue + green = teal
    local c3 = pico.color.mix('blue', 'green')
    pico.set.pencil { color=c3 }
    pico.output.draw.rect({'%', x=0.5, y=0.55, w=0.8, h=0.1})

    -- black + white = gray
    local c4 = pico.color.mix('black', 'white')
    pico.set.pencil { color=c4 }
    pico.output.draw.rect({'%', x=0.5, y=0.75, w=0.8, h=0.1})

    pico.check("colors-05")
end

do
    print("percentage color format")
    pico.output.clear()

    -- {'%', r, g, b} format - percentage (0.0-1.0)
    pico.set.pencil { color={'%', r=1, g=0, b=0} }
    pico.output.draw.rect({'%', x=0.5, y=0.15, w=0.8, h=0.1})

    pico.set.pencil { color={'%', r=0, g=1, b=0} }
    pico.output.draw.rect({'%', x=0.5, y=0.30, w=0.8, h=0.1})

    pico.set.pencil { color={'%', r=0, g=0, b=1} }
    pico.output.draw.rect({'%', x=0.5, y=0.45, w=0.8, h=0.1})

    -- 50% gray
    pico.set.pencil { color={'%', r=0.5, g=0.5, b=0.5} }
    pico.output.draw.rect({'%', x=0.5, y=0.60, w=0.8, h=0.1})

    -- {'!', r, g, b} format - explicit absolute (0-255)
    pico.set.pencil { color={'!', r=255, g=255, b=0} }
    pico.output.draw.rect({'%', x=0.5, y=0.75, w=0.8, h=0.1})

    -- {r, g, b} format - implicit absolute (backward compatible)
    pico.set.pencil { color={r=0, g=255, b=255} }
    pico.output.draw.rect({'%', x=0.5, y=0.90, w=0.8, h=0.1})

    pico.check("colors-06")
end

do
    print("hex color format")
    pico.output.clear()

    pico.set.pencil { color=0xFF0000 }
    pico.output.draw.rect({'%', x=0.5, y=0.15, w=0.8, h=0.1})

    pico.set.pencil { color=0x00FF00 }
    pico.output.draw.rect({'%', x=0.5, y=0.30, w=0.8, h=0.1})

    pico.set.pencil { color=0x0000FF }
    pico.output.draw.rect({'%', x=0.5, y=0.45, w=0.8, h=0.1})

    pico.set.pencil { color=0x7F7F7F }
    pico.output.draw.rect({'%', x=0.5, y=0.60, w=0.8, h=0.1})

    pico.set.pencil { color=0xFFFF00 }
    pico.output.draw.rect({'%', x=0.5, y=0.75, w=0.8, h=0.1})

    pico.set.pencil { color=0x00FFFF }
    pico.output.draw.rect({'%', x=0.5, y=0.90, w=0.8, h=0.1})

    pico.check("colors-06")
end

do
    print("hex color conversion")

    local c = pico.color.darker(0xFF0000, 0)
    assert(c.r==255 and c.g==0 and c.b==0)

    c = pico.color.darker(0x00FF00, 0)
    assert(c.r==0 and c.g==255 and c.b==0)

    c = pico.color.darker(0x0000FF, 0)
    assert(c.r==0 and c.g==0 and c.b==255)

    c = pico.color.darker(0x336699, 0)
    assert(c.r==0x33 and c.g==0x66 and c.b==0x99)

    c = pico.color.darker(0x000000, 0)
    assert(c.r==0 and c.g==0 and c.b==0)

    c = pico.color.darker(0xFFFFFF, 0)
    assert(c.r==255 and c.g==255 and c.b==255)
end

do
    print("color alpha function")
    pico.set.effect { color='white' }
    pico.output.clear()
    local pixmap = {
        {
            pico.color.alpha('red', 0xFF),
            pico.color.alpha('red', 0x80),
            'transparent',
        },
    }
    local r = {'%', x=0.5, y=0.5, w=0, h=0}
    pico.output.draw.pixmap("alpha", pixmap, r)
    pico.check("colors-07")
end

do
    print("pixmap with mixed color formats")
    pico.set.effect { color='white' }
    pico.output.clear()
    local pixmap = {
        { 'red',                 0x00FF00, { r=0,   g=0,   b=255 } },
        { {'%', r=1, g=1, b=0},  'cyan',   {'!', r=255, g=0, b=255} },
        { 0x000000,              'black',  pico.color.alpha('red', 0x80) },
    }
    local r = {'%', x=0.5, y=0.5, w=0, h=0}
    pico.output.draw.pixmap("mixed", pixmap, r)
    pico.check("colors-0X")
end

-- window letterbox color: shrink dst to expose letterbox area
do
    do
        pico.set.effect { color='black' }
        pico.set.scene { target={'%', x=1, y=1, w=0.5, h=0.5, anchor='SE'} }
    end
    do
        print("window color default: gray {0x77,0x77,0x77,0xFF}")
        pico.set.layer("window")
        local c = pico.get.effect().color
        pico.set.layer("world")
        assert(c.r==0x77 and c.g==0x77 and c.b==0x77 and c.a==0xFF)
        pico.output.clear()
        pico.set.pencil { color='white' }
        pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
        pico.check("colors-08")
    end
    do
        print("window color: red letterbox")
        pico.set.layer("window")
        pico.set.effect { color={'!', r=0xFF, g=0, b=0, a=0xFF} }
        local c = pico.get.effect().color
        pico.set.layer("world")
        assert(c.r==0xFF and c.g==0 and c.b==0 and c.a==0xFF)
        pico.output.clear()
        pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
        pico.check("colors-09")
    end
    do
        print("window color: green via bulk")
        pico.set.layer("window")
        pico.set.effect { color={'!', r=0, g=0xFF, b=0, a=0xFF} }
        local c = pico.get.effect().color
        pico.set.layer("world")
        assert(c.r==0 and c.g==0xFF and c.b==0 and c.a==0xFF)
        pico.output.clear()
        pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
        pico.check("colors-10")
    end
    do
        print("window color: alpha preserved by storage")
        pico.set.layer("window")
        pico.set.effect { color={'!', r=0, g=0, b=0xFF, a=0x80} }
        local c = pico.get.effect().color
        pico.set.layer("world")
        assert(c.a == 0x80)
    end
end

pico.init(false)
