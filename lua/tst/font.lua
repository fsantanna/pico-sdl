require 'pico.check'

pico.init(true)

do
    local r = {x=10, y=10, w=0, h=10}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-01")
end

do
    local r = {'C', x=0.5, y=0.5, w=0, h=0.2}
    pico.output.draw.text("hg - gh", r)
    pico.check("font-02")
end

pico.init(false)
