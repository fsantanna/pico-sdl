require 'pico.check'

pico.init(true)
pico.set.window { title="Dim" }

local r = {'%', x=0.5, y=0.5, w=0, h=0}
local N = 1

for i = 0, 100, 25 do
    pico.output.clear()
    r.w = i/100.0
    r.h = i/100.0
    pico.output.draw.rect(r)
    local fmt = string.format("dim-%02d", N)
    N = N + 1
    print(fmt)
    pico.check(fmt)
end

r.w = 0.8
r.h = 0.8
for i = 0, 125, 25 do
    pico.output.clear()

    pico.set.alpha(255)
    pico.set.color.draw('white')
    pico.output.draw.rect(r)

    local rr = {'%', x=0.5, y=0.5, w=i/100.0, h=i/100.0, up=r}
    pico.set.alpha(150)
    pico.set.color.draw('red')
    pico.output.draw.rect(rr)

    local fmt = string.format("dim-%02d", N)
    N = N + 1
    print(fmt)
    pico.check(fmt)
end

pico.init(false)
