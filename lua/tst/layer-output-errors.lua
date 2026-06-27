local pico = require "pico"

-- Errors reachable from _pico_layer_output (src/layer.c). Each case must
-- raise a catchable Lua error so pcall can assert its message.
-- We fix the C/binding side one by one until every case passes.

pico.init(true)

-- Case A: zero-width source. Guarded in l_set_scene.
do
    local ok, err = pcall(function ()
        pico.set.scene {
            source = {'!', x=0, y=0, w=0, h=100, anchor='NW'},
        }
    end)
    assert(not ok, "expected error for zero-width source")
    assert(err:match("invalid source"), err)
    print("A: ok")
end

-- Case B: positive source (w=10) that truncates to 0 inside C clipping
-- (target shifted 95px off-left: src.w -= 95*10/100 = 9.5 -> int 0).
do
    local ok, err = pcall(function ()
        pico.set.scene {
            source = {'!', x=0,   y=0, w=10,  h=100, anchor='NW'},
            target = {'!', x=-95, y=0, w=100, h=100, anchor='NW'},
        }
        pico.output.clear()
        pico.input.delay(10)
    end)
    assert(not ok, "expected error for source truncated to zero")
    assert(err:match("invalid source"), err)
    print("B: ok")
end

pico.init(false)
