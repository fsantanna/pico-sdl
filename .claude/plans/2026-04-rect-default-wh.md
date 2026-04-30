# Rect/Dim default w=h=0 in Lua bindings

## Status: DONE (binding + tests landed; doc cleanup pending)

## Goal

Allow Lua callers to omit `w` and/or `h` from `Rect` tables passed
to `pico.set.view { target = ... }`. Missing fields default to 0,
which the C side already interprets as "infer from layer aspect ratio"
via `_pico_output_draw_layer` (`src/layers.hc:184`) and the `_f3`
helper (`src/aux.hc:5-17`).

## Scope

Audit determined that 0 is only safe where the C consumer passes a
non-NULL `ratio` to `_sdl_rect`/`_sdl_dim` (i.e. has access to an
intrinsic dim from a layer/image/text/video). All such sites already
had `L_dim_default_wh` except `l_set_view target`. The broader
`C_optfieldnum` refactor was abandoned because absolute-dim consumers
(`set.view.dim`, `set.window.dim`, pixmap VLA, etc.) would silently
create degenerate resources with w/h=0.

## Done

- [x] Audited C consumers; identified `l_set_view target` as the only
      missing-but-safe site.
- [x] `lua/pico.c` `l_set_view`: added `L_dim_default_wh` call in the
      `target` branch.
- [x] `tst/view-target.c`: scenarios 05/06/07 cover w-only, h-only,
      both-zero on a non-square (80x40) layer.
- [x] `lua/tst/view-target.lua`: same scenarios mirrored, using
      omitted-field syntax.
- [x] `make tests` green (C and Lua).

## Pending follow-ups

- [ ] Revert `lua/doc/guide.md` and `lua/tst/guide.lua` 7.4
      hello/world targets to omit `w` (cleaner doc demonstrating the
      new ergonomics).
