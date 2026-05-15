require 'pico.check'

--os.execute("mkdir -p /tmp/cross")

pico.init(true)
pico.set.window { title = "Hello World!" }

for i=0, 99 do
    --pico.output.screenshot("window", string.format("/tmp/cross/%02d.png", i))

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

--os.execute("convert -delay 5 -loop 0 /tmp/cross/*.png ../../res/cross.gif")
--os.execute("rm -rf /tmp/cross")

pico.init(false)
