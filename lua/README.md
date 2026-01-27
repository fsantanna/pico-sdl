# Install

## Local

```bash
cd ../  # pico-sdl root
sudo luarocks make lua/pico-sdl-0.2-1.rockspec
```

## Remote

```bash
sudo luarocks install pico-sdl 0.2
```

# Testing

```bash
LUA_CPATH="../pico/?.so;" LUA_PATH="../?/init.lua;../?.lua;" ./pico-lua tst/init.lua
# or
make tests
```
