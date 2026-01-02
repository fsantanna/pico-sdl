local pico = require 'pico'

pico.init(true)

pico.set.title "Control"

while true do
    pico.output.draw.text_pct(
        {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
        "Hello!"
    )
    pico.input.event()
    pico.output.clear()
end

pico.init(false)
