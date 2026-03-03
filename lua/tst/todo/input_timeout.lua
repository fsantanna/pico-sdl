local pico = require 'pico'

pico.init(true)

-- Test 1: filter matches within timeout
-- Move the mouse during the 5s window
print("TEST 1: move mouse within 5s")
print("  filter=mouse.motion, timeout=5000")
do
    local e = pico.input.event('mouse.motion', 5000)
    if e then
        print("  OK: got mouse motion")
    else
        print("  FAIL: timed out")
    end
end

-- Test 2: filter does NOT match within timeout
-- Do NOT press any key during the 3s window
print("TEST 2: do NOT press keys for 3s")
print("  filter=key.dn, timeout=3000")
print("  (move mouse to trigger non-matching events)")
do
    local e = pico.input.event('key.dn', 3000)
    if e then
        print("  FAIL: should have timed out")
    else
        print("  OK: timed out correctly")
    end
end

-- Test 3: filter match after non-matching events
-- Move mouse first, THEN press a key within 5s
print("TEST 3: move mouse, then press key within 5s")
print("  filter=key.dn, timeout=5000")
print("  (this is the actual bug scenario)")
do
    local e = pico.input.event('key.dn', 5000)
    if e then
        print("  OK: got key down")
    else
        print("  FAIL: timed out (bug!)")
    end
end

pico.init(false)
