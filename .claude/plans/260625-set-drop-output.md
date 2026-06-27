# set : drop unneeded _pico_output_present

Audit every setter in `src/get-set.c` and drop each trailing
`_pico_output_present(0)` that cannot change the framebuffer.

## Criterion

`_pico_output_present(0)` exists for PLAIN-mode live refresh:
mirror the current layer to the framebuffer so a change shows up
without an explicit draw/present.

It is needed only when the setter alters how *already-rendered*
content composites or appears.
It is redundant when the setter:

- only configures state used by *future* draws (no current pixel
  changes), or
- changes window chrome that is not part of the framebuffer, or
- targets a non-root layer, where `_pico_output_present(0)` already
  early-returns at `output.c:255`.

## Candidates

| setter                     | verdict   | reason                          |
|----------------------------|-----------|---------------------------------|
| `pico_set_effect_alpha`    | keep      | layer compositing               |
| `pico_set_effect_color`    | keep      | background / clear color        |
| `pico_set_effect_flip`     | keep      | compositing                     |
| `pico_set_effect_grid`     | keep      | compositing                     |
| `pico_set_effect_rotate`   | keep      | compositing                     |
| `pico_set_scene`           | keep      | recreates tex + clears          |
| `pico_set_scene_clip`      | keep      | decided (sibling symmetry)      |
| `pico_set_scene_dim`       | keep      | window resize branch            |
| `pico_set_scene_dst`       | keep      | compositing geometry            |
| `pico_set_scene_src`       | evaluate  | child-only; present may no-op   |
| `pico_set_scene_tile`      | evaluate  | child-only; present may no-op   |
| `pico_set_window_fs`       | keep      | mode change repaints            |
| `pico_set_window_show`     | keep      | first paint on show             |
| `pico_set_window_title`    | drop?     | window chrome, not framebuffer  |

## Steps

- [ ] Confirm each `evaluate` / `drop?` row against the criterion.
- [ ] Remove the redundant `_pico_output_present(0)` calls.
- [ ] Confirm C and Lua test suites still pass.
