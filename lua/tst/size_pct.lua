require 'pico.check'

pico.init(true)
pico.set.title("Size - Fullscreen")

local r = {'C', x=0.5, y=0.5, w=0.5, h=0.5}

-- phy: normal -> double -> half -> normal
print("WINDOW")
do
    print("normal")
    pico.set.view { window={'%', x=1, y=1} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-01")
end

do
    print("double")
    pico.set.view { window={'%', x=2, y=2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-02")
end

do
    print("half")
    pico.set.view { window={'%', x=0.25, y=0.25} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-03")
end

do
    print("normal")
    pico.set.view { window={'%', x=2, y=2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-04")
end

-- phy: normal -> full -> normal
do
    print("full")
    pico.set.view { fullscreen=true }
    pico.output.clear()
    pico.output.draw.rect(r)
    -- pico.check("size_pct-05")
end

do
    print("normal")
    pico.set.view { fullscreen=false }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-06")
end

-- log: normal -> double -> half -> normal
print("WORLD")
do
    print("normal")
    pico.set.view { world={'%', x=1, y=1} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-07")
end

do
    print("double")
    pico.set.view { world={'%', x=2, y=2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-08")
end

do
    print("half")
    pico.set.view { world={'%', x=0.25, y=0.25} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-09")
end

do
    print("normal")
    pico.set.view { world={'%', x=2, y=2} }
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("size_pct-10")
end

pico.init(false)
