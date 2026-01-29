require 'pico.check'

pico.init(true)
pico.set.view {
    title  = "Style",
    window = {w=200, h=200},
    world  = {w=50, h=50},
}

-- stroke
do
    print("30/30 - stroke")
    pico.set.style('stroke')
    local r = {'C', x=0.3, y=0.3, w=0.2, h=0.2}
    pico.output.draw.rect(r)
end

-- fill
do
    print("80/80 - fill")
    pico.set.style('fill')
    local r = {'C', x=0.8, y=0.8, w=0.1, h=0.1}
    pico.output.draw.rect(r)
end

pico.check("style-01")

-- error handling
print(pcall(pico.set.style, 'xxx'))
print(pcall(pico.set.style, true))

pico.init(false)
