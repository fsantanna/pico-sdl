require 'pico.check'

pico.init(true)
pico.set.layer("window")
pico.set.scene { dim={'!', w=640, h=360} }
pico.set.layer("world")
pico.set.scene { dim={'!', w=64, h=36} }

-- red background on main
pico.set.effect { color={'!', r=0xFF, g=0x00, b=0x00} }
pico.output.clear()

-- overlay layer: blue clear (semi-transparent), white rect
pico.layer.empty { mode='!', key="overlay", dim={'!', w=64, h=36} }
pico.set.layer("overlay")
pico.set.effect { color={'!', r=0x00, g=0x00, b=0xFF, a=0x80} }
pico.output.clear()
pico.set.pencil { color='white' }
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'})

-- composite overlay on main
pico.set.layer("world")
pico.output.draw.layer("overlay", {'%', x=0.5, y=0.5, w=0.75, h=0.75, anchor='C'})
pico.check("clear_alpha-01")

-- default effect.color is transparent: red shows through
pico.output.clear()
pico.layer.empty { up="world", key="defclr", clear=true, dim={'!', w=64, h=36} }
pico.output.clear()
pico.output.present()
pico.check("clear_alpha-02")

-- a transparent clear must WIPE the layer (not blend a no-op): paint a
-- detached layer red, clear it back to transparent, then composite over
-- a plain green world -- the world must stay plain green
pico.set.layer("world")
pico.set.effect { color={'!', r=0x00, g=0xFF, b=0x00} }
pico.output.clear()
pico.output.screenshot("world", "../../tst/out/clear_alpha-transp-ref.png")

pico.layer.empty { mode='!', key="wipe", dim={'!', w=64, h=36} }
pico.set.layer("wipe")
pico.set.effect { color={'!', r=0xFF, g=0x00, b=0x00} }
pico.output.clear()
pico.set.effect { color={'!', r=0x00, g=0x00, b=0x00, a=0x00} }
pico.output.clear()

pico.set.layer("world")
pico.output.draw.layer("wipe")
pico.output.screenshot("world", "../../tst/out/clear_alpha-transp-01.png")

-- self-compare (no golden): world must be byte-identical to the ref
local function slurp (p)
    local f = io.open(p, "rb")
    assert(f, p)
    local d = f:read("*a")
    f:close()
    return d
end
assert(slurp("../../tst/out/clear_alpha-transp-ref.png") == slurp("../../tst/out/clear_alpha-transp-01.png"), "transparent clear must wipe the layer")

pico.init(false)
