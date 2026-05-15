# Add layer arg to pico_output_screenshot

Goal: let `pico_output_screenshot` capture any layer (cur, world,
window, or any named layer) without requiring the caller to do a
`set.layer + restore` dance.

Trigger: `lua/tst/cross.lua` regenerates `res/cross.gif` from the
world layer (no grid). The old gif captured the window layer (with
the dev grid). Today the only way is `set.layer + screenshot +
restore`. Make `screenshot(layer, ...)` first-class.

## 1. Scope

- C: `src/pico.h`, `src/pico.c`
- Lua binding: `lua/pico.c`
- Docs: `lua/doc/api.md`
- Callers: `tst/*.c`, `lua/tst/*.lua`, `lua/doc/gen-*.lua`,
  `lua/pico/check.lua`
- Out of scope: any other API change (still in `2026-04-docs` branch
  for layer/scene/realm work — keep separate)

## 2. Design

### Signature (required-first nil)

```c
// C
const char* pico_output_screenshot (
    const char* layer,         // NULL = cur
    const char* path,          // NULL = timestamp
    const Pico_Rel_Rect* rect  // NULL = full
);
```

```
-- Lua
pico.output.screenshot (layer: string?, [path: string,] [rect: Rect])
    -> string
```

### Why required-first nil

Path and layer are both strings, so a positional `screenshot("foo")`
would be ambiguous (path or layer?). Forcing `layer` first (with
`nil` for cur) removes the ambiguity at the cost of touching every
caller.

### Forms

| call                                | meaning                  |
|-------------------------------------|--------------------------|
| `screenshot(nil)`                   | cur, default path        |
| `screenshot(nil, "x.png")`          | cur, given path          |
| `screenshot("window")`              | window, default path     |
| `screenshot("window", "x.png")`     | window, given path       |
| `screenshot(nil, nil, rect)`        | cur, default path, rect  |
| `screenshot("L", "x.png", rect)`    | full form                |

## 3. Open questions

- [ ] Q1: Should the impl save+restore cur layer internally, or
      switch and leave? (Leaning save+restore — matches mouse/cv
      semantics where `layer` arg never mutates state.)
- [ ] Q2: Drop `path = NULL → timestamp` default? (Out of scope —
      keep as-is.)
- [ ] Q3: Should `rect` be interpreted in the **target layer's**
      frame or in cur? (Leaning target layer's, since we are
      already switching.)

## 4. Steps

### 4.1. Core

- [ ] `src/pico.h:293` — update sig: prepend `const char* layer`.
- [ ] `src/pico.c:1404` — update impl:
    - if `layer != NULL`: save cur, switch to `layer`, screenshot,
      restore cur.
    - else: current behavior.
    - `rect` interpretation: per Q3.
- [ ] `src/pico.c:981` — internal caller: pass `NULL`.

### 4.2. Lua binding

- [ ] `lua/pico.c:1619` — `l_output_screenshot`: parse arg 1 as
      optional `layer: string`, then `path`, then `rect`. Pass
      through.

### 4.3. Docs

- [ ] `lua/doc/api.md:182-183` — sig + forms table + example.

### 4.4. Callers (must update — break)

- [ ] `tst/shot.c`, `tst/window.c`, `tst/todo/cross.c`
- [ ] `lua/tst/cross.lua`, `lua/tst/window.lua`,
      `lua/tst/sheet.lua`, `lua/tst/shot.lua`
- [ ] `lua/doc/gen-guide-rects-gif.lua`,
      `lua/doc/gen-guide-anims-gif.lua`,
      `lua/doc/gen-guide-images.lua`
- [ ] `lua/pico/check.lua`
- [ ] `lua/doc/guide.old.md` — only if still referenced (else skip)

### 4.5. Verify

- [ ] `make tests` (root + lua) all green.
- [ ] `lua/tst/cross.lua` regenerates `res/cross.gif` showing the
      dev grid (matches the old/main version).

## 5. Acceptance

- New sig in `pico.h`, `api.md`.
- All callers compile + tests pass.
- `cross.gif` regen visually matches old/main (200×200, grid + axis).

## 6. Progress

(update as work proceeds)

- [x] 4.1 Core
    - [x] sig in pico.h
    - [x] impl: save+switch+restore around capture
    - [x] internal caller (KEY_S) simplified to one-liner
- [x] 4.2 Lua binding
    - [x] `l_output_screenshot` parses opt `layer` first
    - [x] simplified parsing via `lua_tostring` (NULL on missing/nil)
    - [x] rect accepted at position 2 (no path) or 3
          (single `C_rel_rect(L, i)` call via `int i=-1` dispatch)
- [x] 4.3 Docs
    - [x] api.md sig + forms table + cur-preserved note
    - [x] api.md updated with new (L, rect) and (nil, rect) forms
- [x] 4.4 Callers
    - [x] tst/shot.c (10 sites)
    - [x] tst/todo/cross.c (1 site)
    - [x] lua/tst/shot.lua (10 sites)
    - [x] lua/tst/sheet.lua → collapsed to `screenshot("window", ...)`
    - [x] lua/tst/cross.lua (commented gen-loop updated)
    - [x] lua/pico/check.lua → collapsed to `screenshot("window", ...)`
    - [x] lua/doc/gen-guide-{rects,anims,images}.lua
- [x] 4.5 Verify
    - [x] `make tests` (root) — pass
    - [x] `cd lua && make tests` — pass
    - [x] post-merge fixes: `logo.lua` (path passed as layer);
          `lua/tst/shot.lua` `(nil, nil, rect)` → `(nil, rect)`
    - [x] regen `res/cross.gif` via `lua/tst/cross.lua` gen loop
