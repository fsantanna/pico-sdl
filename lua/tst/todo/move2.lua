require 'pico.check'

pico.init(true)
pico.set.view {
    title  = "Moving Around",
    window = {'!', w=640, h=360},
    world  = {'!', w=64, h=18}
}
math.randomseed(0)

local pos = {x=32, y=9}  -- Center of world (50%, 50%)
for i = 0, 99 do
    pico.set.color.draw(0xFF, 0xFF, 0xFF)
    pico.output.draw.pixel(pos)

    pico.input.delay(10)

--[[
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
]]

    pos.x = pos.x + math.random(0, 2) - 1
    pos.y = pos.y + math.random(0, 2) - 1
end

pico.init(false)
