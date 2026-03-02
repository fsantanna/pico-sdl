-- anim.lua
-- Two characters walking along overlapping rectangular paths.
-- Run with: pico-lua anim.lua

pico.init(true)
pico.output.clear()
pico.set.expert(true)

-- Split 4x4 sprite sheet into sub-layers "walk-01" to "walk-16":
-- down (1-4), up (5-8), right (9-12), left (13-16)
local frames = pico.layer.images("walk",
    "img/walk.png", {'#', w=4, h=4})

-- Frame indices for each walking direction
local dirs = {
    right = { 9, 10, 11, 12},
    down  = { 1,  2,  3,  4},
    left  = {13, 14, 15, 16},
    up    = { 5,  6,  7,  8},
}

-- Returns the sprite layer name and (x,y) position for a given
-- step along a path. `fstep` cycles the animation frame
-- independently from position advancement.
local function walk(path, steps, step, fstep)
    local leg = path[(step // steps) % #path + 1]
    local t = (step % steps) / steps
    local x = leg.x + (leg.tx - leg.x) * t
    local y = leg.y + (leg.ty - leg.y) * t
    local f = dirs[leg.dir]
    return frames[f[fstep % 4 + 1]], x, y
end

-- Clockwise path around the first rectangle (faster character)
-- Each leg: start (x,y), direction, target (tx,ty)
local cw = {
    {x=0.08, y=0.08, dir='right', tx=0.58, ty=0.08},
    {x=0.58, y=0.08, dir='down',  tx=0.58, ty=0.58},
    {x=0.58, y=0.58, dir='left',  tx=0.08, ty=0.58},
    {x=0.08, y=0.58, dir='up',    tx=0.08, ty=0.08},
}

-- Counter-clockwise path around the second rectangle (slower)
local ccw = {
    {x=0.41, y=0.41, dir='down',  tx=0.41, ty=0.91},
    {x=0.41, y=0.91, dir='right', tx=0.91, ty=0.91},
    {x=0.91, y=0.91, dir='up',    tx=0.91, ty=0.41},
    {x=0.91, y=0.41, dir='left',  tx=0.41, ty=0.41},
}

-- Animation loop: draw both sprites and their path rectangles,
-- then present everything at once (expert mode).
-- The cw character advances at twice the pace (step*2).
pico.set.style 'stroke'
for step=0, math.huge do
    local f1, x1, y1 = walk(cw,  20, step*2, step)
    local f2, x2, y2 = walk(ccw, 20, step,   step)
    pico.output.clear()
    pico.output.draw.rect { '%', x=0.33, y=0.33, w=0.50, h=0.50 }
    pico.output.draw.rect { '%', x=0.66, y=0.66, w=0.50, h=0.50 }
    pico.output.draw.layer(f1, {'%', x=x1, y=y1, w=0.15})
    pico.output.draw.layer(f2, {'%', x=x2, y=y2, w=0.15})
    pico.output.present()
    pico.input.delay(50)
end
