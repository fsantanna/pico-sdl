local pico = require "pico"
local layout = require "pico.layout"

local function EQ (v1, v2)
    if v1 == v2 then
        return true
    end

    local t1 = type(v1)
    local t2 = type(v2)
    if t1 ~= t2 then
        return false
    end

    local mt1 = getmetatable(v1)
    local mt2 = getmetatable(v2)
    if mt1 ~= mt2 then
        return false
    end

    if t1 == 'table' then
        for k1,x1 in pairs(v1) do
            local x2 = v2[k1]
            if not EQ(x1,x2) then
                return false
            end
        end
        for k2,x2 in pairs(v2) do
            local x1 = v1[k2]
            if not EQ(x2,x1) then
                return false
            end
        end
        return true
    end

    return false
end

do
    print "Test 1..."
    local lay = {
        {1, 2}
    }
--[[
    local us = unit(lay)
    assert(EQ(us, {
        {x=1,y=1,w=1,h=1},
        {x=2,y=1,w=1,h=1},
    }))

    local m1 = mult(lay, {x=0,y=0,w=2,h=1}, us)
    assert(EQ(m1, {
        {x=0,y=0,w=1,h=1},
        {x=1,y=0,w=1,h=1},
    }))
]]

    local m3 = layout.build(lay, {x=100,y=200,w=10,h=10})
    assert(EQ(m3, {
        {x=100,y=200,w=5,h=10},
        {x=105,y=200,w=5,h=10},
    }))
end

do
    print "Test 2..."
    local lay = {
        {1,1,2},
        {1,1,2},
        {3,3,4},
        {5,6,4},
    }
    local rs = layout.build(lay, {x=0,y=0,w=30,h=40})
    assert(EQ(rs, {
        {x=00,y=00,w=20,h=20},
        {x=20,y=00,w=10,h=20},
        {x=00,y=20,w=20,h=10},
        {x=20,y=20,w=10,h=20},
        {x=00,y=30,w=10,h=10},
        {x=10,y=30,w=10,h=10}
    }))
end

do
    print "Test 3..."
    local lay = {
        {1,1,2},
        {1,1,2},
        {3,3,4},
        {5,6,7},
    }
    local rs = layout.build(lay, {x=0,y=0,w=30,h=40})
    assert(EQ(rs, {
        {x=00,y=00,w=20,h=20},
        {x=20,y=00,w=10,h=20},
        {x=00,y=20,w=20,h=10},
        {x=20,y=20,w=10,h=10},
        {x=00,y=30,w=10,h=10},
        {x=10,y=30,w=10,h=10},
        {x=20,y=30,w=10,h=10},
    }))
end

do
    print "Grid 1..."
    local lay = {
        {1,1,2},
        {1,1,2},
        {3,3,4},
        {5,6,7},
    }
    local rs = layout.build(lay, {x=0,y=0,w=100,h=100})

    pico.init(true)
    pico.set.dim.window(500,500)
    pico.set.dim.world(100,100)
    layout.draw(rs, pico.color.white)
    for i=1, 7 do
        local pt = pico.pos({x=50,y=50}, rs[i], {x='left',y='top'})
--require'atmos.x'.print(pt)
        pico.set.color.draw(pico.color.red)
        pico.output.draw.pixel(pt)
    end
    pico.input.delay(1000)
    pico.init(false)
end
