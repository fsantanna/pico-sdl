require 'pico.check'

-- Scene-graph smoke test: children attached to root.
-- Relies on auto-composite (present walk) to blit onto root.
-- No explicit pico.output.draw.layer call.

pico.init(true)

pico.set.show { color='black' }
pico.output.clear()

pico.layer.empty("root", "L", {w=8, h=8})

pico.set.layer("L")
pico.set.view({
    target = {'%', x=0.3, y=0.3, w=0.4, h=0.4, anchor='C'},
})
pico.set.show { color='white' }
pico.output.clear()
pico.set.draw { color='red' }
pico.output.draw.rect(
    {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
)

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-01")

pico.layer.image("root", "img", "open.png")
pico.set.layer("img")
pico.set.view({
    target = {'%', x=0.7, y=0.7, w=0.4, h=0.4, anchor='C'},
})

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-02")

pico.layer.buffer("root", "buf", {w=2, h=2}, {
    {{r=255, g=0,   b=0,   a=255}, {r=0, g=255, b=0,   a=255}},
    {{r=0,   g=0,   b=255, a=255}, {r=255, g=255, b=0, a=255}},
})
pico.set.layer("buf")
pico.set.view({
    target = {'%', x=0.7, y=0.3, w=0.4, h=0.4, anchor='C'},
})

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-03")

pico.set.draw { color='green' }
pico.layer.text("root", "txt", 10, "hello")
pico.set.layer("txt")
pico.set.view({
    target = {'%', x=0.3, y=0.7, w=0.4, h=0.4, anchor='C'},
})

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-04")

---------------------------------------------------------------------------

-- transparent overlay with yellow background
pico.layer.empty("root", "over", {w=500, h=500})
pico.set.layer("over")
pico.set.show { color={r=0xFF, g=0xFF, b=0x00} }
pico.output.clear()
pico.set.view({
    target = {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
    alpha = 0x80,
})
pico.set.layer("root")

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-05")

-- sub-layers: crop blue and green from "buf", parented under "over"
pico.layer.sub("over", "blue", "buf",
    {'!', x=0, y=1, w=1, h=1, anchor='NW'})
pico.set.layer("blue")
pico.set.view({
    target = {'%', x=0.20, y=0.9, w=0.10, h=0.10, anchor='C'},
})

pico.layer.sub("over", "green", "buf",
    {'!', x=1, y=0, w=1, h=1, anchor='NW'})
pico.set.layer("green")
pico.set.view({
    target = {'%', x=0.40, y=0.9, w=0.10, h=0.10, anchor='C'},
})

pico.set.layer("root")
pico.output.present()
pico.check("layer-hier-06")

pico.init(false)
