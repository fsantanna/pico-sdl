require 'pico.check'

-- push/pop scope test (mirrors tst/stack.c minus the realm_get
-- assertions which Lua doesn't expose).
--
-- stack-01: inside push, a red 50%-centered box under world
-- stack-02: after pop, world is empty (black)
--
-- To prove pop actually does something:
--   - comment out pico.pop() → box stays → stack-02 still has red → fail
--   - comment out pico.push() → pop trips depth assert → abort

pico.init(true)

pico.set.effect { color='black' }
pico.output.clear()

pico.push()

pico.layer.empty("world", "box", false, {'!', w=16, h=16})
do
    local old = pico.set.layer("box")
    pico.set.scene {
        target = {'%', x=0.5, y=0.5, w=0.5, h=0.5, anchor='C'},
    }
    pico.set.effect { color='red' }
    pico.output.clear()
    pico.set.layer(old)
end
pico.output.present()
pico.check("stack-01")

pico.pop()

pico.set.effect { color='black' }
pico.output.clear()
pico.output.present()
pico.check("stack-02")

pico.init(false)
