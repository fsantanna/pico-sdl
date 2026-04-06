require 'pico.check'

pico.init(true)

print("phy (500,500) -> log (50,50)")
do
    pico.set.window({title="Mouse Warp", dim={'!', w=500, h=500}})
    pico.set.view({dim={'!', w=50, h=50}})

    -- log (0,0) -> phy (0,0)
    do
        pico.set.mouse({'!', x=0, y=0, anchor='NW'})
        pico.input.event(100)
        local pos = pico.get.mouse()
        assert(pos.x==0 and pos.y==0)
    end

    -- pct (0.5, 0.5) -> log (25,25) -> phy (250,250)
    do
        pico.set.mouse({'%', x=0.5, y=0.5, anchor='NW'})
        pico.input.event(100)
        local pos = pico.get.mouse('%')
        assert(pos.x==0.5 and pos.y==0.5)
        local pos_raw = pico.get.mouse('!')
        assert(pos_raw.x==25 and pos_raw.y==25)
    end
end

-- Zoom out: src = {-25, -25, 100, 100}
-- centered 100x100 logical
print("zoom out 2x")
do
    pico.set.view({source={'!', x=-25, y=-25, w=100, h=100, anchor='NW'}})

    -- log (25, 25) -> phy (250, 250)
    do
        pico.set.mouse({'!', x=25, y=25, anchor='NW'})
        pico.input.event(100)
        local pos = pico.get.mouse()
        assert(pos.x==25 and pos.y==25)
    end

    -- log (-25, -25) -> phy (0, 0)
    do
        pico.set.mouse({'!', x=-25, y=-25, anchor='NW'})
        pico.input.event(100)
        local pos = pico.get.mouse()
        assert(pos.x==-25 and pos.y==-25)
    end
end

pico.init(false)
