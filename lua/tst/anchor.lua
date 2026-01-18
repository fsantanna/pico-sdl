local pico = require 'pico'
require 'pico.check'

pico.init(true)
pico.set.title("Anchoring...")

local phy = {w=200, h=200}
local log = {w=10, h=10}
pico.set.view(-1, phy, nil, log, nil, nil)

-- PIXELS
do
    print("centered pixel - 1dir/1baixo")
    local p = {'C', x=0.5, y=0.50}
    pico.output.clear()
    pico.output.draw.pixel(p)
    pico.check("anchor-01")
end

do
    print("centered pixel - 1dir/1baixo")
    local p = {'NW', x=0.5, y=0.5}
    pico.output.clear()
    pico.output.draw.pixel(p)
    pico.check("anchor-02")
end

do
    print("centered pixel - 1esq/1cima")
    local p = {'SE', x=0.5, y=0.5}
    pico.output.clear()
    pico.output.draw.pixel(p)
    pico.check("anchor-03")
end

-- RECTS
do
    print("centered rect - exact")
    local r = {'C', x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-04")
end

do
    print("topleft centered - 1lin/1col")
    local r = {'NW', x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-05")
end

do
    print("bottomright centered - 1lin/1col")
    local r = {'SE', x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-06")
end

do
    print("rightmiddle centered - 1col")
    local r = {'E', x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-07")
end

do
    print("anchor 25%25% 20%20% - 1lin/1col")
    local r = {{x=0.2,y=0.2}, x=0.2, y=0.2, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-08")
end

-- RECTS out of [0,100]
do
    print("anchor -25%-25% centered - touching border")
    local r = {{x=-0.25,y=-0.25}, x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-09")
end

do
    print("anchor 125%125% centered - touching border")
    local r = {{x=1.25,y=1.25}, x=0.5, y=0.5, w=0.4, h=0.4}
    pico.output.clear()
    pico.output.draw.rect(r)
    pico.check("anchor-10")
end

pico.init(false)
