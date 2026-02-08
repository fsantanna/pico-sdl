require 'pico.check'

pico.init(true)
local phy = {'!', w=100, h=100}
local log = {'!', w=10, h=10}
pico.set.window { title="Buffer", dim=phy }
pico.set.view { dim=log }

do
    -- .x.
    -- xxx
    -- .x.
    local buffer = {
        {
            { r=0x00, g=0x00, b=0x00, a=0xFF },
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
    local r = {'%', x=0.5, y=0.5, w=0, h=0}
    pico.output.clear()
    pico.output.draw.buffer("buf1", buffer, r)
    pico.check("buffer-01")
end

do
    local buffer = {
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
    pico.set.color.clear('white')
    pico.output.clear()
    local r = {'%', x=1, y=1, w=0, h=0, anchor='SE'}
    pico.output.draw.buffer("buf2", buffer, r)
    pico.check("buffer-02")
end

pico.init(false)
