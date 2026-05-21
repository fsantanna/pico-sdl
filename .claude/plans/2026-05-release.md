# Plan: May 2026 Release Cascade

Releases `pico-sdl` and downstream consumers in one pass.

## Targets

| #  | project                          | rockspec name      | scheme         | from → to            | status      |
|----|----------------------------------|--------------------|----------------|----------------------|-------------|
| 1  | `pico-sdl`                       | `pico-sdl`         | repo + rockspec| v0.4-dev → **v0.5**  | partial: branch ✓, main ✗, LuaRocks ✗ |
| 2  | `lua-atmos/env-pico`             | `atmos-env-pico`   | rockspec       | 0.1-3 → **0.2-1**    | partial: branch ✓, origin/main ✗, LuaRocks ✗ |
| 3  | `lua-atmos/pico-birds`           | —                  | git branch     | v0.4 → **v0.5**      | partial: local v0.5 ✓, README ✗, origin/v0.5 ✗, main ✗ |
| 4  | `lua-atmos/pico-rocks`           | —                  | git branch     | v0.4 → **v0.5**      | partial: v0.5 ✓ (README+push), master ✗ |
| 5  | `atmos-lang/pico-birds`          | —                  | git branch     | v0.6 → **v0.7**      | partial: v0.7 ✓ (README+push), main ✗ |
| 6  | `atmos-lang/pico-rocks`          | —                  | git branch     | v0.6 → **v0.7**      | partial: v0.7 ✓ (README+push), master ✗ |

Execute in order — env-pico needs new pico-sdl on LuaRocks,
and pico-birds / pico-rocks READMEs reference env-pico version.

**Invariant:** after each project finishes, `main` (or `master`) must equal
the new `vX.Y` branch — i.e., the default branch is never left behind.
`pico-rocks` uses `master`; all others use `main`.

**Sync strategy:** all default-branch syncs (FF `main`/`master` → push)
are **batched into §7**, executed after every project's release-branch
work and LuaRocks publish is verified. Per-project sections (§1–§6)
only touch the work/release branches; they do **not** push `main`/`master`.

## Status

**Resume point:** finish §3 commit; then continue §4–§6 work-branch
edits + pushes, then §1/§2 LuaRocks publish, then §7 batch sync.

| § | done                          | pending                          |
|---|-------------------------------|----------------------------------|
| 1 | 1.3, 1.4, 1.6                 | 1.1, 1.2, 1.5, 1.7, 1.8          |
| 2 | 2.1, 2.2, 2.5                 | 2.3, 2.4, 2.6, 2.7               |
| 3 | 3.1, 3.2, 3.4                 | 3.3                              |
| 4 | 4.1, 4.2, 4.3                 | —                                |
| 5 | 5.1, 5.2, 5.3                 | —                                |
| 6 | 6.1, 6.2, 6.3                 | —                                |
| 7 | —                             | all (default-branch sync batch)  |
| 8 | —                             | all (announce)                   |

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
| `pico-sdl`       | `VERSION=v0.4-dev` → `v0.5` (line 3)                    |
| `Makefile`       | `make lua` rockspec → `0.5-1`; `--version` check → `v0.5` |
| `README.md`      | prepend `v0.5` list entry; stable + api link → `v0.5`   |
| `lua/README.md`  | prepend `v0.5`; stable + install `0.5`; dev rockspec → `dev-2` |
| `HISTORY.md`     | add `v0.5` section                                      |

### 1.5. Commit on work branch

Stay on `2026-05-release` (or current worktree branch). Main FF + push
are deferred to §7.

```bash
git add -A
git commit -m "release: v0.5"
```

### 1.6. Create release branch and push

Branch `v0.5` from current HEAD; push triggers `docs-version.yml`
for `v0.5/` on gh-pages.

```bash
git branch v0.5
git push origin v0.5
```

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

### 2.4. Commit on work branch

Main push deferred to §7.

```bash
git add -A
git commit -m "release: v0.2"
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

Working dir: `/x/lua-atmos/pico-birds/.work/v0.5`
(worktree on branch `v0.5`; main repo at `/x/lua-atmos/pico-birds`).

No rockspec — versioning is git-branch only.

Main FF + push deferred to §7. Parity check also in §7.

### Checklist

- [x] 3.1. smoke-test `birds-*.lua`
- [x] 3.2. README: `git checkout v0.4` → `v0.5`; env-pico `0.1` → `0.2`
- [x] 3.3. commit on `v0.5` (`3155a3d`)
- [x] 3.4. push `origin v0.5` (at `dd7044e`)

### 3.1. Smoke-test all steps

```bash
for f in birds-*.lua; do pico-lua "$f"; done
```

### 3.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.4` → `git checkout v0.5`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 3.3. Commit on `v0.5`

```bash
git add -A
git commit -m "release: v0.5"
```

### 3.4. Push `origin v0.5`

```bash
git push origin v0.5
```

---

## 4. lua-atmos/pico-rocks v0.5

Working dir: `/x/lua-atmos/pico-rocks/.work/v0.5`
(worktree on branch `v0.5`; main repo at `/x/lua-atmos/pico-rocks`).

Master push deferred to §7.

### Checklist

- [x] 4.1. smoke-test `main.lua`, `battle.lua`
- [x] 4.2. README: `v0.5`, env-pico `0.2`
- [x] 4.3. commit + push `origin v0.5` (at `503da7f`)

### Observed state (2026-05-21)

- worktree clean on `v0.5`; `origin/v0.5` exists at `503da7f`
- `master` at `3d5d6b4`, behind `v0.5` (sync in §7.4)
- README already updated: `git checkout v0.5`, `atmos-env-pico 0.2`

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

### 4.3. Commit on work branch + push `v0.5`

Default branch here is `master` (not `main`). Master push deferred to §7.

```bash
git add -A
git commit -m "release: v0.5"
git branch v0.5
git push origin v0.5
```

---

## 5. atmos-lang/pico-birds v0.7

Working dir: `/x/atmos-lang/pico-birds/.work/v0.7`
(worktree on branch `v0.7`; main repo at `/x/atmos-lang/pico-birds`).

Atmos-language flavor (separate version track from §3).
Main push deferred to §7.

### Checklist

- [x] 5.1. smoke-test `birds-*.atm`
- [x] 5.2. README: `git checkout v0.6` → `v0.7`; env-pico `0.1` → `0.2`
- [x] 5.3. commit on `v0.7` (`7912ec3`); push `origin v0.7` (at `1171d0c`)

### Observed state (2026-05-21)

- worktree clean on `v0.7` at `95418e5` (same as `main`; no work commit yet)
- `v0.7` is local-only; `origin/v0.7` does **not** exist
- README still shows `git checkout v0.6` and `atmos-env-pico 0.1`

### 5.1. Smoke-test all steps

```bash
for f in birds-*.atm; do atmos "$f"; done
```

### 5.2. Update README

| File        | Change                                       |
|-------------|----------------------------------------------|
| `README.md` | `git checkout v0.6` → `git checkout v0.7`    |
| `README.md` | bump atmos-env-pico install to `0.2`         |

### 5.3. Commit on work branch + push `v0.7`

Main push deferred to §7.

```bash
git add -A
git commit -m "release: v0.7"
git branch v0.7
git push origin v0.7
```

---

## 6. atmos-lang/pico-rocks v0.7

Working dir: `/x/atmos-lang/pico-rocks/.work/v0.7`
(worktree on branch `v0.7`; main repo at `/x/atmos-lang/pico-rocks`).

Master push deferred to §7.

### Checklist

- [x] 6.1. smoke-test `main.atm`, `battle.atm`
- [x] 6.2. README: `git checkout v0.6` → `v0.7`; env-pico `0.1` → `0.2`
- [x] 6.3. commit on `v0.7`; push `origin v0.7` (at `74dab5a`)

### Observed state (2026-05-21)

- worktree clean on `v0.7` at `74dab5a`, tracking `origin/v0.7`
- `master` at `8dd6ab7` (behind; sync in §7.6)

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

### 6.3. Commit on work branch + push `v0.7`

Default branch here is `master` (not `main`). Master push deferred to §7.

```bash
git add -A
git commit -m "release: v0.7"
git branch v0.7
git push origin v0.7
```

---

## 7. Sync default branches (batch)

Run **only after** every project's release-branch work, LuaRocks
publishes (§1.8, §2.7), and verifications are complete.

For each repo: FF default branch to the release branch, push, verify.

| repo                       | default | release | dir                                       |
|----------------------------|---------|---------|-------------------------------------------|
| `pico-sdl`                 | main    | v0.5    | `/x/pico-sdl`                             |
| `lua-atmos/env-pico`       | main    | v0.2    | `/x/lua-atmos/env-pico`                   |
| `lua-atmos/pico-birds`     | main    | v0.5    | `/x/lua-atmos/pico-birds`                 |
| `lua-atmos/pico-rocks`     | master  | v0.5    | `/x/lua-atmos/pico-rocks`                 |
| `atmos-lang/pico-birds`    | main    | v0.7    | `/x/atmos-lang/pico-birds`                |
| `atmos-lang/pico-rocks`    | master  | v0.7    | `/x/atmos-lang/pico-rocks`                |

Per-repo procedure (substitute `<default>` and `<release>`):

```bash
git checkout <default>
git pull --ff-only
git merge --ff-only <release>
git push origin <default>

# parity check
test "$(git rev-parse <default>)" = "$(git rev-parse <release>)"
test "$(git rev-parse origin/<default>)" = "$(git rev-parse origin/<release>)"
```

### Checklist

- [ ] 7.1. pico-sdl: main ← v0.5
- [ ] 7.2. env-pico: main ← v0.2
- [ ] 7.3. lua-atmos/pico-birds: main ← v0.5
- [ ] 7.4. lua-atmos/pico-rocks: master ← v0.5
- [ ] 7.5. atmos-lang/pico-birds: main ← v0.7
- [ ] 7.6. atmos-lang/pico-rocks: master ← v0.7

---

## 8. Announce (manual)

- Twitter
- Students
- SDL lists
- Lua lists
