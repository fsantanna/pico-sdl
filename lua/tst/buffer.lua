package.cpath = package.cpath .. ";../?.so"

local pico = require 'pico'

pico.init(true)

pico.set.title "Buffer"
pico.set.size({x=100,y=100}, {x=10,y=10})

-- .x.
-- xxx
-- .x.
local buffer = {
    {
        { r=0x00, g=0x00, b=0x00, a=0x00 },
        { r=0xFF, g=0xFF, b=0x00, a=0xFF },
        { r=0x00, g=0x00, b=0x00, a=0x00 },
    },
    {
        { r=0xFF, g=0x00, b=0x00, a=0xFF },
        { r=0x00, g=0xFF, b=0x00, a=0xFF },
        { r=0x00, g=0x00, b=0xFF, a=0xFF },
    },
    {
        { r=0x00, g=0x00, b=0x00, a=0x00 },
        { r=0x00, g=0xFF, b=0xFF, a=0xFF },
        { r=0x00, g=0x00, b=0x00, a=0x00 },
    },
}

do
    print "centered 3x3 on black - 1dir/1baixo"
    local p1 = pico.pos(50, 50)
    pico.output.clear()
    pico.output.draw.buffer(p1, buffer)
    pico.input.event('key.dn');
    --_pico_check("buf3w3h_center_black")
end

--[=[

    {
        puts("bottomright 9x1 on white");
        pico_set_color_clear((Pico_Color){0xFF, 0xFF, 0xFF, 0xFF});
        pico_output_clear();
        Pico_Pos p2 = pico_pos((Pico_Pct){100,100});
        pico_set_anchor_draw((Pico_Anchor){PICO_RIGHT, PICO_BOTTOM});
        pico_output_draw_buffer(p2, buffer, (Pico_Dim){9,1});
        _pico_check("buf9w1h_rightbottom_white");
    }

]=]

pico.init(false);
