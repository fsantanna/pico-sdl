# clip-expert : Lua port

Port the C visual test `tst/clip-expert.c` to Lua under
`lua/tst/clip-expert.lua`.

The test shows that `scene.clip` scissors `draw.rect` in PLAIN mode
(check 01) but is IGNORED in EXPERT mode (check 02 comes out full
blue instead of red + centered blue box).

## Mapping

| C                              | Lua                              |
|--------------------------------|----------------------------------|
| `pico_set_pencil_style(FILL)`  | `pico.set.pencil { style='fill' }` |
| `pico_set_scene_clip(box)`     | `pico.set.scene { clip=box }`    |
| `pico_set_expert(1, 0)`        | `pico.set.expert(true, 0)`       |
| `pico_output_present(1)`       | `pico.output.present()`          |
| `_pico_check("clip-expert-N")` | `pico.check("clip-expert-N")`    |

## Steps

- [x] Read `tst/clip-expert.c`.
- [x] Create `lua/tst/clip-expert.lua` mirroring the C sequence.
- [x] Fix EXPERT clip bug in `src/get-set.c:pico_set_scene_clip`:
      apply target + `SDL_RenderSetClipRect` eagerly (kept trailing
      `_pico_output_present(0)`).
- [ ] Generate baselines: `cd lua && make gen T=clip-expert`.
- [ ] Inspect `lua/tst/out/clip-expert-*.png` for the EXPERT bug.

## Root cause

`pico_set_scene_clip` only updated the field and relied on
`_pico_output_present(0)` to restore the clip rect.
PLAIN mode presents fully (clip restored), but EXPERT mode
early-returns at `output.c:253`, so the clip was never applied.

## Pending

- TODO: drop all unneeded `_pico_output_present(0)` (and other
  redundant calls) across `src/get-set.c` setters.
