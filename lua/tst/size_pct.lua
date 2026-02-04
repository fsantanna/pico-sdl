require 'pico.check'

pico.init(true)
pico.set.window { title="Size - Fullscreen" }

local r = {'%', x=0.5, y=0.5, w=0.5, h=0.5}

-- phy: normal -> double -> half -> normal
print("WINDOW")
do
    print("normal")
    local win = pico.get.window()
    pico.set.window { dim={'!', w=win.dim.w*1, h=win.dim.h*1} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-01")
end

do
    print("double")
    local win = pico.get.window()
    pico.set.window { dim={'!', w=win.dim.w*2, h=win.dim.h*2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-02")
end

do
    print("half")
    local win = pico.get.window()
    pico.set.window { dim={'!', w=win.dim.w*0.25, h=win.dim.h*0.25} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-03")
end

do
    print("normal")
    local win = pico.get.window()
    pico.set.window { dim={'!', w=win.dim.w*2, h=win.dim.h*2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-04")
end

-- phy: normal -> full -> normal
do
    print("full")
    pico.set.window { fullscreen=true }
    pico.output.clear()
    pico.output.draw.rect(r)
    -- pico.check("size_pct-05")
end

do
    print("normal")
    pico.set.window { fullscreen=false }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-06")
end

-- log: normal -> double -> half -> normal
print("WORLD")
do
    print("normal")
    local view = pico.get.view()
    pico.set.view { dim={'!', w=view.dim.w*1, h=view.dim.h*1} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-07")
end

do
    print("double")
    local view = pico.get.view()
    pico.set.view { dim={'!', w=view.dim.w*2, h=view.dim.h*2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-08")
end

do
    print("half")
    local view = pico.get.view()
    pico.set.view { dim={'!', w=view.dim.w*0.25, h=view.dim.h*0.25} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-09")
end

do
    print("normal")
    local view = pico.get.view()
    pico.set.view { dim={'!', w=view.dim.w*2, h=view.dim.h*2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-10")
end

pico.init(false)
