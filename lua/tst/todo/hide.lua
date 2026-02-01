local pico = require 'pico'

pico.init(true)

pico.set.window { title="Show - Hide" }

pico.input.event 'key.dn'

pico.set.show(false)
print("hidden")
pico.input.delay(1000)

pico.set.show(true)
print("shown")
pico.input.event 'key.dn'

pico.set.show(false)
print("hidden")
pico.input.delay(1000)

pico.init(false)
