# 2026-04 API doc update

Sync `lua/doc/api.md` with current `src/pico.h` (C API) and
`lua/pico.c` (Lua bindings).

## Goal

`lua/doc/api.md` has drifted. Bring it back in line with what
is actually exposed to Lua, using `src/pico.h` as the source
of truth for semantics and `lua/pico.c` (`ll_*` tables) as the
source of truth for which symbols are exported.

## Evidence

Checked against `src/pico.h` and `lua/pico.c` `ll_*` tables.

### Signature mismatches

| API                | docs           | actual                     |
|--------------------|----------------|----------------------------|
| `pico.layer.empty` | `(name, dim)`  | `(up, key, dim, tile)`     |
| `pico.layer.*`     | `(name, ...)`  | `(up, key, ...)` — all 6   |

All layer creators take `up` (parent/nesting target) as first
parameter. None of this is documented.

### Missing from docs (exist in code)

| API                 | pico.h          | lua/pico.c          |
|---------------------|-----------------|---------------------|
| `pico.cv.dim`       | 591             | `ll_cv` 1562        |
| `pico.get.keyboard` | 449             | `ll_get` 1587       |
| `pico.set.mouse`    | 538             | `ll_set` 1606       |
| `pico.color.hex`    | 718             | (check binding)     |

### Event gaps

- `win.resize` event missing from docs
  (`lua/pico.c:1778`)
- `key.dn`/`key.up` events carry `ctrl`, `shift`, `alt`
  (`lua/pico.c:394-398`), not only `key`
- Mouse events carry `left`, `right`, `middle`
  (`lua/pico.c:409-413`), not `but`

### In docs but NOT in code

| API                 | Note                                       |
|---------------------|--------------------------------------------|
| `pico.layer.images` | no `pico_layer_images*` in pico.h or ll_*  |

## Scope

- `lua/doc/api.md` — Types section (Event, Rect/Pos/Dim if
  needed) and Operations section (pico.cv, pico.color,
  pico.layer, pico.get, pico.set)
- No code changes
- No test changes

## Preconditions

- [x] Verify `pico.color.hex` is bound in `lua/pico.c`
  `ll_color` (`lua/pico.c:1574`).
    - Result: **not** a standalone binding.
    - However, `C_color_tis` (`lua/pico.c:147`) accepts an
      integer and routes it through `pico_color_hex`.
    - Consequence: Color in Lua is a union of three forms
      (string / integer-hex / table). Document this on the
      Color type, not as `pico.color.hex`.

## Pending

- [x] Add `up` param to all `pico.layer.*` signatures
      (also documented optional `[mode]` prefix)
- [x] Add `tile` param to `pico.layer.empty`
- [x] Add `pico.cv.dim` entry
- [x] Add `pico.get.keyboard` entry
- [x] Add `pico.set.mouse` entry
- [x] Update Color type: document 3 accepted forms
      (string, integer-hex, table) via `C_color_tis`
- [x] Remove `pico.layer.images` entry
- [x] Update Event type: add `win.resize` event
      (asymmetry resolved: `lua/pico.c:1354` now emits `'win.resize'`)
- [x] Update `key.dn`/`key.up` event: add `ctrl`, `shift`, `alt`
- [x] Update mouse events: replace `but` with
      `left`, `right`, `middle`
- [x] Expand `pico.input.event` filter list
      (`mouse.motion`, `mouse.button.up`, `win.resize`)
- [x] Re-grep `ll_*` tables and `pico.h` to confirm no new
      drift after edits — all 56 bindings match

## Follow-ups

- ~~Event tag asymmetry~~ — resolved by renaming the emitted
  tag to `win.resize` in `lua/pico.c:1354`.
- ~~Expose `pico.color.hex`~~ — declined; integer coercion via
  `C_color_tis` is sufficient.
