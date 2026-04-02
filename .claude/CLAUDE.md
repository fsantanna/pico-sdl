# CLAUDE.md

- In `valgrind.supp`, last clause `sdl-init`, line `src:pico.c:N`.
    - `src:pico.c:N` points to file `src/pico.c` at line `N`
    - `N` is the line in `pico_init` that calls `SDL_Init`
    - **update `N` whenever `src/pico.c` changes**

## Build & Run

### C Programs

Compile and run a C program:

```bash
./pico-sdl path/to/program.c
```

Manual compilation:

```bash
gcc /path/to/program.c src/pico.c -I src \
    -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
```

### `pico-lua`

The `lua/` directory contains Lua bindings for pico-sdl.

Run a Lua program:

```bash
make lua T=<test>   # tst/test.lua
```

## Tests

### Running

```bash
make tests
cd lua/ && make tests
```

### Visual Regression

Visual tests use `check.h` to compare rendered output against expected images.

```bash
make int  T=<app>   # inspect
make gen  T=<app>   # generate asr/
make test T=<app>   # check
```
