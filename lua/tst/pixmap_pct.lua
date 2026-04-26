require 'pico.check'

pico.init(true)
local phy = {'!', w=100, h=100}
local log = {'!', w=10, h=10}
pico.set.window { title="Pixmap", dim=phy }
pico.set.view { dim=log }

-- .x.
-- xxx
-- .x.
local pixmap = {
    {
        'black',                                -- string
        0xFFFF00,                               -- hex number
        { r=0x00, g=0x00, b=0x00, a=0xFF },     -- implicit absolute
    },
    {
        {'!', r=0xFF, g=0x00, b=0x00},          -- '!' table
        'green',                                -- string
        {'%', r=0,    g=0,    b=1   },          -- '%' table
    },
    {
        pico.color.alpha('black', 0),
        0x00FFFF,                               -- hex number
        pico.color.alpha('black', 0),
    },
}

do
    print("centered 3x3 on black - 1dir/1baixo")
    local r = {'%', x=0.5, y=0.5}
    pico.output.clear()
    pico.output.draw.pixmap("buf1", pixmap, r)
    pico.check("pixmap-01")
end

do
    local pixmap = {
        {
            'black',
            0xFFFF00,
            { r=0x00, g=0x00, b=0x00 },             -- default a=0xFF
            {'!', r=0xFF, g=0x00, b=0x00},
            'green',
            {'%', r=0, g=0, b=1},
            pico.color.alpha('black', 0),
            0x00FFFF,
            pico.color.alpha('black', 0),
        }
    }

    print("bottomright 9x1 on white")
    pico.set.show { color='white' }
    pico.output.clear()
    local r = {'%', x=1, y=1, anchor='SE'}
    pico.output.draw.pixmap("buf2", pixmap, r)
    pico.check("pixmap-02")
end

do
    print("aspect: h=0.6 only on 3x3 -> 6x6 cells")
    pico.set.show { color='black' }
    pico.output.clear()
    local r = {'%', x=0.1, y=0.1, h=0.6, anchor='NW'}
    pico.output.draw.pixmap("buf3", pixmap, r)
    pico.check("pixmap-03")
end

do
    print("distort: w=0.6, h=0.3 -> x-stretched")
    pico.set.show { color='white' }
    pico.output.clear()
    local r = {'%', x=0.1, y=0.1, w=0.6, h=0.3, anchor='NW'}
    pico.output.draw.pixmap("buf4", pixmap, r)
    pico.check("pixmap-04")
end

pico.init(false)
