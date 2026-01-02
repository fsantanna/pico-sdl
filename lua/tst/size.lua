local pico = require 'pico'

pico.init(true)

pico.set.title "Size - Fullscreen"

print("no fullscreen - default size")
pico.output.clear()
pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=pico.anchor.C})

pico.input.event('key.dn')

print("fullscreen")
pico.set.view_raw(true, nil, nil, nil, nil, nil)
pico.output.clear()
pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=pico.anchor.C})

pico.input.event('key.dn')

print("no fullscreen - more pixels (128x72 world)")
pico.set.view_raw(false, nil, nil, {w=128, h=72}, nil, nil)
pico.output.clear()
pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.25, h=0.25, anchor=pico.anchor.C})

pico.input.event('key.dn')

print("change window size (640x360)")
pico.set.view_raw(nil, {w=640, h=360}, nil, nil, nil, nil)
pico.output.clear()
pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=pico.anchor.C})

pico.input.event('key.dn')

do
    print("500x500 - phy=log - centered 250x250 rect")
    pico.set.view_raw(nil, {w=500, h=500}, nil, {w=500, h=500}, nil, nil)
    pico.output.clear()
    pico.output.draw.rect_raw({x=125, y=125, w=250, h=250})
    pico.input.event('key.dn')
end

pico.init(false)
