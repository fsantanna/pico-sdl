require 'pico.check'

-- Scene-graph smoke test: children attached to root.
-- Relies on auto-composite (present walk) to blit onto root.
-- No explicit pico.output.draw.layer call.

pico.init(true)

pico.set.effect { color='black' }
pico.output.clear()

pico.layer.empty("root", "L", {w=8, h=8})

pico.set.scene("L", {
    target = {'%', x=0.3, y=0.3, w=0.4, h=0.4, anchor='C'},
    keep = true,
})
pico.set.effect("L", { color='white' })
pico.output.clear("L")
pico.set.pencil("L", { color='red' })
pico.output.draw.rect("L",
    {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}
)

pico.output.present()
pico.check("layer-hier-01")

pico.layer.image("root", "img", "../../res/open.png")
pico.set.scene("img", {
    target = {'%', x=0.7, y=0.7, w=0.4, h=0.4, anchor='C'},
})

pico.output.present()
pico.check("layer-hier-02")

pico.layer.pixmap("root", "buf", {
    {{r=255, g=0,   b=0,   a=255}, {r=0, g=255, b=0,   a=255}},
    {{r=0,   g=0,   b=255, a=255}, {r=255, g=255, b=0, a=255}},
})
pico.set.scene("buf", {
    target = {'%', x=0.7, y=0.3, w=0.4, h=0.4, anchor='C'},
})

pico.output.present()
pico.check("layer-hier-03")

pico.set.pencil { color='green' }
pico.layer.text("root", "txt", 10, "hello")
pico.set.scene("txt", {
    target = {'%', x=0.3, y=0.7, w=0.4, h=0.4, anchor='C'},
})

pico.output.present()
pico.check("layer-hier-04")

---------------------------------------------------------------------------

-- transparent overlay with yellow background
pico.layer.empty("root", "over", {w=500, h=500})
pico.set.effect("over", { color={r=0xFF, g=0xFF, b=0x00}, alpha=0x80 })
pico.output.clear("over")
pico.set.scene("over", {
    target = {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'},
})

pico.output.present()
pico.check("layer-hier-05")

-- sub-layers: crop blue and green from "buf", parented under "over"
pico.layer.sub("over", "blue", "buf",
    {'!', x=0, y=1, w=1, h=1, anchor='NW'})
pico.set.scene("blue", {
    target = {'%', x=0.20, y=0.9, w=0.10, h=0.10, anchor='C'},
})

pico.layer.sub("over", "green", "buf",
    {'!', x=1, y=0, w=1, h=1, anchor='NW'})
pico.set.scene("green", {
    target = {'%', x=0.40, y=0.9, w=0.10, h=0.10, anchor='C'},
})

pico.output.present()
pico.check("layer-hier-06")

pico.init(false)
