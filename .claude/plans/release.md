# Plan: Release cascade

Releases `pico-sdl` and downstream consumers in one pass.
Per-project target versions are set in the next session — keep
`<ver>` placeholders throughout (each section's `<ver>` refers
to that project's target).

## Targets (filled in next session)

| #  | project                          | rockspec name      | scheme         | `<ver>` |
|----|----------------------------------|--------------------|----------------|---------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| `<ver>` |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | `<ver>` |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | `<ver>` |
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | `<ver>` |
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | `<ver>` |
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | `<ver>` |

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

---

## 1. pico-sdl `<ver>`

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

- Create `lua/pico-sdl-<ver>-1.rockspec` (branch = `"v<ver>"`)
- Move previous `lua/pico-sdl-*-1.rockspec` to `lua/old/`

### 1.5. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → `<ver>-1`   |
| `README.md`          | bump version (3 places)        |
| `lua/README.md`      | bump version (4 places)        |
| `HISTORY.md`         | add new version section        |
| `.claude/CLAUDE.md`  | rockspec → `<ver>-1`           |

### 1.6. Commit and push main

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 1.7. Create release branch and push

```bash
git branch v<ver>
git push origin v<ver>
```

Triggers `docs-version.yml` for `v<ver>/` on gh-pages.

### 1.8. Verify local install with luarocks make

```bash
cd lua
sudo luarocks make pico-sdl-<ver>-1.rockspec
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 1.9. Publish and verify LuaRocks

```bash
luarocks upload lua/pico-sdl-<ver>-1.rockspec
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl <ver>
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

---

## 2. atmos-env-pico `<ver>`

Working dir: `/x/lua-atmos/env-pico`.

Requires pico-sdl from section 1 already published on LuaRocks.

### 2.1. Create rockspec

- Create `atmos-env-pico-<ver>-1.rockspec` (branch = `"v<ver>"`)
- Move previous `atmos-env-pico-*-1.rockspec` to `old/`
- Bump dep: `pico-sdl >= <pico-sdl-ver>`

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
git commit -m "release: v<ver>"
git push origin main
```

### 2.5. Create release branch and push

```bash
git branch v<ver>
git push origin v<ver>
```

### 2.6. Verify local install with luarocks make

```bash
sudo luarocks make atmos-env-pico-<ver>-1.rockspec
lua5.4 exs/click-drag-cancel.lua
```

### 2.7. Publish and verify LuaRocks

```bash
luarocks upload atmos-env-pico-<ver>-1.rockspec
sudo luarocks remove atmos-env-pico
sudo luarocks install atmos-env-pico <ver>
lua5.4 exs/click-drag-cancel.lua
```

---

## 3. lua-atmos/pico-birds `<ver>`

Working dir: `/x/lua-atmos/pico-birds`.

No rockspec — versioning is git-branch only.

### 3.1. Smoke-test all steps

```bash
for f in birds-*.lua; do pico-lua "$f"; done
```

### 3.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v<prev>` → `git checkout v<ver>` |
| `README.md` | atmos / atmos-env-pico install versions (if pinned) |

### 3.3. Commit and push main

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
```

### 3.4. Create release branch and push

```bash
git branch v<ver>
git push origin v<ver>
```

---

## 4. lua-atmos/pico-rocks `<ver>`

Working dir: `/x/lua-atmos/pico-rocks`.

Same shape as section 3.

### 4.1. Smoke-test

```bash
pico-lua main.lua
pico-lua battle.lua
```

### 4.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v<prev>` → `git checkout v<ver>` |
| `README.md` | atmos / atmos-env-pico install versions (if pinned) |

### 4.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
git branch v<ver>
git push origin v<ver>
```

---

## 5. atmos-lang/pico-birds `<ver>`

Working dir: `/x/atmos-lang/pico-birds`.

Atmos-language flavor of section 3 (separate version track).

### 5.1. Smoke-test all steps

```bash
for f in birds-*.atm; do atmos "$f"; done
```

### 5.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v<prev>` → `git checkout v<ver>` |
| `README.md` | atmos-lang / atmos-env-pico install versions |

### 5.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
git branch v<ver>
git push origin v<ver>
```

---

## 6. atmos-lang/pico-rocks `<ver>`

Working dir: `/x/atmos-lang/pico-rocks`.

Atmos-language flavor of section 4.

### 6.1. Smoke-test

```bash
atmos main.atm
atmos battle.atm
```

### 6.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v<prev>` → `git checkout v<ver>` |
| `README.md` | atmos-lang / atmos-env-pico install versions |

### 6.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v<ver>"
git push origin main
git branch v<ver>
git push origin v<ver>
```

---

## 7. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
