local pico = require 'pico'

pico.init(true)

pico.set.title "Buffer"
pico.set.size.window({x=100,y=100}, {x=10,y=10})

-- .x.
-- xxx
-- .x.
local b1 = {
    {
        { r=0x00, g=0x00, b=0x00, a=0x00 },
        { r=0xFF, g=0xFF, b=0x00 },
        { r=0x00, g=0x00, b=0x00 },
    },
    {
        { r=0xFF, g=0x00, b=0x00, a=0xFF },
        { r=0x00, g=0xFF, b=0x00, a=0xFF },
        { r=0x00, g=0x00, b=0xFF, a=0xFF },
    },
    {
        { r=0x00, g=0x00, b=0x00 },
        { r=0x00, g=0xFF, b=0xFF },
        { r=0x00, g=0x00, b=0x00 },
    },
}

do
    print "centered 3x3 on black - 1dir/1baixo"
    local p1 = pico.pos(50, 50)
    pico.output.clear()
    pico.output.draw.buffer(p1, b1)
    pico.input.event('key.dn');
    --_pico_check("buf3w3h_center_black")
end

local b2 = {
    { { r=0x00, g=0x00, b=0x00, a=0x00 } },
    { { r=0xFF, g=0xFF, b=0x00, a=0xFF } },
    { { r=0x00, g=0x00, b=0x00, a=0x00 } },
    { { r=0xFF, g=0x00, b=0x00, a=0xFF } },
    { { r=0x00, g=0xFF, b=0x00, a=0xFF } },
    { { r=0x00, g=0x00, b=0xFF, a=0xFF } },
    { { r=0x00, g=0x00, b=0x00, a=0x00 } },
    { { r=0x00, g=0xFF, b=0xFF, a=0xFF } },
    { { r=0x00, g=0x00, b=0x00, a=0x00 } },
}

do
    print "bottomright 9x1 on white"
    local p2 = pico.pos(100, 100)
    pico.set.color.clear { r=0xFF, g=0xFF, b=0xFF, a=0xFF }
    pico.output.clear()
    pico.set.anchor.draw { x='right', y='bottom' }
    pico.output.draw.buffer(p2, b2)
    pico.input.event('key.dn');
    --_pico_check("buf9w1h_rightbottom_white");
end

pico.init(false);
