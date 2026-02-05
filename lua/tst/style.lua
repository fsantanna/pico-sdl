require 'pico.check'

pico.init(true)
pico.set.window { title="Style", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

local rect = {'!', x=5, y=5, w=25, h=18, anc='NW'}
local oval = {'!', x=34, y=5, w=25, h=18, anc='NW'}
local t1   = {'!', x=5,  y=26, anc='NW'}
local t2   = {'!', x=5,  y=43, anc='NW'}
local t3   = {'!', x=30, y=43, anc='NW'}
local poly = {
    {'!', x=34, y=26, anc='NW'},
    {'!', x=59, y=30, anc='NW'},
    {'!', x=50, y=43, anc='NW'},
    {'!', x=38, y=43, anc='NW'},
}

do
    print("default fill")
    pico.output.clear()
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-01")
end

do
    print("stroke")
    pico.output.clear()
    pico.set.style('stroke')
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-02")
end

do
    print("back to fill")
    pico.output.clear()
    pico.set.style('fill')
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-03")
end

pico.init(false)
