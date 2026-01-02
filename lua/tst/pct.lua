local pico = require 'pico'

pico.init(true)

pico.set.title "Pct-To-Pos"

-- Test 1: centered rect using _pct
do
    print("centered rect")
    pico.output.clear()
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=pico.anchor.C})
    pico.input.event('key.dn')
end

-- Test 2: rect at 30% with nested rect at 50% of outer
do
    print("rect at 30%")
    pico.output.clear()

    -- Draw white rect at 30%
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=0.3, y=0.3, w=0.5, h=0.5, anchor=pico.anchor.C})

    -- Draw red rect at 50% (centered relative to world, not outer rect)
    -- In the new API, the nesting would require manual calculation
    -- or using the `up` pointer in Pico_Rect_Pct (not yet implemented in Lua)
    -- For now, just draw at 30% position with smaller size
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect_pct({x=0.3, y=0.3, w=0.25, h=0.25, anchor=pico.anchor.C})

    pico.input.event('key.dn')
end

-- Test 3: rect at 50% anchored by bottom-right
do
    print("rect at 50% anchored by bottom-right")
    pico.output.clear()

    -- White rect anchored bottom-right
    local br_anchor = {x=pico.anchor.RIGHT, y=pico.anchor.BOTTOM}
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=br_anchor})

    -- Red rect anchored top-left at 0% (of the white rect in original code)
    local tl_anchor = {x=pico.anchor.LEFT, y=pico.anchor.TOP}
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect_pct({x=0.25, y=0.25, w=0.25, h=0.25, anchor=tl_anchor})

    pico.input.event('key.dn')
end

-- Test 4: rect at -10/-10 top-left (partial rect visible)
do
    print("rect at -10/-10 top-left (4x7 rect on top)")
    pico.output.clear()

    local tl_anchor = {x=pico.anchor.LEFT, y=pico.anchor.TOP}
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=-0.1, y=-0.1, w=0.15, h=0.28, anchor=tl_anchor})

    pico.input.event('key.dn')
end

-- Test 5: rect at 110/110 bottom-right (symmetric to previous)
do
    print("rect at 110/110 bottom-right (symmetric to previous)")
    pico.output.clear()

    local br_anchor = {x=pico.anchor.RIGHT, y=pico.anchor.BOTTOM}
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=1.1, y=1.1, w=0.15, h=0.28, anchor=br_anchor})

    pico.input.event('key.dn')
end

-- Test 6: rect at 50% with red rect at -10% (extending beyond white rect)
do
    print("rect at 50% with inner rect at -10%")
    pico.output.clear()

    local c_anchor = {x=pico.anchor.CENTER, y=pico.anchor.MIDDLE}
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=c_anchor})

    local tl_anchor = {x=pico.anchor.LEFT, y=pico.anchor.TOP}
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect_pct({x=0.2, y=0.2, w=0.25, h=0.25, anchor=tl_anchor})

    pico.input.event('key.dn')
end

-- Test 7: rect at 50% with red rect at 110% (symmetric to previous)
do
    print("rect at 50% with inner rect at 110% (symmetric to previous)")
    pico.output.clear()

    local c_anchor = {x=pico.anchor.CENTER, y=pico.anchor.MIDDLE}
    pico.set.color.draw(0xFF,0xFF,0xFF)
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.5, h=0.5, anchor=c_anchor})

    local br_anchor = {x=pico.anchor.RIGHT, y=pico.anchor.BOTTOM}
    pico.set.color.draw(0xFF,0x00,0x00)
    pico.output.draw.rect_pct({x=0.8, y=0.8, w=0.25, h=0.25, anchor=br_anchor})

    pico.input.event('key.dn')
end

pico.init(false)
