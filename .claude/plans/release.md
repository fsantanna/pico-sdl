# Plan: Release v0.3.1

## Context

Patch release adding FPS modes (-1/0/N), delta time from input
functions, and `pico_get_ticks` → `pico_get_now` rename.
Uses release branches (not tags) for versioning.
v0.3.1 replaces v0.3 in version lists (not side by side).

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

### 4. Create rockspec

- Create `lua/pico-sdl-<ver>-1.rockspec` (branch = `"v<ver>"`)
- Move previous `lua/pico-sdl-*-1.rockspec` to `lua/old/`

### 5. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → <ver>-1     |
| `README.md`          | bump version (3 places)        |
| `lua/README.md`      | bump version (4 places)        |
| `HISTORY.md`         | add new version section        |
| `.claude/CLAUDE.md`  | rockspec → <ver>-1             |

### 6. Commit and push main

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 7. Create release branch and push

```bash
git branch v<ver>
git push origin v<ver>
```

Triggers `docs-version.yml` for `v<ver>/` on gh-pages.

### 8. Verify local install with luarocks make

```bash
cd lua
sudo luarocks make pico-sdl-<ver>-1.rockspec
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 9. Publish and verify LuaRocks

```bash
luarocks upload lua/pico-sdl-<ver>-1.rockspec
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl <ver>
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 10. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
