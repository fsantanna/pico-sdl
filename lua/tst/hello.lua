-- Simple Hello World example using the new Pico API
local pico = require 'pico'

pico.init(true)
pico.set.title "Hello Pico!"

-- Set colors
pico.set.color.clear(0x00, 0x00, 0x88)  -- Dark blue background
pico.set.color.draw(0xFF, 0xFF, 0x00)   -- Yellow text

-- Clear screen
pico.output.clear()

-- Draw centered text using _pct
pico.output.draw.text_pct({
    x=0.5, y=0.5,      -- Center of screen (50%, 50%)
    w=0, h=0,          -- Auto-size
    anchor=pico.anchor.C  -- Anchored at center
}, "Hello, Pico!")

-- Wait for key press
print("Press any key to exit...")
pico.input.event('key.dn')

pico.init(false)
