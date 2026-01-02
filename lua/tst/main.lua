local pico = require 'pico'

pico.init(true)

-- Get initial view
local view = pico.get.view()
assert(view.window.x==640 and view.window.y==360)
assert(view.world.x==64  and view.world.y==36)

print "shows dark screen"
print "waits any key press"
pico.input.event('key.up')

-- TITLE
print 'changes window title to "Testing..."'
pico.set.title "Testing..."
pico.input.delay(2000)

-- SOUND
print "plays sound"
pico.output.sound "start.wav"
pico.input.delay(2000)

-- CLEAR
pico.set.color.clear(0xFF, 0xFF, 0xFF)
pico.output.clear()

print "shows white screen"
pico.input.delay(2000)

-- DRAW_IMAGE (using _pct for percentage-based positioning)
pico.output.draw.image_pct(
    {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
    "open.png"
)

print "shows centered image"
pico.input.delay(2000)

-- DRAW_PIXEL/RECT/OVAL (using _pct)
pico.set.color.clear(pico.color.black)
pico.set.color.draw(pico.color.white)
pico.output.clear()

-- Draw pixel at center
pico.output.draw.pixel_pct({x=0.5, y=0.5, anchor=pico.anchor.C})

-- Draw rect at 75%, 25%
pico.output.draw.rect_pct({x=0.75, y=0.25, w=0.15, h=0.14, anchor=pico.anchor.C})

-- Draw oval at 25%, 75%
pico.output.draw.oval_pct({x=0.25, y=0.75, w=0.08, h=0.28, anchor=pico.anchor.C})

print "shows oval -> pixel -> rect"
pico.input.delay(2000)

-- DRAW_TEXT
pico.output.draw.text_pct(
    {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
    "Hello!"
)

print "shows centered \"Hello!\" (on top of shapes)"
pico.input.delay(2000)
pico.output.clear()

-- NOTE: pico_output_write/writeln no longer exist in the new API
-- Skipping the write/cursor test section

-- MOUSE
do
    print "waits mouse click"
    local e2 = pico.input.event 'mouse.button.dn'
    print "shows pixel over mouse"
    -- Use raw pixel coordinates from event
    pico.output.draw.pixel_raw { x=e2.x, y=e2.y }
    pico.input.delay(2000)
end

-- EVENT
pico.input.delay(2000)
print "waits more 2 seconds"
local e3 = pico.input.event(2000)

-- GRID=0
pico.set.grid(false)
print "disables grid"
pico.input.delay(2000)

-- EXPERT
pico.output.clear()
pico.set.expert(true)
pico.output.draw.text_pct(
    {x=0.15, y=0.15, w=0, h=0, anchor=pico.anchor.NW},
    "expert"
)
print("shows expert")
pico.output.present()
pico.input.delay(2000)
pico.set.expert(false)
pico.output.clear()

-- ZOOM TEST (using pico.set.view_raw)
print("shows lower-left X, center rect, center/up-right line")
print("increases zoom by shrinking world")

local world_w = view.world.x
local world_h = view.world.y

for i=1, 20 do
    world_w = world_w - 1
    world_h = world_h - 1

    -- Set world dimensions using view_raw
    pico.set.view_raw(
        nil,  -- keep fullscreen as is
        nil,  -- keep window as is
        nil,  -- keep window_target as is
        {w=world_w, h=world_h},  -- change world dimensions
        nil,  -- keep world_source as is
        nil   -- keep world_clip as is
    )

    pico.output.clear()
    pico.set.color.draw { r=0xFF,g=0xFF,b=0xFF }

    -- Draw center rect using _pct
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.15, h=0.28, anchor=pico.anchor.C})

    pico.set.color.draw { r=0xFF,g=0x00,b=0x00 }

    -- Draw X at bottom-left using _pct
    pico.output.draw.text_pct({x=0.25, y=0.75, w=0, h=0, anchor=pico.anchor.C}, "X")

    -- Draw line from center to top-right using _pct
    pico.output.draw.line_pct(
        {x=0.5, y=0.5, anchor=pico.anchor.C},
        {x=1.0, y=0.0, anchor=pico.anchor.C}
    )

    pico.input.delay(250)
end

print("decreases zoom")
for i=1, 20 do
    world_w = world_w + 1
    world_h = world_h + 1

    pico.set.view_raw(nil, nil, nil, {w=world_w, h=world_h}, nil, nil)

    pico.output.clear()
    pico.set.color.draw { r=0xFF,g=0xFF,b=0xFF }
    pico.output.draw.rect_pct({x=0.5, y=0.5, w=0.15, h=0.28, anchor=pico.anchor.C})
    pico.set.color.draw { r=0xFF,g=0x00,b=0x00 }
    pico.output.draw.text_pct({x=0.25, y=0.75, w=0, h=0, anchor=pico.anchor.C}, "X")
    pico.output.draw.line_pct(
        {x=0.5, y=0.5, anchor=pico.anchor.C},
        {x=1.0, y=0.0, anchor=pico.anchor.C}
    )
    pico.input.delay(250)
end

pico.set.color.draw { r=0xFF, g=0xFF, b=0xFF }

-- PAN TEST (using world_source in pico.set.view_raw)
print("scrolls right/down by changing world source")
for i=0, 19 do
    -- Pan by adjusting world source rectangle
    pico.set.view_raw(
        nil, nil, nil, nil,
        {x=10-i, y=10-i, w=world_w, h=world_h},  -- world_source
        nil
    )
    pico.output.clear()
    pico.output.draw.text_pct(
        {x=0.5, y=0.5, w=0, h=0, anchor=pico.anchor.C},
        "Uma frase bem grande..."
    )
    pico.input.delay(250)
end

pico.init(false)
