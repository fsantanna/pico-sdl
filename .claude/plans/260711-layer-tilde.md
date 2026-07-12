# `'~'` text layers: skip re-raster when content is unchanged

## Context

Found while porting FrescoGO (`/x/x/frescogo/atmos`), which draws
varying texts every frame (clock `mm:ss`, counters, live km/h,
names being typed).

Two related problems with text textures in the realm:

1. `pico.output.draw.text` uses realm mode `'='` with an auto-key
   that embeds the CONTENT
   (`/text/<font>/<h>/<r.g.b>/<text>`, `output.c:190`,
   `layer.c:_pico_layer_text`).
   Every distinct string becomes a cached layer that lives until
   `realm_close` — varying texts accumulate textures for the
   whole run (a clock leaks one texture per second shown).
2. The documented workaround — explicit `key` + mode `'~'` via
   `pico.layer.text [mode='~', key=..]` — re-rasterizes on EVERY
   call, even when the text did not change: `realm_put` `'~'` is
   content-blind (`realm.hc`, key-exists branch):

   ```c
   case '~': {
       void* nv = alloc(n, e->key, ctx);   // new texture FIRST
       e->free(e->n, e->key, e->value);    // old freed after
       e->value = nv;
   ```

   `alloc` = `_pico_mem_alloc_layer_text` = TTF render +
   `SDL_CreateTexture`, so per-frame `'~'` = raster + upload +
   free at frame rate.

Verified: the realm has NO automatic reclamation — scopes are
only `pico_init` (base, `pico.c:84`), manual `pico.push/pop`
(`pico.c:249/254`), and `realm_close` at shutdown;
`realm_leave` frees by scope depth, never by age/usage.

## Goal

Make the naive patterns cheap and leak-free:

- per-frame `pico.layer.text [mode='~', key=..]` with unchanged
  text costs ~nothing (cache hit);
- (optional, separate) plain `draw.text` of varying strings does
  not accumulate textures forever.

## Task 1: realm `eq` callback (decided design)

Content-awareness lives in the REALM, not in `layer.c`, via an
optional equality callback passed to `realm_put`:

```c
typedef int (*realm_eq)(int n, const void* key, void* value, void* ctx);
```

Semantics per mode when the key exists:

| mode  | eq given, equal | eq given, differs      | eq NULL        |
|-------|-----------------|------------------------|----------------|
| `'~'` | return existing | replace (today)        | replace (today)|
| `'='` | return existing | **assert fail**        | return (today) |
| `'!'` | return NULL     | return NULL            | return NULL    |

- `'~'` becomes "replace unless equal": unchanged text is a
  cache hit — no alloc, no free, no raster.
- `'='` with `eq` asserts content matches — turns today's
  silent stale hit (e.g. `draw_pixmap` new pixels on existing
  key, `output.c:138`) into a caught bug pointing to `'~'`.
- Rejected alternatives: compare in `layer.c` before `realm_put`
  (2 lookups, duplicated per call site); realm-stored flat
  fingerprint bytes (no callback, but forces flat identity).

Steps:

- [ ] `realm.hc`: add `realm_eq eq` param to `realm_put`;
      consult it in `'~'` (keep-if-equal) and `'='` (assert)
      key-exists branches
- [ ] update all `realm_put` call sites (~11) to pass `NULL`
- [ ] extend `Pico_Layer` (text variant) to store `text` copy,
      `height`, `font` path copy, `color`
      (color baked at raster time — compare vs CURRENT pencil)
- [ ] free the stored copies in `_pico_mem_free_layer`
- [ ] `_pico_layer_text`: pass an `eq` impl comparing stored
      `(text, height, font, color)` vs request + current pencil
- [ ] caveat from `mem.c:92`: `'~'` replace asserts no children;
      the eq-hit path sidesteps the assert (good — document)
- [ ] later: `eq` impls for pixmap (pixels) and image (path)
- [ ] test: `tst/` case calling `layer.text ['~']` twice with
      same/different text and asserting texture identity/change
      (e.g. via `pico.get` on the layer or a counter hook)

## Task 2: ~~LRU sweep for `'='` auto-keys~~ — WON'T DO

No GC in the realm — decided 2026-07-11.
Varying text must use an explicit key + `'~'` (Task 1 makes it
cheap). Plain `draw.text` of varying strings keeps accumulating;
document the pattern in `api.md` instead.

## Task 3: realm owns unique COUNTER (refactor) — DONE

Final design (revised 2026-07-11): realm must assume NOTHING
about keys (generic `(n, bytes)` store), so it owns only the
counter; pico keeps the `/unique/N` string FORMAT:

- [x] `realm_t.unique` field + `realm_unique(r)` in `realm.hc`
- [x] `pico_unique()` delegates to `realm_unique(G.realm)` —
      single source, no `/unique/N` collisions
- [x] `_key_unique` stays in `layer.c` (string format is pico's
      convention, not realm's)
- rejected: `realm_put` generating on `*key==NULL` — implemented
  then reverted; it leaked string assumptions (`snprintf`,
  `strlen`, namespace) into the generic realm
- rejected: raw int `n` as key — layer names round-trip through
  the public API as C strings (`strlen`-based lookups, Lua)
- note: `pico_unique()` now requires `pico_init` (uses `G.realm`)
- [ ] verify: `make tests` (user runs)

## API surface

Public pico API: no signature changes.
Internal: `realm_put` gains the `eq` param (realm.hc is
internal). Behavior change only:

- `'~'` with equal content returns the existing layer
  (observable via texture pointer identity; document in api.md
  under `pico.layer.text`).
- `'='` with `eq` given asserts content equality.

## Consumer (FrescoGO) after this lands

`tela.atm` can call, per frame, with a stable per-widget id:

```atm
pico.layer.text [mode='~', key="/mmss", dim=['%', h=rs.mmss.h], text=mmss]
pico.output.draw.layer("/mmss", rs.mmss)
```

no app-side change-cache needed (Task 2 dropped: plain
`draw.text` of varying strings remains accumulating — use the
keyed `'~'` pattern above).

## Pending / open

- [x] does color live in pencil at creation only? (yes: baked at
      raster time) — compare must use the CURRENT pencil color
      vs the stored one (folded into Task 1 eq impl)
- [ ] `dim` resolution for detached text layers: confirm `'%'`
      `h` resolves against window when `up` is absent
- [ ] `draw.layer(key, rect)` with `rect.w=0`: confirm it infers
      native width like `draw.text` does (FrescoGO relies on it)
