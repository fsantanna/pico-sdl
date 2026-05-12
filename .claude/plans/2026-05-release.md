# Plan: May 2026 Release Cascade

Releases `pico-sdl` and downstream consumers in one pass.

## Targets

| #  | project                          | rockspec name      | scheme         | from → to    |
|----|----------------------------------|--------------------|----------------|--------------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| v0.4-dev → **v0.5** |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | 0.1-3 → **0.2-1** |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | v0.4 → **v0.5** |
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | v0.4 → **v0.5** |
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | v0.6 → **v0.7** |
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | v0.6 → **v0.7** |

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

---

## 1. pico-sdl v0.5

Working dir: `/x/pico-sdl`.

### 1.1. Run C tests

```bash
make tests
```

### 1.2. Build Lua native module and run Lua tests

```bash
cd lua && make tests
```

### 1.3. Run non-automated tests (manual)

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

### 1.4. Create rockspec

- Create `lua/pico-sdl-0.5-1.rockspec` (branch = `"v0.5"`)
- Move previous `lua/pico-sdl-*-1.rockspec` to `lua/old/`

### 1.5. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → `0.5-1`     |
| `README.md`          | bump version (3 places)        |
| `lua/README.md`      | bump version (4 places)        |
| `HISTORY.md`         | add `v0.5` section             |
| `.claude/CLAUDE.md`  | rockspec → `0.5-1`             |

### 1.6. Commit and push main

```bash
git add -A
git commit -m "release: v0.5"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 1.7. Create release branch and push

```bash
git branch v0.5
git push origin v0.5
```

Triggers `docs-version.yml` for `v0.5/` on gh-pages.

### 1.8. Verify local install with luarocks make

```bash
cd lua
sudo luarocks make pico-sdl-0.5-1.rockspec
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 1.9. Publish and verify LuaRocks

```bash
luarocks upload lua/pico-sdl-0.5-1.rockspec
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl 0.5
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

---

## 2. atmos-env-pico 0.2-1

Working dir: `/x/lua-atmos/env-pico`.

Requires pico-sdl 0.5 already on LuaRocks (from §1.9).

### 2.1. Create rockspec

- Create `atmos-env-pico-0.2-1.rockspec` (branch = `"v0.2"`)
- Move previous `atmos-env-pico-*-1.rockspec` to `old/`
- Bump dep: `pico-sdl >= 0.5`

### 2.2. Update files

| File          | Change                                |
|---------------|---------------------------------------|
| `README.md`   | bump version refs (if any)            |
| rockspec      | branch + version + pico-sdl dep       |

### 2.3. Smoke-test examples

```bash
lua5.4 exs/click-drag-cancel.lua
lua5.4 exs/across.lua
lua5.4 exs/hello.lua
```

### 2.4. Commit and push main

```bash
git add -A
git commit -m "release: v0.2"
git push origin main
```

### 2.5. Create release branch and push

```bash
git branch v0.2
git push origin v0.2
```

### 2.6. Verify local install with luarocks make

```bash
sudo luarocks make atmos-env-pico-0.2-1.rockspec
lua5.4 exs/click-drag-cancel.lua
```

### 2.7. Publish and verify LuaRocks

```bash
luarocks upload atmos-env-pico-0.2-1.rockspec
sudo luarocks remove atmos-env-pico
sudo luarocks install atmos-env-pico 0.2
lua5.4 exs/click-drag-cancel.lua
```

---

## 3. lua-atmos/pico-birds v0.5

Working dir: `/x/lua-atmos/pico-birds`.

No rockspec — versioning is git-branch only.

### 3.1. Smoke-test all steps

```bash
for f in birds-*.lua; do pico-lua "$f"; done
```

### 3.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.4` → `git checkout v0.5`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 3.3. Commit and push main

```bash
git add -A
git commit -m "release: v0.5"
git push origin main
```

### 3.4. Create release branch and push

```bash
git branch v0.5
git push origin v0.5
```

---

## 4. lua-atmos/pico-rocks v0.5

Working dir: `/x/lua-atmos/pico-rocks`.

### 4.1. Smoke-test

```bash
pico-lua main.lua
pico-lua battle.lua
```

### 4.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.4` → `git checkout v0.5`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 4.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.5"
git push origin main
git branch v0.5
git push origin v0.5
```

---

## 5. atmos-lang/pico-birds v0.7

Working dir: `/x/atmos-lang/pico-birds`.

Atmos-language flavor (separate version track from §3).

### 5.1. Smoke-test all steps

```bash
for f in birds-*.atm; do atmos "$f"; done
```

### 5.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.6` → `git checkout v0.7`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 5.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.7"
git push origin main
git branch v0.7
git push origin v0.7
```

---

## 6. atmos-lang/pico-rocks v0.7

Working dir: `/x/atmos-lang/pico-rocks`.

### 6.1. Smoke-test

```bash
atmos main.atm
atmos battle.atm
```

### 6.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.6` → `git checkout v0.7`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 6.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.7"
git push origin main
git branch v0.7
git push origin v0.7
```

---

## 7. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
