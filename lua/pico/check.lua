local pico = require "pico"

function pico.check (name)
    io.write("Testing " .. name .. "...")
    local out = "../../tst/out/" .. name .. ".png"
    local asr = "../../tst/asr/" .. name .. ".png"
    pico.output.screenshot(out)

    local f1 = io.open(out, "rb")
    assert(f1, out)
    local d1 = f1:read("*a")
    f1:close()

    local f2 = io.open(asr, "rb")
    assert(f2, asr)
    local d2 = f2:read("*a")
    f2:close()

    if d1 == d2 then
        print("  ok")
    else
        print("  no")
        error("FAIL: " .. name)
    end
end

return pico.check
