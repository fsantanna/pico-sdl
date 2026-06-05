# 06-05 shot blank — explicit cascade clear

## Context

`pico_output_screenshot("world")` (and `pico_layer_screenshot(..,"world",..)`)
returns a blank image in expert mode.

Root cause:
`pico_set_expert` sets `world.scene.clear=1`, and during `pico_output_present`
the post-composite clear in `_pico_layer_draw_all` (src/layer.c:47-53) wipes
`world.tex` right after compositing it onto the window.
A screenshot taken after `present()` reads the just-blanked `world.tex`.
The `window` shot works because it reads the framebuffer composite.

Chosen fix (option E):
remove the implicit per-present auto-clear entirely and make the explicit
`pico_output_clear()` cascade down the hierarchy, clearing every descendant
flagged `scene.clear=1`.
Nothing wipes `world.tex` behind the user's back, so a post-present shot is
always valid until the next explicit `clear()`.

New meaning of `scene.clear=1`:
"clear me when an ancestor's `pico_output_clear()` cascades down".
It no longer triggers any implicit wipe.

This is a breaking behavior change (accepted): expert-mode loops that never
call `clear()` now accumulate frames.
Only `logo.lua` does this today; it gets an explicit `clear()`.

## Changes

### 1. Remove implicit post-composite clear

`src/layer.c` — `_pico_layer_draw_all` (~47-53)
Delete the whole `if (CUR->scene.clear) { ... RenderClear ... }` block.
Layers are no longer wiped during `present()`.

### 2. Cascade clear in `pico_output_clear`

`src/output.c` — `pico_output_clear` (17-25) plus a new static helper.

- Keep clearing the current layer over its clip via `SDL_RenderFillRect`
  with `G.layer->effect.color` (unchanged).
- Add a recursive helper that walks the subtree of `G.layer` and, for each
  descendant with `scene.clear==1`, sets it as render target and full-clears
  it with that layer's own `effect.color`.
  Reuse the child-walk idiom from `_pico_layer_draw_all`
  (`hier.dn.fst` / `hier.nxt` + `realm_get`); recurse into every child so
  flagged grandchildren are reached.
- Disable the clip (`SDL_RenderSetClipRect(NULL)`) around the child clears
  so each child clears fully, mirroring the old auto-clear `RenderClear`.
- After the cascade, restore render target + clip to `G.layer`, using the
  same restore pattern as `_pico_shot` (src/output.c:337-339).
- Keep the trailing `_pico_output_present(0)`.

### 3. Expert mode stops touching clear

`src/get-set.c` — `pico_set_expert` (~227)
Remove the line `G.world.scene.clear = on ? 1 : 0;`.
Leave `G.world.effect.grid = 0;` as-is.
`world.scene.clear` then stays at its init value `0`; world is cleared only by
an explicit `pico_output_clear()` on it (which clears the target directly,
regardless of the flag).

### 4. Drop the color-change implicit clear

`src/get-set.c` — `pico_set_effect_color` (~268-274)
Remove the `if (L->scene.clear && L->hier.up!=NULL) { ... RenderClear ... }`
block.
It belonged to the old implicit-clear mechanism; with E, clearing is explicit.
Safe for current tests — every test that sets a clear-layer's color follows it
with an explicit `pico_output_clear()` (e.g. tst/shot.c "empty layer" and
"scene round-trip").

### 5. Fix the one program relying on implicit clear

`logo.lua` — game loop top (~62)
Add `pico.output.clear()` as the first statement inside `while true do`.

### 6. Docs

`src/pico.h`
- `pico_output_clear` (302-303): note it clears the current layer and cascades
  to descendants with `scene.clear=1`.
- `pico_layer_empty` / `pico_layer_empty_mode` `clear` param (483, 494):
  redefine as "cascade-clear flag: cleared when an ancestor cascades".
- `pico_set_scene_clear` (711): add a one-line doc with the same meaning.

Optionally mirror the wording in `lua/doc/` where `clear` / `expert` are
described (low priority).

## Out of scope / notes

- `src/pico.c` is NOT modified, so `valgrind.supp`'s `src:pico.c:N` line is
  unchanged.
- `pico_set_scene_clear` assertions (no clear on world/window/sub) stay.
- Detached layers (`up==NULL`, e.g. tst/shot.c `empty1`) are not in any
  parent's subtree, so the cascade never reaches them — unchanged behavior.

## Files

| file          | place                              | change                         |
| ------------- | ---------------------------------- | ------------------------------ |
| src/layer.c   | `_pico_layer_draw_all` ~47-53      | remove post-composite clear    |
| src/output.c  | `pico_output_clear` + new helper   | recursive cascade clear        |
| src/get-set.c | `pico_set_expert` ~227             | remove world clear assignment  |
| src/get-set.c | `pico_set_effect_color` ~268       | remove color-change clear      |
| src/pico.h    | clear-related docstrings           | document cascade semantics     |
| logo.lua      | loop top ~62                       | add explicit clear             |

## Verification

- `make test T=shot`
  - non-expert "world layer" block: still passes.
  - new expert "world layer (expert auto-clear)" block: now passes
    (present no longer blanks `world.tex`; shot matches `asr/shot-world.png`).
- `make tests` and `cd lua/ && make tests`: confirm no regressions from
  removing the implicit clear / color-change clear.
- Run `logo.lua` (expert loop) and confirm it renders without accumulation
  after adding the explicit `clear()`.
