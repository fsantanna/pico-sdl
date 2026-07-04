local M = require "pico_native"

function M.layer.images (t)
    assert(t.key, "layer.images: key required")
    M.layer.image(t)
    local names = {}
    local mode = t.sheet[1]
    assert(mode=='#' or mode=='!', "expected '#' or '!' mode")

    -- Grid form: {'#', w=cols, h=rows, [n=count], [key=prefix]}
    if mode == '#' then
        assert(t.sheet.w and t.sheet.h, "mode '#' requires 'w' and 'h'")
        local cols = t.sheet.w
        local rows = t.sheet.h
        local dim = M.get.image(t.key)
        local tw = dim.w / cols
        local th = dim.h / rows
        local n = t.sheet.n or (cols * rows)
        local i = 0
        for row = 0, rows - 1 do
            for col = 0, cols - 1 do
                i = i + 1
                if i > n then break end
                local sub = string.format("%s-%02d", (t.sheet.key or t.key), i)
                M.layer.sub { up=t.up, key=sub, sup=t.key,
                    crop={'!', x=col*tw, y=row*th,
                               w=tw, h=th, anchor='NW'} }
                names[#names+1] = sub
            end
            if i >= n then break end
        end

    -- Explicit form: {'!', [name1=Rect, name2=Rect, ...]}
    else
        for k, crop in pairs(t.sheet) do
            if k ~= 1 then
                local sub = t.key .. "-" .. k
                M.layer.sub { up=t.up, key=sub, sup=t.key, crop=crop }
                names[#names+1] = sub
            end
        end
    end
    return names
end

return M
