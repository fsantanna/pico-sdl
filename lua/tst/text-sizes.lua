require 'pico.check'

pico.init(true)
pico.set.window { title="Text Sizes" }
pico.set.layer("window")
pico.set.scene { dim={'!', w=500, h=500} }
pico.set.layer("world")
pico.set.scene { dim={'!', w=500, h=500} }

-- 12 lines, small to big, stacked top-to-bottom
do
    pico.output.clear()
    pico.set.pencil { font="../../res/DejaVuSans.ttf" }
    pico.set.pencil { color='white' }

    local y = 5
    for i = 1, 12 do
        local h = 5 * i
        local r = {'!', x=5, y=y, w=0, h=h, anchor='NW'}
        pico.output.draw.text("Hello World!", r)
        y = y + h + 4
    end
    pico.check("text-sizes-01")
end

-- repro of intro.atm flicker: typewriter, redrawn (cleared) each frame
-- as the prefix grows. r1 '!' and r2 '%' at the SAME integer height
-- (22px = 0.044*500). 22 still jitters: ptsize lands the glyph surface
-- at H0 != 22, so box_h != H0 and the auto-width re-quantises per char
-- (20 happens to match H0 -> stays stable). Stacked so the boxes share
-- a border (r1 bottom = r2 top = y211).
do
    local msg = "For a long time, the Pingus have lived hapilly."
    local r1 = {'!', x=20, y=200, w=0, h=22, anchor='W'}
    local r2 = {'%', x=0.04, y=0.444, w=0, h=0.044, anchor='W'}
    local n = #msg
    for i = 1, n do
        local s = msg:sub(1, i)
        pico.output.clear()

        -- texts (white)
        pico.set.pencil { color='white', style='fill' }
        pico.output.draw.text(s, r1)
        pico.output.draw.text(s, r2)

        -- enclosing boxes (red stroke), sized to the measured text
        pico.set.pencil { color='red', style='stroke' }
        local m1 = {'!', w=0, h=22}
        pico.get.text(m1, s)
        pico.output.draw.rect({'!', x=20, y=200, w=m1.w, h=22, anchor='W'})
        local m2 = {'%', w=0, h=0.044}
        pico.get.text(m2, s)
        pico.output.draw.rect({'%', x=0.04, y=0.444, w=m2.w, h=0.044, anchor='W'})

        -- capture start / middle / end of the reveal
        if i == 1    then pico.check("text-sizes-02") end
        if i == n//2 then pico.check("text-sizes-03") end
        if i == n    then pico.check("text-sizes-04") end

        pico.input.delay(50)
    end
end

pico.init(false)
