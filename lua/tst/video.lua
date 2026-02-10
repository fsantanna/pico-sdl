require 'pico.check'

pico.init(true)
pico.set.window { title="Video" }
pico.set.color.clear({'!', r=0x00, g=0x00, b=0x00})

-- pico_get_video: check Y4M header parsing
do
    print("get_video: dimensions and fps")
    local info = pico.get.video("video.y4m")
    assert(info.dim.w == 10)
    assert(info.dim.h == 10)
    assert(info.fps == 10)
    assert(info.frame == 0)
    assert(info.done == false)
end

-- pico_layer_video: create layer
do
    print("layer_video: create")
    local name = pico.layer.video("vid", "video.y4m")
    assert(name == "vid")
end

-- pico_video_sync: first frame (0)
do
    print("video_sync: frame 0 (top-left)")
    pico.set.layer("vid")
    local ok = pico.video.sync("vid", 0)
    assert(ok == true)
    pico.set.layer(nil)
    pico.output.clear()
    pico.output.draw.layer("vid",
        {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
    pico.check("video-01")
end

-- pico_video_sync: middle frame (25)
do
    print("video_sync: frame 25 (bottom-middle)")
    local ok = pico.video.sync("vid", 25)
    assert(ok == true)
    pico.output.clear()
    pico.output.draw.layer("vid",
        {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
    pico.check("video-02")
end

-- pico_video_sync: last frame (49)
do
    print("video_sync: frame 49 (top-right)")
    local ok = pico.video.sync("vid", 49)
    assert(ok == true)
    pico.output.clear()
    pico.output.draw.layer("vid",
        {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
    pico.check("video-03")
end

-- pico_video_sync: backward seek
do
    print("video_sync: backward seek to frame 10")
    local ok = pico.video.sync("vid", 10)
    assert(ok == true)
    pico.output.clear()
    pico.output.draw.layer("vid",
        {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
    pico.check("video-04")
end

-- pico_video_sync: past EOF
do
    print("video_sync: past EOF returns 0")
    local ok = pico.video.sync("vid", 100)
    assert(ok == false)
end

-- pico_get_video: check state after sync
do
    print("get_video: state after sync")
    local info = pico.get.video("vid")
    assert(info.done == true)
end

-- pico_output_draw_video: all-in-one
do
    print("draw_video: first call")
    pico.output.clear()
    local ok = pico.output.draw.video("video.y4m",
        {'%', x=0.5, y=0.5, w=1, h=1, anchor='C'})
    assert(ok == true)
    pico.check("video-05")
end

pico.init(false)
