require 'pico.check'

pico.init(true)

-- pico.get.text: empty string, abs mode -> w stays 0 (no fill)
do
    local dim = pico.get.text({'!', w=nil, h=10}, "")
    assert(dim.w==0 and dim.h==10)
end
-- pico.get.text: empty string, pct mode -> w stays 0 (no fill)
do
    local dim = pico.get.text({'%', w=nil, h=0.1}, "")
    assert(dim.w==0 and dim.h*100//1==10)
end

-- pico.output.draw.text: empty string, abs mode -> no crash, screen stays clear
do
    pico.output.clear()
    local r = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text.fix("", r)
    pico.check("text_empty-01")
end
-- pico.output.draw.text: empty string, pct mode -> no crash, screen stays clear
do
    pico.output.clear()
    local r = {'%', x=0.5, y=0.5, h=0.2}
    pico.output.draw.text.fix("", r)
    pico.check("text_empty-02")
end

-- draw non-empty text, then empty text over it -> original remains
do
    pico.output.clear()
    local r1 = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text.fix("HELLO", r1)
    local r2 = {'!', x=10, y=10, w=0, h=10, anchor='NW'}
    pico.output.draw.text.fix("", r2)
    pico.check("text_empty-03")
end

pico.init(false)
