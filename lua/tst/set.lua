require 'pico.check'

pico.init(true)
pico.set.window { title="Set Test",
                   dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

-- set all: alpha comparison (left=0x80, right=0xFF)
do
    print("set all: alpha comparison")
    pico.output.clear()
    pico.set { alpha=0x80, color={draw='red'} }
    pico.output.draw.rect({'%', x=0.25, y=0.5, w=0.4, h=0.8})
    pico.set { alpha=0xFF }
    pico.output.draw.rect({'%', x=0.75, y=0.5, w=0.4, h=0.8})
    pico.check("set-01")
end

-- set all: style (stroke rect)
do
    print("set all: style")
    pico.output.clear()
    pico.set { style='stroke', color={draw='white'} }
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.8})
    pico.check("set-02")
end

-- individual setters (green rect with alpha)
do
    print("individual setters")
    pico.output.clear()
    pico.set.color.draw('green')
    pico.set.alpha(0xCC)
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})
    pico.check("set-03")
end

pico.init(false)
