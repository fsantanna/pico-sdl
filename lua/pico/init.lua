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

return M
