# pico.layer: Keyword-Table Constructors (full migration)

Redesign every `pico.layer.*` Lua constructor around a single
keyword-table argument. No legacy positional forms — all call sites
migrate. Companion of [260704-layer-target.md] (target semantics);
this plan covers the constructor surface.

## Current (positional, 7 functions)

```
pico.layer.empty      ([m], up, key, clear, Dim|Rect [, Tile [, Rect]])
pico.layer.image      ([m], up, [key,] path [, Rect])
pico.layer.pixmap     ([m], up, key, pix [, Rect])
pico.layer.text       ([m], up, key, height, text [, Rect])
pico.layer.video      ([m], up, [key,] path [, Rect])
pico.layer.sub        ([m], up, key, sup, crop [, Rect])
pico.layer.screenshot ([m], up, key, [src] [, region])
```

Problems: duck-typed Dim/Rect dispatch, optional-key stack shuffling
(image/video insert nil), boolean trap (`clear`), inert `clear` on
detached, `rect` vs `target` naming, unknown args silently ignored,
trailing optionals disambiguated by field-sniffing.

## Proposed (one table each)

```lua
pico.layer.empty      { key=, up=, mode=, clear=, dim=, tile=, target= }
pico.layer.image      { key=, up=, mode=, path=,                target= }
pico.layer.pixmap     { key=, up=, mode=, pix=,                 target= }
pico.layer.text       { key=, up=, mode=, height=, text=,       target= }
pico.layer.video      { key=, up=, mode=, path=,                target= }
pico.layer.sub        { key=, up=, mode=, sup=, crop=,          target= }
pico.layer.screenshot { key=, up=, mode=, src=, region=                 }
```

### Function x field

| fn     | key   | up  | mode  | clear | dim   | tile | target | own |
| ------ | ----- | --- | ----- | ----- | ----- | ---- | ------ | ------------------------ |
| empty  | REQ   | opt | opt^m | opt^c | opt^d | opt  | opt    | -                        |
| image  | opt^p | opt | opt^m | -     | -     | -    | opt    | path: REQ                |
| pixmap | REQ   | opt | opt^m | -     | -     | -    | opt    | pix: REQ                 |
| text   | REQ   | opt | opt^m | -     | -     | -    | opt    | height: REQ, text: REQ   |
| video  | opt^p | opt | opt^m | -     | -     | -    | opt    | path: REQ                |
| sub    | REQ   | opt | opt^m | -     | -     | -    | opt    | sup: REQ, crop: REQ      |
| shot   | REQ   | opt | opt^m | -     | -     | -    | opt    | src: opt(cur), region: opt(full) |

- `^p` key defaults to `path` (today's behavior).
- `^m` mode defaults `'!'`; image/video WITHOUT key default `'='`
  (path-cached), same rule as today but now visible: the key field is
  simply absent.
- `^c` clear defaults `false`; **error** when `clear=true` and
  `up=nil` (inert flag, asymmetry (f) — no legacy to protect now).
- `^d` dim required unless `target.w,h > 0` (rect-as-dim inference,
  now explicit); error otherwise. `dim` + `target` together finally
  expresses "size X placed at rect" in one call.

### Tile: sibling field (embedded alternative rejected)

`tile` stays a sibling of `dim` (form A), as today:

```lua
pico.layer.empty { key="map", dim={'!', w=10, h=10}, tile={w=8, h=8} }
```

- constructor: tile present -> dim is in TILES (mem.c multiplies).
- `set.scene { tile= }`: independent cheap retile, no texture touch.
- omission = "don't touch" (sticky), like every `set.scene` field.

Considered alternative (NOT supported): embedding the unit-modifier
inside the value it modifies — `dim={w=10, h=10, tile={8,8}}` — and
mirroring it in `set.scene`. Reads well at the call site, but:

- `Dim` would stop being the uniform `{mode, w, h}` shape shared by
  `get.image`, `cv.dim`, `set.scene{dim}`.
- retile via `set.scene{dim=...}` goes through the dim setter, which
  destroys + clears the texture; a same-dim guard would be needed.
- forces sticky-vs-reset and tile-clearing decisions, plus a conflict
  rule if both spellings were accepted.

Keeping A alone preserves today's semantics for all of these and
shrinks the migration (existing `tile=` call sites stay valid).

### Shared validation (all constructors)

- Missing REQ field -> error naming it (`luaL_argcheck` at call
  sites).
- Unknown fields are IGNORED (rejection was implemented, then
  reverted: simpler bindings preferred over typo defense).
- `target` accepted in both attach modes; semantics per
  [260704-layer-target.md] SS3 (stored raw, lazy resolve).
- `screenshot` gains `target` for full parity (applied after
  creation like the others); `region` remains the SOURCE crop —
  distinct fields, both allowed together.

## Implementation

| file | place | change |
| ------------ | ----------------- | ---------------------------------- |
| lua/pico.c | new `L_kw_*` helpers | read/validate fields from table; reject unknown keys |
| lua/pico.c | `l_layer_empty` | table form only; dim-from-target inference; clear/up check |
| lua/pico.c | `l_layer_image/video` | drop nil-insert shuffling; key/mode default rule |
| lua/pico.c | `l_layer_pixmap/text/sub` | table form |
| lua/pico.c | `l_layer_screenshot` | table form |
| lua/pico.c | drop `L_realm_opt`, `L_opt_dst`, `L_is_rect` | subsumed by kw parsing |

C API unchanged (no kwargs in C); C constructor-target DECIDED as
set-after pattern, docs only ([260704-layer-target.md] SS2 option B).

## Migration (no legacy)

All Lua call sites rewritten in the same change:

| area | files |
| ----------- | ------------------------------------------------ |
| tests | `lua/tst/*.lua` (~30 files use `pico.layer.*`) |
| docs | `lua/doc/api.md`, `guide.md`, `doc/*.lua` gifs |
| samples | `logo.lua`, README snippets |
| skills refs | `pico-lua` skill examples (external; flag only) |

Example migration:

```lua
-- before
pico.layer.empty(nil, "bg", true, {'!', w=32, h=32})
-- after
pico.layer.empty { key="bg", dim={'!', w=32, h=32} }
--   (clear=true dropped: was inert on detached)

-- before (rect-as-dim)
pico.layer.empty("world", "bg3", false, r)
-- after
pico.layer.empty { up="world", key="bg3", target=r }
```

## Open Questions

- `text.height` -> `dim: Dim` (approved, PENDING apply): `h`
  required, `w` must be 0/absent (inferred); `'%'`/`'#'` resolve
  against `up` (or current layer if detached).

## Decisions (closed)

- Tile: sibling form A only (embedded `dim.tile` rejected, see above).
- image/video: keep implicit `'='`-when-no-key mode rule.
- `screenshot`: gains `target` now (parity); `region` RENAMED to
  `crop`, `src` RENAMED to `sup` (full unification with
  `sub{sup, crop}`: sub = live view, screenshot = frozen copy).
- `pixmap.pix` RENAMED to `pixels` (matches the C param name).
- Visual test for shot target: draw with no rect into world SE
  quadrant -> new golden `shot-snap-target.png` (user generates).
- `set.*` naming: already aligned (`clear` consistent; `mode` is
  creation-only; `target` unified by this plan). No renames.

## Status

- [x] Decide open questions (all closed)
- [x] `L_kw_*` helpers + validation (check/str/int/bool/realm/tab/
  target; unknown-field and missing-field errors)
- [x] Rewrite 7 constructors + `images` wrapper (init.lua, `sheet=`)
- [x] Test: `shot` with `target` (+ `region` combined)
- [x] Migrate lua/tst (20 files) + tst/todo (3) + doc/*.lua (2) +
  api.md + guide.md
- [x] User runs: `make tests` (C) + `cd lua && make tests` — all pass
