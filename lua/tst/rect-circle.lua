require 'pico.check'

pico.init(true)
pico.set.window { title="Rect-Circle" }
pico.set.dim {'!', w=200, h=100}

do
    print("square from w only (pct)")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.25})
    pico.check("rect-circle-01")
end

do
    print("square from h only (pct)")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, h=0.5})
    pico.check("rect-circle-02")
end

do
    print("circle from w only (pct)")
    pico.output.clear()
    pico.output.draw.oval({'%', x=0.5, y=0.5, w=0.25})
    pico.check("rect-circle-03")
end

do
    print("circle from h only (raw)")
    pico.output.clear()
    pico.output.draw.oval({'!', x=100, y=50, h=60})
    pico.check("rect-circle-04")
end

do
    print("both given: rect and oval unchanged")
    pico.output.clear()
    pico.output.draw.rect({'%', x=0.5, y=0.5, w=0.8, h=0.4})
    pico.set.pencil { color='red' }
    pico.output.draw.oval({'%', x=0.5, y=0.5, w=0.8, h=0.4})
    pico.check("rect-circle-05")
end

pico.init(false)
