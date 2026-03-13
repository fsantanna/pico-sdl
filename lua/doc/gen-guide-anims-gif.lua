-- gen-guide-anims-gif.lua
-- Generates the 2-anims animation GIF for the guide
-- Run with: pico-lua gen-guide-anims-gif.lua

pico.init(true)
pico.output.clear()
pico.set.expert(true, 20)

local frames = pico.layer.images("walk",
    "img/walk.png", {'#', w=4, h=4})
local dirs = {
    down  = { 1,  2,  3,  4},
    up    = { 5,  6,  7,  8},
    right = { 9, 10, 11, 12},
    left  = {13, 14, 15, 16},
}

-- TODO: must be 20
-- * not 40 like anims.lua
-- * two turns to complete the animation
local steps = 20

local paths = {
    clock = {
        {x=0.1, y=0.1, dir='right', tx=0.5, ty=0.1},
        {x=0.5, y=0.1, dir='down',  tx=0.5, ty=0.5},
        {x=0.5, y=0.5, dir='left',  tx=0.1, ty=0.5},
        {x=0.1, y=0.5, dir='up',    tx=0.1, ty=0.1},
    },
    counter = {
        {x=0.4, y=0.4, dir='down',  tx=0.4, ty=0.8},
        {x=0.4, y=0.8, dir='right', tx=0.8, ty=0.8},
        {x=0.8, y=0.8, dir='up',    tx=0.8, ty=0.4},
        {x=0.8, y=0.4, dir='left',  tx=0.4, ty=0.4},
    },
}
local function walk(side, step, fstep)
    local path = paths[side]
    local leg = path[(step // steps) % #path + 1]
    local t = (step % steps) / steps
    local x = leg.x + (leg.tx - leg.x) * t
    local y = leg.y + (leg.ty - leg.y) * t
    local f = dirs[leg.dir]
    return frames[f[(fstep // 4) % 4 + 1]], x, y
end
os.execute("mkdir -p img/anims/")
pico.set.style 'stroke'
local f1, x1, y1 = walk('clock',   0, 0)
local f2, x2, y2 = walk('counter', 0, 0)
for step=0, 79 do
    pico.output.clear()
    pico.output.draw.rect { '%', x=0.3, y=0.3, w=0.4, h=0.4 }
    pico.output.draw.rect { '%', x=0.6, y=0.6, w=0.4, h=0.4 }
    pico.output.draw.layer(f1, {'%', x=x1, y=y1, w=0.15})
    pico.output.draw.layer(f2, {'%', x=x2, y=y2, w=0.15})
    pico.output.present()
    pico.output.screenshot(string.format(
        "img/anims/frame-%03d.png", step))
    f1, x1, y1 = walk('clock',   (step+1)*2, step+1)
    f2, x2, y2 = walk('counter', step+1,     step+1)
end
os.execute("convert -delay 10 -loop 0 "
    .. "img/anims/frame-*.png img/2-anims.gif")
os.execute("rm -rf img/anims")
print("2-anims (gif)")

pico.init(false)
