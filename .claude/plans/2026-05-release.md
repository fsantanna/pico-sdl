# Plan: May 2026 Release Cascade

Releases `pico-sdl` and downstream consumers in one pass.

## Targets

| #  | project                          | rockspec name      | scheme         | from → to            | status      |
|----|----------------------------------|--------------------|----------------|----------------------|-------------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| v0.4-dev → **v0.5**  | partial: branch ✓, main ✗, LuaRocks ✗ |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | 0.1-3 → **0.2-1**    | partial: branch ✓, origin/main ✗, LuaRocks ✗ |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | v0.4 → **v0.5**      | partial: v0.5 ✓, main ✗ |
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | v0.4 → **v0.5**      | partial: v0.5 ✓, master ✗ |
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | v0.6 → **v0.7**      | partial: v0.7 ✓, main ✗ |
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | v0.6 → **v0.7**      | partial: v0.7 ✓, master ✗ |

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

**Invariant:** after each project finishes, `main` (or `master`) must equal
the new `vX.Y` branch — i.e., the default branch is never left behind.
`pico-rocks` uses `master`; all others use `main`.

## Status

**Resume point:** sync default branches + publish 2 rockspecs.

Release branches all exist on origin (`v0.5` ×4, `v0.2`, `v0.7` ×2),
but the **invariant is violated everywhere** — `main` / `master` is
1 commit behind its release branch in every repo.
Plus the two LuaRocks publishes are pending.

| § | done                          | pending                          |
|---|-------------------------------|----------------------------------|
| 1 | 1.3, 1.4, 1.6                 | 1.1, 1.2, 1.5, 1.7, 1.8          |
| 2 | 2.1, 2.2, 2.5                 | 2.3, 2.4 (push origin/main), 2.6, 2.7 |
| 3 | 3.2, 3.4                      | 3.1, 3.3, 3.5                    |
| 4 | 4.2, 4.3 (branch only)        | 4.1, 4.3 (push master), 4.4      |
| 5 | 5.2, 5.3 (branch only)        | 5.1, 5.3 (push main), 5.4        |
| 6 | 6.2, 6.3 (branch only)        | 6.1, 6.3 (push master), 6.4      |
| 7 | —                             | all                              |

Notes:
- Smoke-tests (1.1, 1.2, 2.3, 3.1, 4.1, 5.1, 6.1) and luarocks-make
  verifications (1.7, 2.6) are not verifiable from repo state — user
  must confirm or re-run.
- LuaRocks search confirms:
    - `pico-sdl` latest published = **0.3.1-1** (needs 0.5-1) → 1.8 pending
    - `atmos-env-pico` latest published = **0.1-3** (needs 0.2-1) → 2.7 pending
- env-pico: local `main` already at v0.2 tip but **not pushed** to
  origin/main (still at 2b2aaea).

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

### 1.3. Create rockspec

- Create `lua/pico-sdl-0.5-1.rockspec` (branch = `"v0.5"`)
- Keep `lua/pico-sdl-dev-2.rockspec` in place
    - same content as v0.5-1 (only `version` + `branch` differ)
    - archive to `lua/old/` only when content changes (new deps/sources)

### 1.4. Update files

| File             | Change                                                  |
|------------------|---------------------------------------------------------|
| `Makefile`       | `make lua` rockspec → `0.5-1`; `--version` check → `v0.5` |
| `README.md`      | prepend `v0.5` list entry; stable + api link → `v0.5`   |
| `lua/README.md`  | prepend `v0.5`; stable + install `0.5`; dev rockspec → `dev-2` |
| `HISTORY.md`     | add `v0.5` section                                      |

### 1.5. Commit and push main

If on a worktree branch, fast-forward `main` first.

```bash
git add -A
git commit -m "release: v0.5"
git checkout main
git merge --ff-only 2026-05-release
git push origin main
```

Triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 1.6. Create release branch and push

Branch from `main` so `main == v0.5`.

```bash
git branch v0.5 main
git push origin v0.5
```

Triggers `docs-version.yml` for `v0.5/` on gh-pages.

### 1.7. Verify local install with luarocks make

```bash
sudo luarocks --lua-version=5.4 make lua/pico-sdl-0.5-1.rockspec
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

Branch from `main` so `main == v0.2`.

```bash
git branch v0.2 main
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

Branch from `main` so `main == v0.5`.

```bash
git branch v0.5 main
git push origin v0.5
```

### 3.5. Verify default branch parity

```bash
test "$(git rev-parse main)" = "$(git rev-parse v0.5)"
test "$(git rev-parse origin/main)" = "$(git rev-parse origin/v0.5)"
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

### 4.3. Commit, push master, create branch

Default branch here is `master` (not `main`).

```bash
git add -A
git commit -m "release: v0.5"
git push origin master
git branch v0.5 master
git push origin v0.5
```

### 4.4. Verify default branch parity

```bash
test "$(git rev-parse master)" = "$(git rev-parse v0.5)"
test "$(git rev-parse origin/master)" = "$(git rev-parse origin/v0.5)"
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
git branch v0.7 main
git push origin v0.7
```

### 5.4. Verify default branch parity

```bash
test "$(git rev-parse main)" = "$(git rev-parse v0.7)"
test "$(git rev-parse origin/main)" = "$(git rev-parse origin/v0.7)"
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

### 6.3. Commit, push master, create branch

Default branch here is `master` (not `main`).

```bash
git add -A
git commit -m "release: v0.7"
git push origin master
git branch v0.7 master
git push origin v0.7
```

### 6.4. Verify default branch parity

```bash
test "$(git rev-parse master)" = "$(git rev-parse v0.7)"
test "$(git rev-parse origin/master)" = "$(git rev-parse origin/v0.7)"
```

---

## 7. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
