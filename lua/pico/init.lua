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

function M.layer.images (path, t)
    local parent = M.layer.image(nil, path)
    local names = {}
    if t.w and t.h then
        -- Grid form: {w=cols, h=rows, n=count, prefix="..."}
        local prefix = t.prefix or ""
        local cols = t.w
        local rows = t.h
        local dim = M.get.image(path)
        local tw = dim.w / cols
        local th = dim.h / rows
        local n = t.n or (cols * rows)
        local i = 0
        for row = 0, rows - 1 do
            for col = 0, cols - 1 do
                i = i + 1
                if i > n then break end
                local name = prefix .. i
                M.layer.sub(name, parent,
                    {'!', x=col*tw, y=row*th,
                          w=tw, h=th, anchor='NW'})
                names[#names+1] = name
            end
            if i >= n then break end
        end
    else
        -- Explicit form: {name=rect, ...}
        for name, crop in pairs(t) do
            M.layer.sub(name, parent, crop)
            names[#names+1] = name
        end
    end
    return names
end

return M
