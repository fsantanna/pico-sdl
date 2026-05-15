require 'pico.check'

--[[
os.execute("mkdir -p /tmp/cross")
]]

pico.init(true)
pico.set.window { title = "Hello World!" }

for i=0, 99 do
--[[
    if i%5 == 0 then
        pico.output.screenshot(string.format("/tmp/cross/%02d.png", i//5))
    end
]]
    pico.output.draw.pixel { '!', x=i,    y=i }
    pico.output.draw.pixel { '!', x=99-i, y=i }
    pico.input.delay(5)
    if i == 0 then
        pico.check("cross-01")
    elseif i == 49 then
        pico.check("cross-02")
    elseif i == 99 then
        pico.check("cross-03")
    end
end

--[[
pico.output.screenshot("/tmp/cross/20.png")
os.execute("convert -delay 10 -loop 0 /tmp/cross/*.png ../../res/cross.gif")
os.execute("rm -rf /tmp/cross")
]]

pico.init(false)
