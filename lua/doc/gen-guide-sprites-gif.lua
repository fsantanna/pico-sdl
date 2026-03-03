-- gen-guide-sprites-gif.lua
-- Generates the 2-sprites animation GIF for the guide
-- Run with: pico-lua gen-guide-sprites-gif.lua

pico.init(true)
pico.output.clear()
pico.set.expert(true)

local frames = pico.layer.images("walk",
    "img/walk.png", {'#', w=4, h=4})
local dirs = {
    right = { 9, 10, 11, 12},
    down  = { 1,  2,  3,  4},
    left  = {13, 14, 15, 16},
    up    = { 5,  6,  7,  8},
}
local function walk(path, steps, step, fstep)
    local leg = path[(step // steps) % #path + 1]
    local t = (step % steps) / steps
    local x = leg.x + (leg.tx - leg.x) * t
    local y = leg.y + (leg.ty - leg.y) * t
    local f = dirs[leg.dir]
    return frames[f[(fstep // 4) % 4 + 1]], x, y
end
local cw = {
    {x=0.08, y=0.08, dir='right', tx=0.58, ty=0.08},
    {x=0.58, y=0.08, dir='down',  tx=0.58, ty=0.58},
    {x=0.58, y=0.58, dir='left',  tx=0.08, ty=0.58},
    {x=0.08, y=0.58, dir='up',    tx=0.08, ty=0.08},
}
local ccw = {
    {x=0.41, y=0.41, dir='down',  tx=0.41, ty=0.91},
    {x=0.41, y=0.91, dir='right', tx=0.91, ty=0.91},
    {x=0.91, y=0.91, dir='up',    tx=0.91, ty=0.41},
    {x=0.91, y=0.41, dir='left',  tx=0.41, ty=0.41},
}
os.execute("mkdir -p img/anim")
pico.set.style 'stroke'
for step=0, 79 do
    local f1, x1, y1 = walk(cw,  20, step*2, step)
    local f2, x2, y2 = walk(ccw, 20, step,   step)
    pico.output.clear()
    pico.output.draw.rect { '%', x=0.33, y=0.33, w=0.50, h=0.50 }
    pico.output.draw.rect { '%', x=0.66, y=0.66, w=0.50, h=0.50 }
    pico.output.draw.layer(f1, {'%', x=x1, y=y1, w=0.15})
    pico.output.draw.layer(f2, {'%', x=x2, y=y2, w=0.15})
    pico.output.present()
    --pico.input.delay(50)
    pico.output.screenshot(string.format(
        "img/anim/frame-%03d.png", step))
end
os.execute("convert -delay 10 -loop 0 "
    .. "img/anim/frame-*.png img/2-sprites.gif")
os.execute("rm -rf img/anim")
print("2-sprites (gif)")

pico.init(false)
