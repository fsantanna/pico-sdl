require 'pico.check'

pico.init(true)
local phy = {'!', w=100, h=100}
local log = {'!', w=10, h=10}
pico.set.window { title="Pixmap", dim=phy }
pico.set.view { dim=log }

do
    -- .x.
    -- xxx
    -- .x.
    local pixmap = {
        {
            { r=0x00, g=0x00, b=0x00 },             -- default a=0xFF
            { r=0xFF, g=0xFF, b=0x00, a=0xFF },
            { r=0x00, g=0x00, b=0x00, a=0xFF },
        },
        {
            { r=0xFF, g=0x00, b=0x00, a=0xFF },
            { r=0x00, g=0xFF, b=0x00, a=0xFF },
            { r=0x00, g=0x00, b=0xFF, a=0xFF },
        },
        {
            pico.color.alpha('black', 0),
            { r=0x00, g=0xFF, b=0xFF, a=0xFF },
            pico.color.alpha('black', 0),
        },
    }

    print("centered 3x3 on black - 1dir/1baixo")
    local r = {'!', x=5-1, y=5-1, w=0, h=0, anchor='NW'}
    pico.output.clear()
    pico.output.draw.pixmap("buf1", pixmap, r)
    pico.check("pixmap-01")
end

do
    local pixmap = {
        {
            { r=0x00, g=0x00, b=0x00, a=0xFF },
            { r=0xFF, g=0xFF, b=0x00, a=0xFF },
            { r=0x00, g=0x00, b=0x00, a=0xFF },
            { r=0xFF, g=0x00, b=0x00, a=0xFF },
            { r=0x00, g=0xFF, b=0x00, a=0xFF },
            { r=0x00, g=0x00, b=0xFF, a=0xFF },
            pico.color.alpha('black', 0),
            { r=0x00, g=0xFF, b=0xFF, a=0xFF },
            pico.color.alpha('black', 0),
        }
    }

    print("bottomright 9x1 on white")
    pico.set.show { color='white' }
    pico.output.clear()
    local r = {'!', x=1, y=9, w=0, h=0, anchor='NW'}
    pico.output.draw.pixmap("buf2", pixmap, r)
    pico.check("pixmap-02")
end

pico.init(false)
