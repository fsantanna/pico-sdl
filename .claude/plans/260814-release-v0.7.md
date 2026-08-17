# Plan: Release cascade — 26-08-14, pico-sdl v0.7

Releases `pico-sdl` `0.7` and downstream consumers in one pass.

## Targets

| #  | project                          | rockspec name      | scheme         | from → to      |
|----|----------------------------------|--------------------|----------------|----------------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| 0.6 → **0.7**  |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | 0.3 → **0.4**  |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | v0.6 → **v0.7**|
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | v0.6 → **v0.7**|
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | v0.8 → **v0.9**|
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | v0.8 → **v0.9**|

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

## Resume point (2026-08-17, switching machines)

- Section 1 (pico-sdl) DONE: `0.7-1` on LuaRocks; `v0.7` pushed
    - main-sync deferred to end (batch)
    - ⚠ open: Ctrl-C on `anims.lua` → segfault on teardown
    - strays left untracked on purpose (font/tile WIP)
- Resume at §2.4: env-pico commit on `v0.4`
- Work state (all UNCOMMITTED, local only — must commit+push
  before other machine can pull):
    - env-pico: branch `v0.4` (no upstream); README,
      `exs/click-drag-cancel.lua` (text.dyn), rockspec `0.4-1`,
      plan `260815-release-v0.4.md`
    - lua-atmos/pico-birds: branch `v0.7` (no upstream);
      README + birds-02..11 (layer.image table args)
    - lua-atmos/pico-rocks: branch `v0.7` (no upstream);
      README, `main.lua` (text.dyn/fix), `ts.lua` (layer.images)
    - atmos-lang/pico-birds: on `main` — CREATE branch first;
      birds-*.atm (layer.image + watching v0.8 syntax)
    - atmos-lang/pico-rocks: on `master` — CREATE branch first;
      `main.atm`, `ts.atm`
    - atmos-lang/atmos: branch `v0.8`; `doc/manual.md` (`x`→`(x)`)
      among other user edits (separate track)
- All smoke-tests already pass headless vs pico-sdl 0.7
  (see per-section Status blocks)
- Interactive still pending: pico-rocks gameplay after ENTER
  (both lua and atm flavors)
- Target versions decided: env-pico 0.4; lua-atmos birds/rocks
  v0.7; atmos-lang birds/rocks v0.9

---

## 1. pico-sdl `0.7`

Working dir: `/x/pico-sdl`.

Status:
- §1.1 ✅ `make tests` pass
- §1.2 ✅ archive fresh; `lua make tests` pass
- §1.3 ✅ compile gate pass (all `tst/todo` + `doc/exs`)
    - demos + guide examples ran headless w/o crash
    - true input checks (mouse/key) not possible headless
- §1.4 ✅ rockspec `0.7-1` created; `0.6-3` → `lua/old/`
- §1.6–1.7 ✅ committed + pushed `origin/v0.7`
- §1.8 ✅ luarocks make + guide examples ok (user)
    - ⚠ Ctrl-C on `anims.lua` → segfault on teardown (investigate)
- §1.3b ✅ interactive `tst/todo` demos verified by user
- §1.9 ✅ `0.7-1` live on luarocks.org; installed + verified
- Section 1 done (main-sync deferred to end)
- §1.5 ✅ bumps: `pico-sdl` script, `Makefile`, READMEs,
  `HISTORY.md` date (aug/26)
- Strategy change: work on branch `v0.7` (created, current);
  merge back / push `main` only at the end (batch sync)
- Pending decision: `pico-sdl:24` VALGRIND= (disabled) —
  keep or revert before commit
- Untracked to triage before commit: `res/PixulBrush.ttf`,
  `tst/font-brush.c`, `tst/text-sizes-sdl.c`, `tst/todo/circle.c`

### 1.1. Run C tests

```bash
make tests
```

### 1.2. Build Lua native module and run Lua tests

`lua/Makefile` does NOT rebuild a stale `../src/libpico-sdl.a`
(its rule has no prerequisites), so rebuild the archive first
whenever `src/*.c` changed:

```bash
make lib
cd lua && make tests
```

### 1.3. Run non-automated tests (manual)

First confirm every manual example still compiles (neither dir is
covered by `make tests`):

```bash
for f in tst/todo/*.c doc/exs/*.c; do
    gcc "$f" src/libpico-sdl.a -I src -lm \
        -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx \
        -o /tmp/ck || echo "FAIL $f"
done
```

Interactive C tests (live in `tst/todo/`):

```bash
for f in main cross control rain hide video input_timeout \
         mouse-rect mouse-rect-click; do
    ./pico-sdl tst/todo/$f.c
done
```

Guide examples (`pico-lua` is at `./lua/pico-lua` unless installed):

```bash
./lua/pico-lua lua/doc/rects.lua
./lua/pico-lua lua/doc/anims.lua
```

### 1.4. Create rockspec

- Create `lua/pico-sdl-0.7-1.rockspec` (branch = `"v0.7"`)
- Move previous `lua/pico-sdl-0.6-3.rockspec` to `lua/old/`

### 1.5. Update files

| File                 | Change                         |
|----------------------|--------------------------------|
| `Makefile`           | rockspec version → `0.7-1`     |
| `README.md`          | bump version (3 places)        |
| `lua/README.md`      | bump version (3 places)        |
| `HISTORY.md`         | fill `v0.7 (???/??)` date      |

### 1.6. Commit and push main

Remove untracked stray files first (`.orig`, scratch `.png`,
old test sources, etc.):

```bash
git status --porcelain   # review, then delete strays
git add -A
git commit -m "release: v0.7"
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 1.7. Create release branch and push

```bash
git branch v0.7
git push origin v0.7
```

Triggers `docs-version.yml` for `v0.7/` on gh-pages.

### 1.8. Verify local install with luarocks make

```bash
cd lua
sudo luarocks make pico-sdl-0.7-1.rockspec
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

### 1.9. Publish and verify LuaRocks

```bash
luarocks upload lua/pico-sdl-0.7-1.rockspec
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl 0.7
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

---

## 2. atmos-env-pico `0.4`

Working dir: `/x/lua-atmos/env-pico`.

Requires pico-sdl `0.7` from section 1 already published on
LuaRocks.

### 2.1. Create rockspec

- Create `atmos-env-pico-0.4-1.rockspec` (branch = `"v0.4"`)
- Move previous `atmos-env-pico-*-1.rockspec` to `old/`
- Bump dep: `pico-sdl >= 0.7`

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
git commit -m "release: v0.4"
git push origin main
```

### 2.5. Create release branch and push

```bash
git branch v0.4
git push origin v0.4
```

### 2.6. Verify local install with luarocks make

```bash
sudo luarocks make atmos-env-pico-0.4-1.rockspec
lua5.4 exs/click-drag-cancel.lua
```

### 2.7. Publish and verify LuaRocks

```bash
luarocks upload atmos-env-pico-0.4-1.rockspec
sudo luarocks remove atmos-env-pico
sudo luarocks install atmos-env-pico 0.4
lua5.4 exs/click-drag-cancel.lua
```

---

## 3. lua-atmos/pico-birds `v0.7`

Working dir: `/x/lua-atmos/pico-birds`.

No rockspec — versioning is git-branch only.

Status:
- §3.1 ✅ all `birds-*.lua` pass headless vs local v0.7
    - migrated `layer.image` positional → table args (02–11)
    - env-pico `click-drag-cancel.lua` also migrated
      (`text.dyn(key, text, rect)`)

### 3.1. Smoke-test all steps

```bash
for f in birds-*.lua; do pico-lua "$f"; done
```

### 3.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.6` → `git checkout v0.7`    |
| `README.md` | atmos / atmos-env-pico install versions (if pinned) |

### 3.3. Commit and push main

```bash
git add -A
git commit -m "release: v0.7"
git push origin main
```

### 3.4. Create release branch and push

```bash
git branch v0.7
git push origin v0.7
```

---

## 4. lua-atmos/pico-rocks `v0.7`

Working dir: `/x/lua-atmos/pico-rocks`.

Same shape as section 3.

Status:
- §4.1 ✅ `main.lua` passes headless vs v0.7
    - `ts.lua`: `layer.images` positional → table (3 sites)
    - `main.lua`: `text` → `text.dyn` (scores), `text.fix` (banner)
    - gameplay after ENTER not exercised headless
    - `battle.lua` is a module of `main.lua`, not an entry point

### 4.1. Smoke-test

```bash
pico-lua main.lua
pico-lua battle.lua
```

### 4.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.6` → `git checkout v0.7`    |
| `README.md` | atmos / atmos-env-pico install versions (if pinned) |

### 4.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.7"
git push origin main
git branch v0.7
git push origin v0.7
```

---

## 5. atmos-lang/pico-birds `v0.9`

Working dir: `/x/atmos-lang/pico-birds`.

Atmos-language flavor of section 3 (separate version track).

Status:
- §5.1 ✅ all `birds-*.atm` pass headless (pico v0.7 + atmos v0.8)
    - pico: `layer.image` positional → `[ key=, path= ]` (11 files)
    - atmos v0.8 syntax:
        - `watching until c` → `watching until (c)` (05,06,08–11)
        - `watching \e{...}` → `watching <:collided || until (...)>`
          (07)
        - `watching bird` → `watching (bird)` (10,11)

### 5.1. Smoke-test all steps

```bash
for f in birds-*.atm; do atmos "$f"; done
```

### 5.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.8` → `git checkout v0.9`    |
| `README.md` | atmos-lang / atmos-env-pico install versions |

### 5.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.9"
git push origin main
git branch v0.9
git push origin v0.9
```

---

## 6. atmos-lang/pico-rocks `v0.9`

Working dir: `/x/atmos-lang/pico-rocks`.

Atmos-language flavor of section 4.

Status:
- §6.1 ✅ `main.atm` passes headless (menu; ENTER not pressed)
    - `ts.atm`: `layer.images` → table (3), `until (...)` parens
    - `main.atm`: `text.dyn` scores, `text.fix` banner
    - `battle.atm`/`ts.atm` gameplay needs interactive run

### 6.1. Smoke-test

```bash
atmos main.atm
atmos battle.atm
```

### 6.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.8` → `git checkout v0.9`    |
| `README.md` | atmos-lang / atmos-env-pico install versions |

### 6.3. Commit, push main, create branch

```bash
git add -A
git commit -m "release: v0.9"
git push origin main
git branch v0.9
git push origin v0.9
```

---

## 6.5. Extra consumers — won't do

- `lessmilk`, `pixelization` — skipped by decision

## 7. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists

---

## 8. Backport to template `release.md`

Working dir: `/x/pico-sdl`.

After the cascade completes, fold anything relevant learned
this run back into `.claude/plans/release.md`.

### 8.1. Capture deltas during execution

- [x] §1.5 table misses `pico-sdl` script `VERSION=` line
- [x] Makefile rockspec was stale at `0.6-2` (not latest `0.6-3`)
- [ ] (running list of divergences from the template)
