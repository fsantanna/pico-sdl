require 'pico.check'

pico.init(true)
pico.set.title("Size - Fullscreen")

local all = pico.get.view()
local phy = all.window
local log = all.world
local r = {'!', x=50-25, y=50-25, w=50, h=50, anc='NW'}

-- phy: normal -> double -> half -> normal
print("WINDOW")
do
    print("normal")
    pico.set.view { window=phy }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-01")
end

do
    print("double")
    local dim = {'!', w=phy.w*2, h=phy.h*2}
    pico.set.view { window=dim }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-02")
end

do
    print("half")
    local dim = {'!', w=phy.w/2, h=phy.h/2}
    pico.set.view { window=dim }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-03")
end

do
    print("normal")
    pico.set.view { window=phy }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-04")
end

-- phy: normal -> full -> normal
do
    print("full")
    pico.set.view { fullscreen=true }
    pico.output.clear()
    pico.output.draw.rect(r)
    -- pico.check("size_raw-05")
end

do
    print("normal")
    pico.set.view { fullscreen=false }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-06")
end

-- log: normal -> double -> half -> normal
print("WORLD")
do
    print("normal")
    pico.set.view { world=log }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-07")
end

do
    print("double")
    local dim = {'!', w=log.w*2, h=log.h*2}
    pico.set.view { world=dim }
    pico.output.clear()
    local r = {'!', x=log.w-25, y=log.h-25, w=50, h=50, anc='NW'}
    pico.output.draw.rect(r)
    pico.check("size_raw-08")
end

do
    print("half")
    local dim = {'!', w=log.w/2, h=log.h/2}
    pico.set.view { world=dim }
    pico.output.clear()
    local r = {'!', x=log.w/4-25, y=log.w/4-25, w=50, h=50, anc='NW'}
    pico.output.draw.rect(r)
    pico.check("size_raw-09")
end

do
    print("normal")
    pico.set.view { world=log }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_raw-10")
end

pico.init(false)
