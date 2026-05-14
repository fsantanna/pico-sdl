pico.init(true)

-- helper: window-pixel mouse set (NW anchor)
local function mouse_w (x, y)
    local old = pico.set.layer("window")
    pico.set.mouse({'!', x=x, y=y, anchor='NW'})
    pico.set.layer(old)
end

-- helper: each call site is exercised in both forms
--   * table form `{mode, anchor='NW'}` — preserves legacy NW numbers
--   * mode-string form — new C-anchor default; result is shifted by
--     +0.5 for '!'  and  +0.5/scene.dim.w for '%'

print("phy (500,500) -> log (50,50)")
do
    pico.set.window { title="Mouse" }
    pico.set.layer("window")
    pico.set.scene { dim={'!', w=500, h=500} }
    pico.set.layer("world")
    pico.set.scene { dim={'!', w=50, h=50} }

    -- phy (0,0) -> log (0,0)
    do
        mouse_w(0, 3)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x==0 and pos.y>0.29 and pos.y<0.31)
        local pos = pico.get.mouse(nil, {'!', anchor='NW'})
        assert(pos.x==0 and pos.y>0.29 and pos.y<0.31)
        local pos = pico.get.mouse('!')
        assert(pos.anchor.x==0.5 and pos.anchor.y==0.5)
        assert(pos.x==0.5 and pos.y>0.79 and pos.y<0.81)
        local pos = pico.get.mouse(nil, '!')
        assert(pos.x==0.5 and pos.y>0.79 and pos.y<0.81)
    end

    -- phy (250,250) -> log (25,25)
    do
        mouse_w(250, 251)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x==25 and pos.y>25.09 and pos.y<25.11)
        local pos = pico.get.mouse('!')
        assert(pos.x==25.5 and pos.y>25.59 and pos.y<25.61)
    end

    -- phy (490,490) -> log (49,49)
    do
        mouse_w(499, 490)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>49.89 and pos.x<49.91 and pos.y==49)
        local pos = pico.get.mouse('!')
        assert(pos.x>50.39 and pos.x<50.41 and pos.y==49.5)
    end
end

-- Zoom out: src = {-25, -25, 100, 100}
-- centered 100x100 logical
print("zoom out 2x")
do
    pico.set.scene { source={'!', x=-25, y=-25, w=100, h=100, anchor='NW'} }

    -- phy (250, 250) -> log (25,25)
    do
        mouse_w(250, 253)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x==25 and pos.y>25.59 and pos.y<25.61)
        local pos = pico.get.mouse('!')
        assert(pos.x==25.5 and pos.y>26.09 and pos.y<26.11)
    end

    -- phy (0,0) -> log (-25,-25)
    do
        mouse_w(0, 0)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x==-25 and pos.y==-25)
        local pos = pico.get.mouse('!')
        assert(pos.x==-24.5 and pos.y==-24.5)
    end

    -- phy (500,500) -> log (75,75)
    do
        mouse_w(495, 499)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x==74 and pos.y>74.79 and pos.y<74.81)
        local pos = pico.get.mouse('!')
        assert(pos.x==74.5 and pos.y>75.29 and pos.y<75.31)
    end
end

-- Zoom in: src = {20, 20, 10, 10}
-- centered 10x10 logical
print("zoom in 5x")
do
    pico.set.scene { source={'!', x=20, y=20, w=10, h=10, anchor='NW'} }

    -- phy (0,0) -> log (20,20)
    do
        mouse_w(1, 2)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>20.01 and pos.x<20.03 and pos.y>20.03 and pos.y<20.05)
        local pos = pico.get.mouse('!')
        assert(pos.x>20.51 and pos.x<20.53 and pos.y>20.53 and pos.y<20.55)
    end

    -- phy center (250,250) -> log (25,25)
    do
        mouse_w(254, 251)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>25.07 and pos.x<25.09 and pos.y>25.01 and pos.y<25.03)
        local pos = pico.get.mouse('!')
        assert(pos.x>25.57 and pos.x<25.59 and pos.y>25.51 and pos.y<25.53)
    end

    -- phy (500,500) -> log (30,30)
    do
        mouse_w(497, 498)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>29.93 and pos.x<29.95 and pos.y>29.95 and pos.y<29.97)
        local pos = pico.get.mouse('!')
        assert(pos.x>30.43 and pos.x<30.45 and pos.y>30.45 and pos.y<30.47)
    end
end

-- Scroll/Zoom in: src = {25, 25, 25, 25}
-- bottom-right (SE) half
print("scroll/zoom SE")
do
    pico.set.scene { source={'!', x=25, y=25, w=25, h=25, anchor='NW'} }

    -- phy (0,0) -> log (25,25)
    do
        mouse_w(2, 1)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>25.09 and pos.x<25.11 and pos.y>25.04 and pos.y<25.06)
        local pos = pico.get.mouse('!')
        assert(pos.x>25.59 and pos.x<25.61 and pos.y>25.54 and pos.y<25.56)
    end

    -- phy (250,250) -> log (37,37)
    do
        mouse_w(253, 250)
        local pos = pico.get.mouse({'!', anchor='NW'})
        assert(pos.x>37.64 and pos.x<37.66 and pos.y==37.5)
        local pos = pico.get.mouse('!')
        assert(pos.x>38.14 and pos.x<38.16 and pos.y==38.0)
    end
end

print("normal PCT")
do
    pico.set.scene { source={'!', x=0, y=0, w=50, h=50, anchor='NW'} }

    -- phy (250,250) -> pct (0.5,0.5)
    do
        mouse_w(250, 250)
        local pos = pico.get.mouse({'%', anchor='NW'})
        assert(pos.x==0.5 and pos.y==0.5)
        local pos = pico.get.mouse('%')
        assert(pos.anchor.x==0.5 and pos.anchor.y==0.5)
        assert(pos.x>0.509 and pos.x<0.511 and pos.y>0.509 and pos.y<0.511)
    end

    -- phy (0,0) -> pct (0,0)
    do
        mouse_w(0, 0)
        local pos = pico.get.mouse({'%', anchor='NW'})
        assert(pos.x==0.0 and pos.y==0.0)
        local pos = pico.get.mouse('%')
        assert(pos.x>0.009 and pos.x<0.011 and pos.y>0.009 and pos.y<0.011)
    end
end

print("zoom PCT")
do
    pico.set.scene { source={'!', x=20, y=20, w=10, h=10, anchor='NW'} }

    -- phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
    do
        mouse_w(0, 0)
        local pos = pico.get.mouse({'%', anchor='NW'})
        assert(pos.x>0.39 and pos.x<0.41)
        assert(pos.y>0.39 and pos.y<0.41)
        local pos = pico.get.mouse('%')
        assert(pos.x>0.40 and pos.x<0.42)
        assert(pos.y>0.40 and pos.y<0.42)
    end

    -- phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
    do
        mouse_w(250, 250)
        local pos = pico.get.mouse({'%', anchor='NW'})
        assert(pos.x>0.49 and pos.x<0.51)
        assert(pos.y>0.49 and pos.y<0.51)
        local pos = pico.get.mouse('%')
        assert(pos.x>0.50 and pos.x<0.52)
        assert(pos.y>0.50 and pos.y<0.52)
    end
end

print("window mode (raw phy)")
do
    mouse_w(123, 456)
    local old = pico.set.layer("window")
    local pos = pico.get.mouse({'!', anchor='NW'})
    pico.set.layer(old)
    assert(pos.x==123 and pos.y==456)
    -- equivalent via explicit layer arg (no set_layer dance)
    local pos = pico.get.mouse("window", {'!', anchor='NW'})
    assert(pos.x==123 and pos.y==456)
    local pos = pico.get.mouse("window", '!')
    assert(pos.x==123.5 and pos.y==456.5)
end

-- roundtrip: set(rel) -> get(rel) -> assert equal
print("roundtrip '!'")
do
    pico.set.scene { source={'!', x=0, y=0, w=50, h=50, anchor='NW'} }

    pico.set.mouse({'!', x=25, y=25, anchor='NW'})
    local pos = pico.get.mouse({'!', anchor='NW'})
    assert(pos.x==25 and pos.y==25)
    pico.set.mouse(nil, {'!', x=25, y=25, anchor='NW'})
    local pos = pico.get.mouse(nil, {'!', anchor='NW'})
    assert(pos.x==25 and pos.y==25)
    local pos = pico.get.mouse('!')
    assert(pos.x==25.5 and pos.y==25.5)
end

print("roundtrip '%'")
do
    pico.set.mouse({'%', x=0.5, y=0.5, anchor='NW'})
    local pos = pico.get.mouse({'%', anchor='NW'})
    assert(pos.x>0.49 and pos.x<0.51)
    assert(pos.y>0.49 and pos.y<0.51)
    local pos = pico.get.mouse('%')
    assert(pos.x>0.50 and pos.x<0.52)
    assert(pos.y>0.50 and pos.y<0.52)
end

print("roundtrip '!' zoomed")
do
    pico.set.scene { source={'!', x=20, y=20, w=10, h=10, anchor='NW'} }

    pico.set.mouse({'!', x=25, y=25, anchor='NW'})
    local pos = pico.get.mouse({'!', anchor='NW'})
    assert(pos.x==25 and pos.y==25)
    local pos = pico.get.mouse('!')
    assert(pos.x==25.5 and pos.y==25.5)
end

pico.init(false)
