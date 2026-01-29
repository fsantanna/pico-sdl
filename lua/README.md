# pico-sdl-lua

A Lua binding for [pico-sdl](../).

[
    [`v0.1`](https://github.com/fsantanna/pico-sdl/tree/v0.1)
]

This is the unstable `main` branch.
Please, switch to stable [`v0.1`](https://github.com/fsantanna/pico-sdl/tree/v0.1).
<!--
-->

[
    [Hello World!](#hello-world)    |
    [Install & Run](#install--run)  |
    [Documentation](#documentation)
]

<img src="pico-logo.png" width="250" align="right">

# Hello World!

The following example draws an `X` on screen gradually with instant feedback:

<picture>
<img align="right" src="../x.gif">
</picture>

```
pico.init(true)
pico.set.view {
    title  = "Draws an X",        -- window title
    window = {'!', w=160, h=160}, -- physical screen size
    world  = {'!', w=16,  h=16},  -- logical screen size (10x10 pixel size)
}
pico.output.clear()
for i = 0, 15 do
    pico.output.draw.pixel({'!', x=i, y=i})    -- raw position (i,i)
    pico.output.draw.pixel({'!', x=15-i, y=i}) -- raw position (15-i,i)
    pico.input.delay(100)
end
pico.init(false)
```

# Install & Run

## Luarocks

```
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
sudo luarocks install pico-sdl 0.2
pico-lua tst/cross.lua
```

## Development

```
cd ../  # pico-sdl root
sudo luarocks make lua/pico-sdl-0.2-1.rockspec
pico-lua /lua/tst/cross.lua
```

# Documentation

- [Guide](doc/guide.md)
- [API](doc/api.md)
