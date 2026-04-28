local M = require "pico_native"

function M.layer.images (m, up, key, path, t)
    if not t then
        up, key, path, t = m, up, key, path
        m = '!'
    end
    assert(key, "layer.images: key required")
    M.layer.image(m, up, key, path)
    local names = {}
    local mode = t[1]
    assert(mode=='#' or mode=='!', "expected '#' or '!' mode")

    -- Grid form: {w=cols, h=rows, n=count"}
    if mode == '#' then
        assert(t.w and t.h, "mode '#' requires 'w' and 'h'")
        local cols = t.w
        local rows = t.h
        local dim = M.get.image(key)
        local tw = dim.w / cols
        local th = dim.h / rows
        local n = t.n or (cols * rows)
        local i = 0
        for row = 0, rows - 1 do
            for col = 0, cols - 1 do
                i = i + 1
                if i > n then break end
                local sub = string.format("%s-%02d", (t.key or key), i)
                M.layer.sub(up, sub, key,
                    {'!', x=col*tw, y=row*th,
                          w=tw, h=th, anchor='NW'})
                names[#names+1] = sub
            end
            if i >= n then break end
        end

    -- Explicit form: {key=rect, ...}
    else
        for k, crop in pairs(t) do
            if k ~= 1 then
                local sub = key .. "-" .. k
                M.layer.sub(up, sub, key, crop)
                names[#names+1] = sub
            end
        end
    end
    return names
end

return M
