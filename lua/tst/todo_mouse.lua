require 'pico'

pico.init(true)

print("phy (500,500) -> log (50,50)")
do
    pico.set.view {
        window = {w=500, h=500},
        world  = {w=50,  h=50},
    }

    -- phy (0,0) -> log (0,0)
    do
        pico.input.warp(0, 3)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==0 and pos.y==0)
    end

    -- phy (250,250) -> log (25,25)
    do
        pico.input.warp(250, 251)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==25 and pos.y==25)
    end

    -- phy (490,490) -> log (49,49)
    do
        pico.input.warp(499, 490)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==49 and pos.y==49)
    end
end

-- Zoom out: src = {-25, -25, 100, 100}
-- centered 100x100 logical
print("zoom out 2x")
do
    pico.set.view(-1, nil, nil, nil, {x=-25, y=-25, w=100, h=100}, nil)

    -- phy (250, 250) -> log (25,25)
    do
        pico.input.warp(250, 253)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==25 and pos.y==25)
    end

    -- phy (0,0) -> log (-25,-25)
    do
        pico.input.warp(0, 0)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==-25 and pos.y==-25)
    end

    -- phy (500,500) -> log (75,75)
    do
        pico.input.warp(495, 499)
        pico.input.pump()
        local pos = pico.get.mouse()
        print(string.format("got (%d,%d)", pos.x, pos.y))
        assert(pos.x==74 and pos.y==74)
    end
end

-- Zoom in: src = {20, 20, 10, 10}
-- centered 10x10 logical
print("zoom in 5x")
do
    pico.set.view(-1, nil, nil, nil, {x=20, y=20, w=10, h=10}, nil)

    -- phy (0,0) -> log (20,20)
    do
        pico.input.warp(1, 2)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==20 and pos.y==20)
    end

    -- phy center (250,250) -> log (25,25)
    do
        pico.input.warp(254, 251)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==25 and pos.y==25)
    end

    -- phy (500,500) -> log (30,30)
    do
        pico.input.warp(497, 498)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==29 and pos.y==29)
    end
end

-- Scroll/Zoom in: src = {25, 25, 25, 25}
-- bottom-right (SE) half
print("scroll/zoom SE")
do
    pico.set.view(-1, nil, nil, nil, {x=25, y=25, w=25, h=25}, nil)

    -- phy (0,0) -> log (25,25)
    do
        pico.input.warp(2, 1)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==25 and pos.y==25)
    end

    -- phy (250,250) -> log (37,37)
    do
        pico.input.warp(253, 250)
        pico.input.pump()
        local pos = pico.get.mouse()
        assert(pos.x==37 and pos.y==37)
    end
end

print("normal PCT")
do
    pico.set.view(-1, nil, nil, nil, {x=0, y=0, w=50, h=50}, nil)

    -- phy (250,250) -> pct (0.5,0.5)
    do
        pico.input.warp(250, 250)
        pico.input.pump()
        local pos = pico.get.mouse_pct({'NW'})
        assert(pos.x==0.5 and pos.y==0.5)
    end

    -- phy (0,0) -> pct (0,0)
    do
        pico.input.warp(0, 0)
        pico.input.pump()
        local pos = pico.get.mouse_pct({'C'})
        assert(pos.x==0.0 and pos.y==0.0)
    end
end

print("zoom PCT")
do
    -- Zoom with src = {20, 20, 10, 10}, but pct should still be 0-1 relative
    -- to full logical world
    pico.set.view(-1, nil, nil, nil, {x=20, y=20, w=10, h=10}, nil)

    -- phy (0,0) -> raw (20,20) -> pct (0.4,0.4)
    do
        pico.input.warp(0, 0)
        pico.input.pump()
        local pos = pico.get.mouse_pct({'NW'})
        assert(pos.x>0.39 and pos.x<0.41)
        assert(pos.y>0.39 and pos.y<0.41)
    end

    -- phy (250,250) -> raw (25,25) -> pct (0.5,0.5)
    do
        pico.input.warp(250, 250)
        pico.input.pump()
        local pos = pico.get.mouse_pct({'C'})
        assert(pos.x>0.49 and pos.x<0.51)
        assert(pos.y>0.49 and pos.y<0.51)
    end
end

pico.init(false)
