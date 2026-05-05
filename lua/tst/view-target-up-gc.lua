require 'pico.check'

pico.init(true)
pico.set.window { title="View Target Up GC" }

-- regression: stored Pico_Rel_Rect.up was a Lua userdata pointer;
-- after the call returned, GC freed it, leaving a dangling pointer.
local up = {'%', x=0.5, y=0.5, w=1, h=1}
pico.set.view {
    target = {'%', x=0.5, y=0.5, w=0.5, h=0.5, up=up}
}
collectgarbage("collect")
collectgarbage("collect")

-- present walks the up chain -> SEGV here without the fix
pico.output.clear()
pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.5, h=0.5})

pico.init(false)
