pico.init(true)

pico.set.expert(true, 20)
pico.set.style 'stroke'

-- 4x4 sprite sheet -> subs "walk-01" to "walk-16"

local frames = pico.layer.images (
    "walk", "img/walk.png", {'#', w=4, h=4})

local dirs = {
    down  = { 1,  2,  3,  4},   -- walk-01 -> walk-04
    up    = { 5,  6,  7,  8},
    right = { 9, 10, 11, 12},
    left  = {13, 14, 15, 16},
}

-- start (x,y), direction, target (tx,ty)
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

-- Returns the sprite layer name and (x,y) position for a given
-- step along a path. `fstep` cycles the animation frame
-- independently from position advancement.

local steps = 40
local function walk (side, step, fstep)
    local path = paths[side]
    local leg = path[(step // steps) % #path + 1]
    local t = (step % steps) / steps
    local x = leg.x + (leg.tx - leg.x) * t
    local y = leg.y + (leg.ty - leg.y) * t
    local f = dirs[leg.dir]
    return frames[f[(fstep // 4) % 4 + 1]], x, y
end

-- Animation loop: draw both sprites and their path rectangles,
-- then present everything at once (expert mode).
-- The cw character advances at twice the pace (step*2).

local f1, x1, y1 = walk('clock',   0, 0)
local f2, x2, y2 = walk('counter', 0, 0)

local step = 0

while true do
    pico.output.clear()
    pico.output.draw.rect { '%', x=0.3, y=0.3, w=0.4, h=0.4 }
    pico.output.draw.rect { '%', x=0.6, y=0.6, w=0.4, h=0.4 }
    pico.output.draw.layer(f1, {'%', x=x1, y=y1, w=0.15})
    pico.output.draw.layer(f2, {'%', x=x2, y=y2, w=0.15})
    pico.output.present()

    local e = pico.input.event('quit')
    if e then
        break
    end

    step = step + 1
    f1, x1, y1 = walk('clock',   step*2, step)
    f2, x2, y2 = walk('counter', step,   step)
end

pico.init(false)
