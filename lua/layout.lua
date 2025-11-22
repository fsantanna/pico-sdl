local pico = require "pico"

local M = {}

local function unit (lay)
    local L, C = #lay, #lay[1]
    local ret = {}
    for l=1, L do
        for c=1, C do
            local nxt = #ret + 1
            if lay[l][c] == nxt then
                local r = {x=c,y=l,w=nil,h=nil}
                for cx=c, C do
                    if lay[l][cx] ~= nxt then
                        break
                    end
                    r.w = (cx - c) + 1
                end
                for lx=l, L do
                    if lay[lx][c] ~= nxt then
                        break
                    end
                    r.h = (lx - l) + 1
                end
                ret[#ret+1] = r
            end
        end
    end
    return ret
end

local function mult (lay, ref, rs)
    local ret = {}
    local px,py = ref.w/#lay[1], ref.h/#lay
    for i,r in ipairs(rs) do
        local w = r.w * px
        local h = r.h * py
        local x = ref.x + ((r.x-1) * px)
        local y = ref.y + ((r.y-1) * py)
        ret[#ret+1] = {x=x,y=y,w=w,h=h}
    end
    return ret
end

function M.build (lay, ref)
    local us = unit(lay)
    local ms = mult(lay, ref, us)
    return ms
end

function M.draw (rs, clr)
    pico.set.style 'stroke'
    pico.set.anchor.draw('left', 'top')
    for _,r in ipairs(rs) do
        pico.set.color.draw(clr)
        pico.output.draw.rect(r)
    end
end

return M
