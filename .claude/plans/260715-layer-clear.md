# Plan: layer-clear

Fix layers showing textures from old (popped) layers after a scene
is re-created (push/pop + re-create).

## Diagnosis

Two defects combine to produce the symptom:

1. Fresh textures are never cleared.
   `_pico_tex_create` (`src/pico.c`) creates
   `SDL_TEXTUREACCESS_TARGET` textures whose contents SDL leaves
   undefined.
   Nothing wipes them at creation
   (`_pico_mem_alloc_layer_empty`, `pico_set_scene`,
   `pico_set_scene_dim`).
   After `pico_pop` destroys the old scene's textures, accelerated
   drivers recycle that GPU memory into the re-created scene's
   textures, so new layers inherit old layers' pixels.
   The software renderer (`PICO_TESTS=1`) zero-fills surfaces,
   which hides the bug under `make tests`.

2. `pico_output_clear` with transparent color is a no-op.
   `src/output.c` fills with `SDL_RenderFillRect` under the global
   `SDL_BLENDMODE_BLEND` (`src/pico.c` init), so filling with the
   default `PICO_COLOR_TRANSPARENT` (commit 33038c2) blends nothing.
   The other clear paths (`_pico_output_clear_pre` cascade,
   `pico_set_effect_color` repaint) use `SDL_RenderClear`, which
   overwrites — the direct path is inconsistent.

## Items

- [x] Diagnose old-texture leak
- [x] Failing test: `tst/clear-transp.c`
      (deterministic, transparent clear no-op)
- [x] Repro test: `tst/push-pop-recycle.c`
      (user scenario; fails only on accelerated renderers)
- [x] Fix `_pico_tex_create`: clear new texture to transparent
      (save/restore render target)
- [x] Update `valgrind.supp` `sdl-init` line: 99 -> 108
      (`SDL_Init` shifted by clear block; helper now in `layer.c`)
- [x] Simplify `_pico_tex_create`: whole wipe + restore guarded by
      `if (G.init)` (init textures are opaque and fully repainted, so
      they skip it); restore via `_pico_layer_target(G.layer)`; trusts
      "caller sets color"; removed now-redundant `_pico_layer_target(L)`
      in `pico_set_scene` and `pico_set_scene_dim`
- [x] Refactor: extract `_pico_layer_target(Pico_Layer*)` in
      `layer.c` (target + clip dance) and replace the 8 duplicated
      sites (`output.c`, `get-set.c`, `layer.c`); `-Wall -Werror`
- [x] Fix `pico_output_clear`: wrap `RenderFillRect` in
      `SDL_BLENDMODE_NONE`, restore `SDL_BLENDMODE_BLEND`
- [ ] User runs tests to confirm both pass
- [x] Add `tst/clear-transp.c` to Makefile `tests` list

## Won't do

- Merge the two tests into one: they guard different fixes with
  different determinism.
  `clear-transp` fails under the software renderer (CI guard);
  `push-pop-recycle` passes there even when broken (SDL zero-fills
  surfaces) and only reproduces on an accelerated renderer, so it
  stays a manual GPU repro, out of the `tests` list.

## Notes

- Run repro on real hardware (accelerated renderer):
  `./pico-sdl tst/push-pop-recycle.c`
- Deterministic test: `make test T=clear-transp`
