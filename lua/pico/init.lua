local M = require "pico_native"

local function set (self, t)
    for k, v in pairs(t) do
        local field = self[k]
        if type(field) == "function" then
            field(v)
        elseif type(field) == "table" then
            set(field, v)
        else
            error("pico.set: unknown key '" .. k .. "'")
        end
    end
end

setmetatable(M.set, {
    __call = function (self, t)
        set(self, t)
    end
})

function M.layer.images (name, path, t)
    assert(name, "layer.images: name required")
    M.layer.image(name, path)
    local names = {}
    local mode = t[1]
    assert(mode=='#' or mode=='!', "expected '#' or '!' mode")

    -- Grid form: {w=cols, h=rows, n=count"}
    if mode == '#' then
        assert(t.w and t.h, "mode '#' requires 'w' and 'h'")
        local cols = t.w
        local rows = t.h
        local dim = M.get.image(name)
        local tw = dim.w / cols
        local th = dim.h / rows
        local n = t.n or (cols * rows)
        local i = 0
        for row = 0, rows - 1 do
            for col = 0, cols - 1 do
                i = i + 1
                if i > n then break end
                local sub = string.format("%s-%02d", name, i)
                M.layer.sub(sub, name,
                    {'!', x=col*tw, y=row*th,
                          w=tw, h=th, anchor='NW'})
                names[#names+1] = sub
            end
            if i >= n then break end
        end

    -- Explicit form: {name=rect, ...}
    else
        for key, crop in pairs(t) do
            if key ~= 1 then
                local sub = name .. "-" .. key
                M.layer.sub(sub, name, crop)
                names[#names+1] = sub
            end
        end
    end
    return names
end

return M
