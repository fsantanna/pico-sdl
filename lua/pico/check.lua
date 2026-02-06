local pico = require "pico"

local function on (name)
    local v = os.getenv(name)
    return v~=nil and v~=""
end

local INT = on("PICO_CHECK_INT")
local ASR = on("PICO_CHECK_ASR")

function pico.check (name)
    local out = "../../tst/out/" .. name .. ".png"
    local asr = "../../tst/asr/" .. name .. ".png"
    pico.output.screenshot(out)

    print("Testing: " .. name)

    if INT then
        print("-=- press any key -=-")
        pico.input.event('key.dn')
    end

    if ASR then
        local f1 = io.open(out, "rb")
        assert(f1, out)
        local d1 = f1:read("*a")
        f1:close()

        local f2 = io.open(asr, "rb")
        assert(f2, asr)
        local d2 = f2:read("*a")
        f2:close()

        assert(d1 == d2, "FAIL: " .. name)
    end
end

return pico.check
