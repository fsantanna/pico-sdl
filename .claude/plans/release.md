# Plan: Release v0.3.1

## Context

Patch release adding FPS modes (-1/0/N), delta time from input
functions, and `pico_get_ticks` → `pico_get_now` rename.
Uses release branches (not tags) for versioning.

## Steps

### 1. Run C tests

```bash
make tests
```

### 2. Build Lua native module and run Lua tests

```bash
cd lua && make tests
```

### 3. Run non-automated tests (manual)

Interactive C tests:

```bash
./pico-sdl tst/todo_main.c
./pico-sdl tst/todo_cross.c
./pico-sdl tst/todo_scale.c
./pico-sdl tst/todo_control.c
./pico-sdl tst/todo_rain.c
./pico-sdl tst/todo_hide.c
./pico-sdl tst/todo_video.c
./pico-sdl tst/todo_input_timeout.c
```

Guide examples:

```bash
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 4. Create rockspec `lua/pico-sdl-0.3.1-1.rockspec`

Copy from `lua/pico-sdl-0.3-1.rockspec`, change:
- `version` to `"0.3.1-1"`
- `branch` stays `"v0.3"`

Move previous rockspec to `lua/old/`.

### 5. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → 0.3.1-1    |
| `README.md`          | version list + stable link     |
| `lua/README.md`      | version list, stable, install  |
| `HISTORY.md`         | add v0.3.1 section             |
| `.claude/CLAUDE.md`  | rockspec example               |

### 6. Commit and push main

```bash
git add ...
git commit -m "release: v0.3.1"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 7. Update v0.3 branch

```bash
git checkout v0.3
git merge main
git push origin v0.3
git checkout main
```

Triggers `docs-version.yml` for `v0.3/` on gh-pages.

### 8. Publish to LuaRocks

```bash
luarocks upload lua/pico-sdl-0.3.1-1.rockspec
```

### 9. Verify LuaRocks install

```bash
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl 0.3.1
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 10. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
