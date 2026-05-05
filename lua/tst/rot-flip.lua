require 'pico.check'

pico.init(true)
pico.set.window { title="Rotate / Flip" }
pico.set.effect { color='white' }

-- Create a layer from image for rotate/flip tests
pico.layer.image('!', nil, "img", "../../res/open.png")

-- ROTATION - center anchor
do
    print("rotate 0 degrees (no rotate)")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=0, anchor='C' } })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-01")
end
do
    print("rotate 45 degrees")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=45, anchor='C' } })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-02")
end
do
    print("rotate 90 degrees")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=90, anchor='C' } })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-03")
end
do
    print("rotate 180 degrees")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=180, anchor='C' } })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-04")
end

-- ROTATION - corner anchor (NW)
do
    print("rotate 45 degrees, anchor NW")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=45, anchor='NW' } })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-05")
end

-- Reset rotate for flip tests
do
    
    pico.set.effect("img", { rotate = { angle=0, anchor='C' } })
    
end

-- FLIP - horizontal
do
    print("flip horizontal")
    pico.output.clear()
    
    pico.set.effect("img", { flip = "horizontal" })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-06")
end
do
    print("flip vertical")
    pico.output.clear()
    
    pico.set.effect("img", { flip = "vertical" })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-07")
end
do
    print("flip both")
    pico.output.clear()
    
    pico.set.effect("img", { flip = "both" })
    
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-08")
end

-- COMBINED - rotate + flip
do
    print("rotate 45 + flip horizontal")
    pico.output.clear()
    
    pico.set.effect("img", { rotate = { angle=45, anchor='C' }, flip = "horizontal" })

    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    pico.output.draw.layer("img", r)
    pico.check("rot-flip-09")
end

-- ANIMATED ROTATION - center anchor (default)
do
    print("animated rotate - center anchor")
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    for angle = 0, 355, 5 do
        pico.output.clear()
        
        pico.set.effect("img", { rotate = { angle=angle, anchor='C' } })
        
        pico.output.draw.layer("img", r)
        if angle == 180 then
            pico.check("rot-flip-10")
        end
        pico.input.delay(10)
    end
end

-- ANIMATED ROTATION - anchor outside (1.1, 1.1)
do
    print("animated rotate - anchor outside (1.1, 1.1)")
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    for angle = 0, 355, 5 do
        pico.output.clear()
        
        pico.set.effect("img", { rotate = { angle=angle, anchor={x=1.1, y=1.1} } })
        
        pico.output.draw.layer("img", r)
        if angle == 180 then
            pico.check("rot-flip-11")
        end
        pico.input.delay(10)
    end
end

-- ANIMATED ROTATION - anchor negative (-0.1, -0.1)
do
    print("animated rotate - anchor negative (-0.1, -0.1)")
    local r = {'!', x=50, y=50, w=48, h=48, anchor='C'}
    for angle = 0, 355, 5 do
        pico.output.clear()
        
        pico.set.effect("img", { rotate = { angle=angle, anchor={x=-0.1, y=-0.1} } })
        
        pico.output.draw.layer("img", r)
        if angle == 180 then
            pico.check("rot-flip-12")
        end
        pico.input.delay(10)
    end
end

pico.init(false)
