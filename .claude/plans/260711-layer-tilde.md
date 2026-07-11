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

## Task 1: content-compare on `'~'` text layers

In `_pico_layer_text` (`layer.c:104`), before the `realm_put`:

- `realm_get` the key; if a layer exists AND it is a TEXT layer
  AND its `(text, height, font, color)` match the request,
  return it untouched (no alloc, no free).
- Otherwise fall through to the current `'~'` replace.

Requires the text layer to remember its inputs:

- [ ] extend `Pico_Layer` (text variant) to store `text` copy,
      `height`, `font` path copy, `color`
      (some may already be derivable; check `scene.dim` vs
      `height`)
- [ ] free the stored copies in `_pico_mem_free_layer`
- [ ] compare in `_pico_layer_text` for mode `'~'` only
      (`'='` auto-key already embeds content in the key;
      `'!'` semantics unchanged)
- [ ] caveat from `mem.c:92`: `'~'` replace asserts no children;
      the compare-hit path sidesteps the assert (good — document)
- [ ] test: `tst/` case calling `layer.text ['~']` twice with
      same/different text and asserting texture identity/change
      (e.g. via `pico.get` on the layer or a counter hook)

## Task 2 (optional/follow-up): LRU sweep for `'='` auto-keys

Fixes unkeyed varying `draw.text` without API change:

- [ ] stamp realm entries with `gen = G.frame` on every `'='`
      hit of a `/text/`-prefixed key
- [ ] `pico_output_present` increments `G.frame`; periodically
      (e.g. 1x/s) sweep `/text/` entries with `gen` older than
      K frames
- [ ] never touch user-named layers (explicit `key` outside the
      reserved `/text/` namespace)
- [ ] texts drawn every frame stay cached (zero re-raster);
      intermittent texts (blink) re-raster on reappearing —
      acceptable

Note: auto push/pop per frame is NOT an alternative — it would
re-rasterize everything every frame and destroy user layers.

## API surface

No signature changes. Behavior change only:

- `'~'` with equal content returns the existing layer
  (observable via texture pointer identity; document in api.md
  under `pico.layer.text`).

## Consumer (FrescoGO) after this lands

`tela.atm` can call, per frame, with a stable per-widget id:

```atm
pico.layer.text [mode='~', key="/mmss", dim=['%', h=rs.mmss.h], text=mmss]
pico.output.draw.layer("/mmss", rs.mmss)
```

no app-side change-cache needed; with Task 2, even plain
`draw.text` becomes safe for varying strings.

## Pending / open

- [ ] does color live in pencil at creation only? (yes: baked at
      raster time) — compare must use the CURRENT pencil color
      vs the stored one
- [ ] `dim` resolution for detached text layers: confirm `'%'`
      `h` resolves against window when `up` is absent
- [ ] `draw.layer(key, rect)` with `rect.w=0`: confirm it infers
      native width like `draw.text` does (FrescoGO relies on it)
