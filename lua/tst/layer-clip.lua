require 'pico.check'

-- Aux-clip regression: dst overflow (1-3), src overflow (4),
-- world.scene.src y-overflow (5), child layer src y-overflow (6).

pico.init(true)
pico.set.window { title = "Layer clip" }
pico.set.effect { color = 'white' }
pico.output.clear()

pico.layer.image("world", "img", "../../res/open.png")

-- case 1: dst fully inside world (aux no-op)
do
    local old = pico.set.layer("img")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'},
    }
    pico.set.layer(old)
    pico.output.clear()
    pico.output.present()
    pico.check("layer-clip-01")
end

-- case 2: dst 4-side overflow (aux must window the center)
do
    local old = pico.set.layer("img")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=2.0, h=2.0, anchor='C'},
    }
    pico.set.layer(old)
    pico.output.clear()
    pico.output.present()
    pico.check("layer-clip-02")
end

-- case 3: same layer thrice (50x50) - centered + half off-left + half off-right
do
    local old = pico.set.layer("img")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'},
    }
    pico.set.layer(old)
    pico.output.clear()
    pico.output.draw.layer("img",
        {'%', x=0, y=0.5, w=0.5, h=0.5, anchor='C'})
    pico.output.draw.layer("img",
        {'%', x=1.0, y=0.5, w=0.5, h=0.5, anchor='C'})
    pico.output.present()
    pico.check("layer-clip-03")
end

-- case 4: src overflow - scene.src past layer.dim (aux clips src)
do
    local old = pico.set.layer("img")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'},
        source = {'%', x=0.5, y=0.5, w=2.0, h=2.0, anchor='C'},
    }
    pico.set.layer(old)
    pico.output.clear()
    pico.output.present()
    pico.check("layer-clip-04")
end

-- case 5: world scene.src y-overflow (mimics navigate-02 after 3 Ctrl+UP)
do
    pico.output.clear()
    pico.set.pencil { color = 'black' }
    pico.output.draw.rect(
        {'%', x=0.5, y=0.5, w=0.25, h=0.25, anchor='C'})
    pico.set.scene {
        source = {'%', x=0.5, y=0.2, w=1.0, h=1.0, anchor='C'},
    }
    pico.output.present()
    pico.check("layer-clip-05")
end

-- case 6: child layer src y-overflow (Phase A aux on layer->world blit)
do
    pico.layer.empty("world", "L", {w=48, h=48})
    do
        local old = pico.set.layer("L")
        pico.set.effect { color = 'green' }
        pico.output.clear()
        pico.set.pencil { color = 'black' }
        pico.output.draw.rect(
            {'%', x=0.5, y=0.5, w=0.4, h=0.4, anchor='C'})
        pico.set.scene {
            target = {'%', x=0.5, y=0.5, w=1.0, h=1.0, anchor='C'},
            source = {'%', x=0.5, y=0.2, w=1.0, h=1.0, anchor='C'},
        }
        pico.set.layer(old)
    end
    pico.output.clear()
    -- reset world.scene.src so case 6 tests Phase A aux only
    pico.set.scene {
        source = {'%', x=0.5, y=0.5, w=1.0, h=1.0, anchor='C'},
    }
    pico.output.present()
    pico.check("layer-clip-06")
end

pico.init(false)
