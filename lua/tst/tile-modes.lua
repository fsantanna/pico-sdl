require 'pico.check'

-- §7 (dim, tile) joint resolution: assert texture dim (D) and stored
-- tile (T). Mirrors tst/tile-modes.c. Behavioral (no image).

pico.init(true)

-- known parent frame for '%' cases: world = 100x100
pico.set.dim {'!', w=100, h=100}

local function check (key, dw, dh, tw, th)
    local old = pico.set.layer(key)
    local v = pico.get.scene()
    print(string.format("%s: dim=%dx%d tile=%dx%d", key, v.dim.w, v.dim.h, v.tile.w, v.tile.h))
    assert(v.dim.w == dw and v.dim.h == dh, "dim mismatch")
    assert(v.tile.w == tw and v.tile.h == th, "tile mismatch")
    pico.set.layer(old)
end

-- G1: tile='#' derives T = floor(D / t)
print("G1: dim='!' + tile='#'")
pico.layer.empty { up="world", key="g1", clear=true, dim={'!', w=100, h=100}, tile={'#', w=10, h=10} }
check("g1", 100, 100, 10, 10)

-- G2: tile='%' derives T = floor(t% * D)  ('%' is 0.0-1.0)
print("G2: dim='!' + tile='%'")
pico.layer.empty { up="world", key="g2", clear=true, dim={'!', w=100, h=100}, tile={'%', w=0.1, h=0.1} }
check("g2", 100, 100, 10, 10)

-- G3: dim='%' of parent + tile='!'
print("G3: dim='%' + tile='!'")
pico.layer.empty { up="world", key="g3", clear=true, dim={'%', w=0.5, h=0.5}, tile={'!', w=5, h=5} }
check("g3", 50, 50, 5, 5)

-- G4: non-divisible -> floor + clip
print("G4: floor (dim%tile != 0)")
pico.layer.empty { up="world", key="g4", clear=true, dim={'!', w=100, h=100}, tile={'#', w=3, h=3} }
check("g4", 100, 100, 33, 33)

-- G5: canonical tiled layer, dim='#' cells + bare tile (defaults '!')
print("G5: dim='#' + tile (default '!')")
pico.layer.empty { up="world", key="g5", clear=true, dim={'#', w=20, h=15}, tile={'!', w=16, h=16} }
check("g5", 320, 240, 16, 16)

-- G6: set.scene joint (dim='#' + tile='!')
print("G6: set.scene joint dim+tile")
pico.layer.empty { up="world", key="g6", clear=true, dim={'!', w=10, h=10} }
pico.set.layer("g6")
pico.set.scene { dim={'#', w=8, h=8}, tile={'!', w=4, h=4} }
pico.set.layer("world")
check("g6", 32, 32, 4, 4)

-- G7: set.scene tile-only, resolves against current scene.dim (32x32)
print("G7: set.scene tile-only vs current dim")
pico.set.layer("g6")
pico.set.scene { tile={'#', w=2, h=2} }
pico.set.layer("world")
check("g6", 32, 32, 16, 16)

print("ALL OK")
pico.init(false)
