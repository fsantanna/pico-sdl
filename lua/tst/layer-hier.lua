require 'pico.check'

-- Scene-graph smoke test: children attached to root.
-- Relies on auto-composite (present walk) to blit onto root.
-- No explicit pico.output.draw.layer call.

pico.init(true)

pico.set.effect { color='black' }
pico.output.clear()

pico.layer.empty { up="world", key="L", dim={'!', w=8, h=8} }

local old = pico.set.layer("L")
pico.set.scene({
    target = {'%', x=0.3, y=0.3, w=0.4, h=0.4, anchor='C'},
    clear = false,
})
pico.set.effect { color='white' }
pico.output.clear()
pico.set.pencil { color='red' }
pico.output.draw.rect(
    {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
)

pico.set.layer(old)
pico.output.present()
pico.check("layer-hier-01")

pico.layer.image { up="world", key="img", path="../../res/open.png",
    target={'%', x=0.7, y=0.7, w=0.4, h=0.4, anchor='C'} }
pico.output.present()
pico.check("layer-hier-02")

pico.layer.pixmap { up="world", key="buf", pixels={
    {{r=255, g=0,   b=0,   a=255}, {r=0, g=255, b=0,   a=255}},
    {{r=0,   g=0,   b=255, a=255}, {r=255, g=255, b=0, a=255}},
}, target={'%', x=0.7, y=0.3, w=0.4, h=0.4, anchor='C'} }
pico.output.present()
pico.check("layer-hier-03")

pico.set.pencil { color='green' }
pico.layer.text { up="world", key="txt", dim={'!', h=10}, text="hello",
    target={'%', x=0.3, y=0.7, w=0.4, h=0.4, anchor='C'} }
pico.output.present()
pico.check("layer-hier-04")

---------------------------------------------------------------------------

-- transparent overlay with yellow background
pico.layer.empty { up="world", key="over", clear=false, dim={'!', w=500, h=500} }
old = pico.set.layer("over")
pico.set.effect { color={r=0xFF, g=0xFF, b=0x00}, alpha=0x80 }
pico.output.clear()
pico.set.scene({
    target = {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
})
pico.set.layer(old)
pico.output.present()
pico.check("layer-hier-05")

-- sub-layers: crop blue and green from "buf", parented under "over"
pico.layer.sub { up="over", key="blue", sup="buf",
    crop={'!', x=0, y=1, w=1, h=1, anchor='NW'},
    target={'%', x=0.20, y=0.9, w=0.10, h=0.10, anchor='C'} }

pico.layer.sub { up="over", key="green", sup="buf",
    crop={'!', x=1, y=0, w=1, h=1, anchor='NW'},
    target={'%', x=0.40, y=0.9, w=0.10, h=0.10, anchor='C'} }

pico.output.present()
pico.check("layer-hier-06")

pico.init(false)
