-- todo_video.lua — simple video player
--
-- Controls:
--   +/-    : increase/decrease speed (0.5x steps)
--   space  : pause/resume
--   left   : seek backward 1 second
--   right  : seek forward 1 second
--   click  : seek to position on seek bar
--   escape : quit
--
-- Supports negative speed (reverse playback).

require 'pico'

local VIDEO_PATH = "video.y4m"
local BAR_H = 10

pico.init(true)
pico.set.expert(true)

local info = pico.get.video(VIDEO_PATH)
local total = info.fps * 5

-- Window: video width scaled up, plus bar
local scale = 30
local win_w = info.dim.w * scale
local win_h = info.dim.h * scale + BAR_H * 2
pico.set.window("Video Player", -1,
    {'!', w=win_w, h=win_h})
pico.set.view{
    world = {'!', w=win_w, h=win_h},
}

pico.layer.video("vid", VIDEO_PATH)

local speed = 1.0
local paused = false
local frame_f = 0.0
local frame = 0
local last_tick = pico.get.ticks()

while true do
    -- Timing
    local now = pico.get.ticks()
    local dt = now - last_tick
    last_tick = now

    -- Advance frame by speed
    if not paused then
        frame_f = frame_f + speed * info.fps * dt / 1000.0
        -- Clamp
        if frame_f < 0 then
            frame_f = 0
        elseif frame_f >= total then
            frame_f = total - 1
        end
        frame = math.floor(frame_f)
    end

    -- Sync video
    pico.video.sync("vid", frame)

    -- Draw video
    pico.set.color.clear({'!', r=0x20, g=0x20, b=0x20})
    pico.output.clear()
    pico.output.draw.layer("vid",
        {'!', x=win_w/2, y=(win_h - BAR_H*2)/2,
              w=win_w, h=win_h - BAR_H*2, anchor='C'})

    -- Draw seek bar background
    pico.set.color.draw({'!', r=0x40, g=0x40, b=0x40})
    pico.output.draw.rect(
        {'!', x=0, y=win_h - BAR_H*2,
              w=win_w, h=BAR_H, anchor='NW'})

    -- Draw seek bar progress
    local pct = frame / (total - 1)
    local bar_w = math.floor(pct * win_w)
    pico.set.color.draw({'!', r=0x00, g=0xAA, b=0xFF})
    pico.output.draw.rect(
        {'!', x=0, y=win_h - BAR_H*2,
              w=bar_w, h=BAR_H, anchor='NW'})

    -- Draw info text
    local label = string.format("frame %d/%d  speed %.1fx%s",
        frame, total - 1, speed,
        paused and "  [PAUSED]" or "")
    pico.set.color.draw('white')
    pico.output.draw.text(label,
        {'!', x=2, y=win_h - BAR_H,
              w=0, h=BAR_H, anchor='NW'})

    pico.output.present()

    -- Events
    local timeout = 16
    while timeout > 0 do
        local before = pico.get.ticks()
        local e = pico.input.event(timeout)
        if not e then
            break
        end

        if e.tag == "quit" then
            goto done
        elseif e.tag == "key.dn" then
            local k = e.key
            if k == "Escape" then
                goto done
            elseif k == "+" or k == "="
                    or k == "Keypad +" then
                speed = speed + 0.5
            elseif k == "-"
                    or k == "Keypad -" then
                speed = speed - 0.5
            elseif k == "Space" then
                paused = not paused
            elseif k == "Left" then
                frame_f = frame_f - info.fps
                if frame_f < 0 then
                    frame_f = 0
                end
                frame = math.floor(frame_f)
            elseif k == "Right" then
                frame_f = frame_f + info.fps
                if frame_f >= total then
                    frame_f = total - 1
                end
                frame = math.floor(frame_f)
            end
        elseif e.tag == "mouse.button.dn" then
            -- Click on seek bar area
            if e.y >= win_h - BAR_H * 2 then
                local click_pct = e.x / win_w
                if click_pct < 0 then
                    click_pct = 0
                elseif click_pct > 1 then
                    click_pct = 1
                end
                frame_f = click_pct * (total - 1)
                frame = math.floor(frame_f)
            end
        end

        timeout = timeout - (pico.get.ticks() - before)
    end
end

::done::
pico.init(false)
