local M = require "pico_native"

local function apply_set (target, values)
    for k, v in pairs(values) do
        local field = target[k]
        if type(field) == "function" then
            field(v)
        elseif type(field) == "table" then
            apply_set(field, v)
        else
            error("pico.set: unknown key '" .. k .. "'")
        end
    end
end

setmetatable(M.set, {
    __call = function (self, t)
        apply_set(self, t)
    end
})

return M
