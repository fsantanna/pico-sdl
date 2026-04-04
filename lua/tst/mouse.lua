pico.init(true)

print("phy (500,500) -> log (50,50)")
do
    pico.set.window { title="Mouse", dim={'!', w=500, h=500} }
    pico.set.view { dim={'!', w=50, h=50} }

    -- phy (0,0) -> log (0,0)
    do
        pico.set.mouse({'w', x=0, y=3})
        local pos = pico.get.mouse('!')
        assert(pos.x==0 and pos.y>0.29 and pos.y<0.31)
    end

    -- phy (250,250) -> log (25,25)
    do
        pico.set.mouse({'w', x=250, y=251})
        local pos = pico.get.mouse('!')
        assert(pos.x==25 and pos.y>25.09 and pos.y<25.11)
    end

    -- phy (490,490) -> log (49,49)
    do
        pico.set.mouse({'w', x=499, y=490})
        local pos = pico.get.mouse('!')
        assert(pos.x>49.89 and pos.x<49.91 and pos.y==49)
    end
end

-- Zoom out: src = {-25, -25, 100, 100}
-- centered 100x100 logical
print("zoom out 2x")
do
    pico.set.view { source={'!', x=-25, y=-25, w=100, h=100} }

    -- phy (250, 250) -> log (25,25)
    do
        pico.set.mouse({'w', x=250, y=253})
        local pos = pico.get.mouse('!')
        assert(pos.x==25 and pos.y>25.59 and pos.y<25.61)
    end

    -- phy (0,0) -> log (-25,-25)
    do
        pico.set.mouse({'w', x=0, y=0})
        local pos = pico.get.mouse('!')
        assert(pos.x==-25 and pos.y==-25)
    end

    -- phy (500,500) -> log (75,75)
    do
        pico.set.mouse({'w', x=495, y=499})
        local pos = pico.get.mouse('!')
        assert(pos.x==74 and pos.y>74.79 and pos.y<74.81)
    end
end

-- Zoom in: src = {20, 20, 10, 10}
-- centered 10x10 logical
print("zoom in 5x")
do
    pico.set.view { source={'!', x=20, y=20, w=10, h=10} }

    -- phy (0,0) -> log (20,20)
    do
        pico.set.mouse({'w', x=1, y=2})
        local pos = pico.get.mouse('!')
        assert(pos.x>20.01 and pos.x<20.03 and pos.y>20.03 and pos.y<20.05)
    end

    -- phy center (250,250) -> log (25,25)
    do
        pico.set.mouse({'w', x=254, y=251})
        local pos = pico.get.mouse('!')
        assert(pos.x>25.07 and pos.x<25.09 and pos.y>25.01 and pos.y<25.03)
    end

    -- phy (500,500) -> log (30,30)
    do
        pico.set.mouse({'w', x=497, y=498})
        local pos = pico.get.mouse('!')
        assert(pos.x>29.93 and pos.x<29.95 and pos.y>29.95 and pos.y<29.97)
    end
end

-- Scroll/Zoom in: src = {25, 25, 25, 25}
-- bottom-right (SE) half
print("scroll/zoom SE")
do
    pico.set.view { source={'!', x=25, y=25, w=25, h=25} }

    -- phy (0,0) -> log (25,25)
    do
        pico.set.mouse({'w', x=2, y=1})
        local pos = pico.get.mouse('!')
        assert(pos.x>25.09 and pos.x<25.11 and pos.y>25.04 and pos.y<25.06)
    end

    -- phy (250,250) -> log (37,37)
    do
        pico.set.mouse({'w', x=253, y=250})
        local pos = pico.get.mouse('!')
        assert(pos.x>37.64 and pos.x<37.66 and pos.y==37.5)
    end
end

print("normal PCT")
do
    pico.set.view { source={'!', x=0, y=0, w=50, h=50} }

    -- phy (250,250) -> pct (0.5,0.5)
    do
        pico.set.mouse({'w', x=250, y=250})
        local pos = pico.get.mouse('%')
        assert(pos.x==0.5 and pos.y==0.5)
    end

    -- phy (0,0) -> pct (0,0)
    do
        pico.set.mouse({'w', x=0, y=0})
        local pos = pico.get.mouse('%')
        assert(pos.x==0.0 and pos.y==0.0)
    end
end

print("zoom PCT")
do
    pico.set.view { source={'!', x=20, y=20, w=10, h=10} }

    -- phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
    do
        pico.set.mouse({'w', x=0, y=0})
        local pos = pico.get.mouse('%')
        assert(pos.x>0.39 and pos.x<0.41)
        assert(pos.y>0.39 and pos.y<0.41)
    end

    -- phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
    do
        pico.set.mouse({'w', x=250, y=250})
        local pos = pico.get.mouse('%')
        assert(pos.x>0.49 and pos.x<0.51)
        assert(pos.y>0.49 and pos.y<0.51)
    end
end

print("window mode (raw phy)")
do
    pico.set.mouse({'w', x=123, y=456})
    local pos = pico.get.mouse('w')
    assert(pos.x==123 and pos.y==456)
end

-- roundtrip: set(rel) -> get(rel) -> assert equal
print("roundtrip '!'")
do
    pico.set.view { source={'!', x=0, y=0, w=50, h=50} }

    pico.set.mouse({'!', x=25, y=25})
    local pos = pico.get.mouse('!')
    assert(pos.x==25 and pos.y==25)
end

print("roundtrip '%'")
do
    pico.set.mouse({'%', x=0.5, y=0.5})
    local pos = pico.get.mouse('%')
    assert(pos.x>0.49 and pos.x<0.51)
    assert(pos.y>0.49 and pos.y<0.51)
end

print("roundtrip '!' zoomed")
do
    pico.set.view { source={'!', x=20, y=20, w=10, h=10} }

    pico.set.mouse({'!', x=25, y=25})
    local pos = pico.get.mouse('!')
    assert(pos.x==25 and pos.y==25)
end

pico.init(false)
