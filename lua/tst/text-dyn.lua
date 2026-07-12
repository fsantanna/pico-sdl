require 'pico.check'

pico.init(true)

local r = {'!', x=50, y=50, w=0, h=10, anchor='C'}

-- dyn: repeated same content -> cache hit, output stable
do
    pico.output.clear()
    pico.output.draw.text.dyn("/dyn", "hello", r)
    pico.output.draw.text.dyn("/dyn", "hello", r)
    pico.check("text-dyn-01")
end

-- dyn: changed content -> re-raster, new text shown
do
    pico.output.clear()
    pico.output.draw.text.dyn("/dyn", "hello world", r)
    pico.check("text-dyn-02")
end

-- dyn: changed color -> re-raster with new color
do
    pico.output.clear()
    pico.set.pencil { color='red' }
    pico.output.draw.text.dyn("/dyn", "hello world", r)
    pico.check("text-dyn-03")
    pico.set.pencil { color='white' }
end

-- fix: same content twice -> shared cache, output stable
do
    pico.output.clear()
    pico.output.draw.text.fix("fixed", r)
    pico.output.draw.text.fix("fixed", r)
    pico.check("text-dyn-04")
end

pico.init(false)
