# Plan: Release v0.3

## Context

pico-sdl is 528 commits ahead of the v0.2 branch.
All v0.3 features listed in CLAUDE.md are implemented (colors, layers,
push/pop, view refactoring, expert FPS, etc.).
Guide documentation and animation support are the most recent work.
This plan uses release branches (not tags) for versioning.

## Steps

### 1. ~~Run C tests~~ DONE

```bash
make tests
```

### 2. ~~Build Lua native module and run Lua tests~~ DONE

```bash
cd lua && make tests
```

### 2b. Run non-automated tests (manual) — PENDING

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

### 3. ~~Create rockspec `lua/pico-sdl-0.3-1.rockspec`~~ DONE

Copy from previous rockspec, change:
- `version` to new version
- `tag` → `branch` pointing to release branch

Move previous rockspec to `lua/old/`.

### 3b. ~~Move old rockspec to `lua/old/`~~ DONE

### 4. ~~Update `Makefile` rockspec reference~~ DONE

```makefile
lua:
    sudo luarocks make lua/pico-sdl-0.3-1.rockspec --lua-version=5.4
```

### 5. ~~Update `README.md`~~ DONE

- Add version to version list + point stable link
- **Also update** (missed in original plan):
    - `README.md` — Doxygen API link to release version
    - `lua/README.md` — version list, stable link, install examples
    - `HISTORY.md` — release date, verify completeness
    - `lua/doc/api.md` — add any new API entries

### 6. ~~Update `CLAUDE.md` rockspec reference~~ DONE

In Build section, the `luarocks make` example references `0.1-2`.
Update to `0.3-1`.

### 7. ~~Commit all changes~~ DONE

Single commit: `release: v0.3`

### 8. ~~Push main~~ DONE

```bash
git push origin main
```

This triggers:
- `tests.yml` — CI validates
- `docs.yml` — deploys docs to gh-pages `main/`

### 8b. Create release branch and push — PENDING

1. Create branch `v0.3` from `main`
2. On `v0.3`: change README links `main` → `v0.3`, commit
3. Push branch `v0.3`

```bash
git branch v0.3
git checkout v0.3
# edit README links
git commit -am "release: v0.3 branch links"
git push origin v0.3
```

This triggers:
- `docs-tag.yml` — copies docs to `v0.3/` on gh-pages
  (workflow must be updated: `tags: ['v*']` → `branches: ['v*']`)

### 8c. Update `docs-tag.yml` to trigger on branches — PENDING

Change trigger from `tags: ['v*']` to `branches: ['v*']`.
Workflow logic stays the same (copies `main/` docs to `v0.3/`
on gh-pages).

### 8d. Delete existing `v0.3` tag — LATER

```bash
git tag -d v0.3
git push origin :refs/tags/v0.3
```

### 9. Publish to LuaRocks (manual) — PENDING

```bash
luarocks upload lua/pico-sdl-0.3-1.rockspec
```

### 9b. Verify LuaRocks install (manual)

```bash
sudo luarocks remove pico-sdl
sudo luarocks install pico-sdl 0.3
pico-lua lua/doc/rects.lua
pico-lua lua/doc/anims.lua
```

Also read through `lua/doc/guide.md` for correctness.

### 10. Announce (manual) — PENDING

- Twitter
- Students
- SDL lists
- Lua lists

## Files to modify

| File                                | Change                             |
|-------------------------------------|------------------------------------|
| `lua/pico-sdl-0.3-1.rockspec`      | `tag` → `branch`                  |
| `lua/old/`                          | Move previous rockspec here        |
| `Makefile:16`                       | Rockspec version                   |
| `README.md`                         | Version list + stable link         |
| `lua/README.md`                     | Version list, stable, install      |
| `.claude/CLAUDE.md`                 | Rockspec example                   |
| `HISTORY.md`                        | Date + verify completeness         |
| `lua/doc/api.md`                    | New API entries                    |
| `.github/workflows/docs-tag.yml`   | Trigger: `tags` → `branches`      |
| `README.md` (on v0.3 branch)       | Links `main` → `v0.3`             |
| `lua/README.md` (on v0.3 branch)   | Links `main` → `v0.3`             |

## Verification

1. C tests (`make tests`) and Lua tests (`cd lua && make tests`)
   pass before and after changes
2. `git diff` shows only version bumps + README update
3. After push, check GitHub Actions for green CI
4. After branch push, verify `docs-tag` workflow creates `v0.3/`
   on gh-pages
