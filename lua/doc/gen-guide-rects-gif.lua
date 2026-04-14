pico.init(true)

pico.set {
    expert = true,
    window = { dim={'!', w=200, h=200}, title="2x Rects" },
    view   = { dim={'!', w=10,  h=10}  },
    alpha  = 0xCC,
}

local k = {'!', x=4, y=4}
local m = {'!', x=5, y=5}

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
    pico.set.draw { color='red' }
    pico.output.draw.pixel(m)
    pico.set.draw { color='blue' }
    pico.output.draw.pixel(k)
    pico.output.present()
    pico.output.screenshot(string.format("img/anim/frame-%03d.png", step))

    local mv = moves[step + 1]
    if mv then
        m.x, m.y = m.x + mv[1], m.y + mv[2]
        k.x, k.y = k.x + mv[3], k.y + mv[4]
    end
end

os.execute("convert -delay 20 -loop 0 "
    .. "img/anim/frame-*.png img/2-rects.gif")
os.execute("rm -rf img/anim")
print("2-rects (gif)")

pico.init(false)
