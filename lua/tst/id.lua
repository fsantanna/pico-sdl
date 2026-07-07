pico.init(true)

print "pico.id: monotonic integer"
do
    local a = pico.id()
    local b = pico.id()
    assert(math.type(a) == 'integer')
    assert(a >= 1)
    assert(b == a+1)
end

print "pico.id: prefix form"
do
    local s = pico.id("bullet")
    local n = tonumber(s:match("^bullet%-(%d+)$"))
    assert(n ~= nil)
    assert(pico.id() == n+1)
end

print "layer constructors return the key"
do
    local e = pico.layer.empty { up="world", key="e1", clear=true, dim={'!', w=50, h=50} }
    assert(e == "e1")
    local t = pico.layer.text { key="t1", dim={'!', h=20}, text="hi" }
    assert(t == "t1")
    local p = pico.layer.pixmap { key="p1", pixels={{{r=255, g=0, b=0, a=255}}} }
    assert(p == "p1")
end

print "key omitted: auto-generates /id-N"
do
    local a = pico.layer.empty { dim={'!', w=10, h=10} }
    local b = pico.layer.empty { dim={'!', w=10, h=10} }
    assert(a:match("^/id%-%d+$"))
    assert(b:match("^/id%-%d+$"))
    assert(a ~= b)
    local s = pico.layer.sub { sup=a, crop={'%', x=0.5, y=0.5, w=1, h=1} }
    assert(s:match("^/id%-%d+$"))
    local h = pico.layer.screenshot { sup=a }
    assert(h:match("^/id%-%d+$"))
end

print "image: key omitted defaults to path"
do
    local k = pico.layer.image { path="../../res/open.png" }
    assert(k == "../../res/open.png")
end

print "\n=== ALL TESTS PASSED ==="
pico.init(false)
