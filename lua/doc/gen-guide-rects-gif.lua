pico.init(true)
pico.set.window { dim={'!', w=200, h=200}, title="2x Rects" }
pico.set.view   { dim={'!', w=10,  h=10}  }
pico.set.expert(true)

local mx, my = 5, 5              -- red pixel (mouse)
local kx, ky = 4, 4              -- blue pixel (keys)

-- {red_dx, red_dy, blue_dx, blue_dy} per frame
local moves = {
    -- phase 1: mouse fast and linear, keys idle
    { 1, 0,  0, 0},
    { 1, 0,  0, 0},
    { 1, 0,  0, 0},
    { 0, 1,  0, 0},
    { 0, 1,  0, 0},
    { 0, 1,  0, 0},
    -- phase 2: keys move slower, mouse idle
    { 0, 0,  0, 0},
    { 0, 0,  0, 1},
    { 0, 0,  0, 0},
    { 0, 0,  0, 1},
    { 0, 0,  0, 0},
    { 0, 0,  1, 0},
    { 0, 0,  0, 0},
    { 0, 0,  1, 0},
    -- phase 3: both moving, mouse faster
    {-1,-1,  0, 0},
    {-1, 0,  0,-1},
    {-1,-1,  0, 0},
    { 0,-1, -1, 0},
    {-1, 0,  0, 0},
    { 0,-1,  0,-1},
    {-1, 0,  0, 0},
    { 0, 0, -1, 0},
}

os.execute("mkdir -p img/anim")
for step = 0, #moves do
    pico.output.clear()
    pico.set.color.draw 'red'
    pico.output.draw.pixel {'!', x=mx, y=my}
    pico.set.color.draw 'blue'
    pico.output.draw.pixel {'!', x=kx, y=ky}
    pico.output.present()
    pico.output.screenshot(
        string.format("img/anim/frame-%03d.png", step))

    local m = moves[step + 1]
    if m then
        mx, my = mx + m[1], my + m[2]
        kx, ky = kx + m[3], ky + m[4]
    end
end

os.execute("convert -delay 20 -loop 0 "
    .. "img/anim/frame-*.png img/2-rects.gif")
os.execute("rm -rf img/anim")
print("2-rects (gif)")

pico.init(false)
