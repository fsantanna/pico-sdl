-- A clip set on the WINDOW (parent) layer must scissor the world -> window
-- composite, not just direct draws. Currently the composite ignores it, so
-- the world overwrites the whole window (full red instead of a red box).
--
-- Two blocks pin the same behaviour in both modes:
--   01 PLAIN  -- green comes from the window effect (clear) colour, since a
--                plain present clears the window every frame.
--   02 EXPERT -- green is drawn onto the window layer and persists, since an
--                expert present does not clear the window.

require 'pico.check'

pico.init(true)
pico.set.window { title="Window Clip" }
pico.set.pencil { style='fill' }

local full = {'%', x=0.5, y=0.5, w=1.0, h=1.0, anchor='C'}
local box  = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'}

--------------------------------------------------------------------------------
-- PLAIN: green backdrop via the window effect (clear) colour
--------------------------------------------------------------------------------

-- GREEN backdrop: the plain present clears the window to this each frame
pico.set.layer("window")
pico.set.effect { color='green' }

-- clip the WINDOW to a centred box; the composite should honour it
pico.set.scene { clip=box }

-- WORLD: full red -> should reach the window only inside the box
pico.set.layer("world")
pico.set.pencil { color='red' }
pico.output.draw.rect(full)

pico.check("window-clip-01")
-- want: GREEN field, RED centred box
-- now:  full RED (window clip ignored by the composite)

--------------------------------------------------------------------------------
-- EXPERT: green backdrop drawn onto the window layer (persists)
--------------------------------------------------------------------------------

pico.set.expert(true, 0)

-- GREEN background painted on the WINDOW layer (full)
pico.set.layer("window")
pico.set.scene { clip=full }
pico.set.pencil { color='green' }
pico.output.draw.rect(full)

-- clip the WINDOW to a centred box; the composite should honour it
pico.set.scene { clip=box }

-- WORLD: full red -> should reach the window only inside the box
pico.set.layer("world")
pico.output.clear()
pico.set.pencil { color='red' }
pico.output.draw.rect(full)

pico.output.present()
pico.check("window-clip-02")
-- want: GREEN field, RED centred box
-- now:  full RED (window clip ignored by the composite)

pico.init(false)
