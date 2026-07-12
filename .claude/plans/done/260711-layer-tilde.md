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

## Task 1: realm fingerprints + text dyn/fix — DONE (untested)

Final design (2026-07-11), after iterating through: eq callback
→ sibling `"\0"+key` fp entry → **fp bytes stored IN the realm
entry** (opaque bytes, realm never interprets them — consistent
with "realm assumes nothing"):

```c
void* realm_put (realm_t* r, int mode, int n, const void** key,
                 int fn, const void* fp,
                 realm_free_t free, realm_alloc_t alloc, void* ctx);
// single function: fn=0/fp=NULL = no fingerprint (old behavior)
```

Semantics when the key exists (fn==0 = no fingerprint = today):

| mode  | fp equal        | fp differs             | no fp          |
|-------|-----------------|------------------------|----------------|
| `'~'` | return existing | replace + new fp       | replace (today)|
| `'='` | return existing | **assert fail**        | return (today) |
| `'!'` | return NULL     | return NULL            | return NULL    |

Text fp = the content string `/text/<font>/<h>/<r.g.b>/<text>`
(already built for the auto-key; now built always and passed as
fp — auto-key case is trivially equal).

API: mode stays optional; two blessed shorthands (dyn/fix):

- `pico_output_draw_text_fix(text, rect)` = `('=', NULL)` —
  immutable value, shared by content, do NOT feed varying text
- `pico_output_draw_text_dyn(key, text, rect)` = `('~', key)` —
  mutable slot, re-raster only on change
- plain `pico_output_draw_text` REMOVED; `_mode` kept underneath
- Lua: `pico.output.draw.text.fix/dyn` (text became a subtable)
- `pico_layer_text_mode('~')` gets the optimization for free
  (fp lives in `_pico_layer_text`)

Steps done:

- [x] `realm.hc`: `fn/fp` in entry, `realm_fp_set/eq`, fp freed
      with entry; single `realm_put` gains `fn, fp` params
      (no `_fp` wrapper — all ~10 call sites pass `0, NULL`)
- [x] hardening after review (2026-07-12): `'='` mismatch
      returns NULL (soft, like `'!'`); `'~'` is
      acquire-then-commit (failure = entry untouched);
      `realm_fp_set(int* dfn, void** dfp, ..)` reused by both
      paths; NULL alloc result → cleanup + return NULL (never
      stored: would be ambiguous with `realm_get` "absent")
- [x] `_pico_layer_text`: always build fp, pass to `realm_put_fp`
- [x] `output.c`: `_fix`/`_dyn` wrappers; plain removed
- [x] `pico.h`: decls + docs (fix warns about varying text)
- [x] `lua/pico.c`: `l_output_draw_text_fix/dyn`, nested reg
- [x] sweep `tst/*.c` (10 files), `lua/tst/*.lua` (12 files),
      `lua/doc/guide.md`, `layer.c` internal calls → `fix`
- [x] compiles clean (`-Wall -Werror`: pico.c layer.c output.c,
      lua/pico.c syntax)
- [x] caveat from `mem.c:92`: `'~'` replace asserts no children;
      fp-hit path sidesteps it — documented in the
      `_pico_mem_free_layer` comment (2026-07-12)
- [x] test: `tst/text-dyn.c` (white-box via `_pico.h`) — dyn
      hit/miss by text and color, layer `'~'` API, fix auto-key
      identity; added to Makefile tests
- [x] test: `lua/tst/text-dyn.lua` (visual via `pico.check`) —
      dyn hit/change/color, fix; added to lua Makefile
- [x] run: both suites + `text-dyn` C and Lua — all pass
      (2026-07-12)
      caveat: C color-change check asserts pointer inequality —
      could in theory false-fail on SDL pointer reuse
- fp adopters (pixmap/image/sub/empty) — WON'T DO (2026-07-12):
  text was the motivating case; others adopt on demand
- [x] verify: `make tests` + `cd lua && make tests` — all pass
      (2026-07-12, after realm hardening + goto unwind)

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
- [x] upstreamed: `realm-allocator` main == local `realm.hc`
      (verified 2026-07-12, modulo header-guard name)
- [x] `make realm` pin bumped v0.1 → v0.2 (`Makefile:23`)
- [x] v0.2 tag cut upstream; verified `make realm` v0.2 ==
      local `realm.hc` (modulo header guard) — 2026-07-12
- [x] Makefile: `src/%.o` deps now include `src/*.hc`
- [x] verify: `make tests` — all pass (2026-07-11)

## API surface

- BREAKING: `pico_output_draw_text` → `_fix`; Lua `draw.text(`
  → `draw.text.fix(` (all in-repo callers swept)
- NEW: `pico_output_draw_text_dyn` / Lua `draw.text.dyn`
- `'~'` with equal fingerprint returns the existing layer
  (observable via texture pointer identity)
- `'='` with differing fingerprint asserts

## Consumer (FrescoGO) after this lands

`tela.atm` can call, per frame, with a stable per-widget id:

```atm
pico.output.draw.text.dyn("/mmss", mmss, rs.mmss)
```

or, when a persistent layer is wanted:

```atm
pico.layer.text [mode='~', key="/mmss", dim=['%', h=rs.mmss.h], text=mmss]
pico.output.draw.layer("/mmss", rs.mmss)
```

both are re-raster-on-change now; no app-side change-cache
needed (Task 2 dropped: `draw.text.fix` of varying strings
remains accumulating — the `fix` name warns about it).

## Pending / open

- [x] does color live in pencil at creation only? (yes: baked at
      raster time) — compare must use the CURRENT pencil color
      vs the stored one (folded into Task 1 eq impl)
- [x] `dim` resolution for detached text layers: `'%'` with no
      base resolves against the CURRENT layer (`geom.c:381`,
      `G.layer->scene.dim`) — window only when current layer is
      the window. verified 2026-07-12
- [x] `draw.layer(key, rect)` with `rect.w=0`: works —
      `_pico_layer_output` passes the layer's native dim as
      aspect ratio (`layer.c:254`, `_f_rat`): `w=0` → from `h`
      by native aspect; `w=h=0` → full native. not buggy;
      recorded here as a FrescoGO consumer prerequisite.
      verified 2026-07-12
