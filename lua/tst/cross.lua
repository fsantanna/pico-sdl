require 'pico.check'

pico.init(true)
pico.set.window { dim={'!', w=160, h=160} }
pico.set.view { dim={'!', w=16, h=16} }
pico.output.clear()

for i = 0, 15 do
    pico.output.draw.pixel({'!', x=i, y=i, anchor='NW'})
    pico.output.draw.pixel({'!', x=15-i, y=i, anchor='NW'})
    pico.input.delay(10)
    if i == 0 then
        pico.check("cross-01")
    elseif i == 8 then
        pico.check("cross-02")
    elseif i == 15 then
        pico.check("cross-03")
    end
end

pico.init(false)
