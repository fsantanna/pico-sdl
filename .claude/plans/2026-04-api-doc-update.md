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

- Verify `pico.color.hex` is actually bound in `lua/pico.c`
  (`ll_color`) before adding to docs — pico.h has it but the
  Lua table may not. If absent, mark as gap instead.

## Pending

- [ ] Verify `pico.color.hex` is in `ll_color` (`lua/pico.c:1574`)
- [ ] Add `up` param to all `pico.layer.*` signatures
- [ ] Add `tile` param to `pico.layer.empty`
- [ ] Add `pico.cv.dim` entry
- [ ] Add `pico.get.keyboard` entry
- [ ] Add `pico.set.mouse` entry
- [ ] Add `pico.color.hex` entry (if bound)
- [ ] Remove `pico.layer.images` entry
- [ ] Update Event type: add `win.resize`
- [ ] Update `key.dn`/`key.up` event: add `ctrl`, `shift`, `alt`
- [ ] Update mouse events: replace `but` with
      `left`, `right`, `middle`
- [ ] Re-grep `ll_*` tables and `pico.h` to confirm no new
      drift after edits
