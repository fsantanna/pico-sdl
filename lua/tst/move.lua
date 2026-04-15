require 'pico.check'

-- TODO: math.random() not same as C

pico.init(true)
pico.set.window { title="Moving Around", dim={'!', w=640, h=360} }
pico.set.view { dim={'!', w=64, h=18} }
math.randomseed(0)

local pos = {'!', x=32, y=9}
for i = 0, 99 do
    pico.set.draw { color={r=0xFF, g=0xFF, b=0xFF} }
    pico.output.draw.pixel(pos)

    pico.input.delay(10)

    if i == 0 then
        pico.check("move-01")
    elseif i == 20 then
        pico.check("move-02")
    elseif i == 40 then
        pico.check("move-03")
    elseif i == 60 then
        pico.check("move-04")
    elseif i == 80 then
        pico.check("move-05")
    end

    pos.x = pos.x + math.random(3) - 2
    pos.y = pos.y + math.random(3) - 2
end

pico.init(false)
