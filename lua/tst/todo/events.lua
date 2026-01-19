local pico = require 'pico'

pico.init(true)

pico.set.title "Events"

while true do
    local e = pico.input.event()
    print('===', e.tag)
    for k,v in pairs(e) do
        print(k,v)
    end
end

pico.init(false)
