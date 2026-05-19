# Plan: May 2026 Release Cascade

Releases `pico-sdl` and downstream consumers in one pass.

## Targets

| #  | project                          | rockspec name      | scheme         | from → to            | status      |
|----|----------------------------------|--------------------|----------------|----------------------|-------------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| v0.4-dev → **v0.5**  | rockspec + HISTORY done |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | 0.1-3 → **0.2-1**    | pending     |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | v0.4 → **v0.5**      | code done   |
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | v0.4 → **v0.5**      | code done   |
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | v0.6 → **v0.7**      | code done   |
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | v0.6 → **v0.7**      | code done   |

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

## Status (as of 2026-05-19)

**Resume point:** §1.4 in progress — bump versions in `Makefile`, `README.md`,
`lua/README.md`, `.claude/CLAUDE.md` (HISTORY.md ✓ done). Then §1.5–§1.8.

Working dir for §1: `/x/pico-sdl/.work/2026-05-release` (worktree on `2026-05-release` branch).

| § | done                          | pending                              |
|---|-------------------------------|--------------------------------------|
| 1 | C+Lua tests, rockspec, HISTORY.md | Makefile + README + lua/README + CLAUDE.md bumps, push, upload |
| 2 | —                             | all (waits on §1)                    |
| 3 | get.image fix, smoke-test     | README, commit, branch               |
| 4 | API migration, smoke-test     | README, commit, branch               |
| 5 | API migration, get.image, smoke-test | README, commit, branch        |
| 6 | API migration, smoke-test     | README, commit, branch               |
| 7 | —                             | announce                             |

---

## 1. pico-sdl v0.5

Working dir: `/x/pico-sdl`.

### 1.1. Run C tests [DONE]

```bash
make tests
```

### 1.2. Build Lua native module and run Lua tests [DONE]

```bash
cd lua && make tests
```

### 1.3. Create rockspec [DONE]

- Create `lua/pico-sdl-0.5-1.rockspec` (branch = `"v0.5"`)
- Keep `lua/pico-sdl-dev-2.rockspec` in place
    - same content as v0.5-1 (only `version` + `branch` differ)
    - archive to `lua/old/` only when content changes (new deps/sources)

### 1.4. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → `0.5-1`     |
| `README.md`          | bump version (3 places)        |
| `lua/README.md`      | bump version (4 places)        |
| `HISTORY.md`         | add `v0.5` section ✓           |
| `.claude/CLAUDE.md`  | rockspec → `0.5-1`             |

### 1.5. Commit and push main

```bash
git add -A
git commit -m "release: v0.5"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 1.6. Create release branch and push

```bash
git branch v0.5
git push origin v0.5
```

Triggers `docs-version.yml` for `v0.5/` on gh-pages.

### 1.7. Verify local install with luarocks make

```bash
cd lua
sudo luarocks make pico-sdl-0.5-1.rockspec
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 1.8. Publish and verify LuaRocks

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

Requires pico-sdl 0.5 already on LuaRocks (from §1.8).

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

### 3.0. pico.get.image inverted args [DONE]

Collapsed `local pct = {'%'}` + call into one-liner
`local pct = pico.get.image('%', path)` across
birds-07/08/09/10/11.lua (+ birds-11 pause block).

### 3.1. Smoke-test all steps [DONE]

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

### 4.0. Update to pico-sdl 0.5 API [DONE]

| file       | change                                                            |
|------------|-------------------------------------------------------------------|
| main.lua   | `pico.set.color.draw 'white'` → `pico.set.pencil { color=... }`   |
| battle.lua | `pico.vs.rect_rect` → `pico.vs.rect.rect`                         |
| ts.lua     | `pico.layer.images(...)` → add `up=nil` first arg (×3)            |
| ts.lua     | `pico.set.color.draw(...)` → `pico.set.pencil { color=... }` (×2) |
| ts.lua     | drop unused `local H = pico.get.view().dim.h`                     |

### 4.1. Smoke-test [DONE]

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

### 5.0. Update to pico-sdl 0.5 API [DONE]

Applied across all 11 `birds-*.atm`:

| change                                              | files            |
|-----------------------------------------------------|------------------|
| `3.14` → `math.pi`                                  | 01–11            |
| preregister `pico.layer.image(nil, :up/:dn, ...)`; bird sprite refs use tags `:up`/`:dn` (no `UP`/`DN` vars) | 01–11 |
| `pico.output.draw.image(...)` → `pico.output.draw.layer(...)` (bird sprites) | 01–11 |
| `pico.get.image(UP, pct)` → path-literal arg        | 07–11            |
| `pico.vs.rect_rect` → `pico.vs.rect.rect`           | 07–11            |
| `pico.vs.pos_rect` → `pico.vs.pos.rect`             | 10, 11           |
| pause-image `draw.image(img, r)` kept as-is (img is a path) | 11 only   |

### 5.0a. pico.get.image inverted args [DONE]

Same one-liner collapse applied across birds-07/08/09/10/11.atm
(+ birds-11 pause block) using `"%"` string form.

### 5.1. Smoke-test all steps [DONE]

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

### 6.0. Update to pico-sdl 0.5 API [DONE]

| file        | change                                                            |
|-------------|-------------------------------------------------------------------|
| main.atm    | `pico.zet.color.draw "white"` → `pico.zet.pencil @{ color="white" }` (×2) |
| battle.atm  | `pico.vs.rect_rect` → `pico.vs.rect.rect`                         |
| ts.atm      | `pico.layer.images(...)` → add `nil` first arg (×3)               |
| ts.atm      | `pico.zet.color.draw(...)` → `pico.zet.pencil @{ color=... }` (×2)|
| ts.atm      | drop unused `val H = pico.get.view().dim.h`                       |

### 6.1. Smoke-test [DONE]

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
