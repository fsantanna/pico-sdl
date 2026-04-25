# TODO

1. Support `'transparent'` as color string
    - `PICO_COLOR_TRANSPARENT` is `Pico_Color_A` (4 bytes)
    - `c_color_s` casting to `Pico_Color*` reads only `{0,0,0}`
    - Needs RGBA consumer — `set.color.draw` and `set.color.clear` use RGB only

2. Guide: use input `dt` in animations
    - Animation examples should use `dt` from `pico.input.event`
    - Currently uses fixed increments

3. Guide: `pico.get.now` never introduced
    - Used in section 9.2 but never formally introduced

4. Support percentage mode for alpha
    - `pico.set.alpha('%', 0.5)` — normalized 0.0–1.0
    - Currently only raw 0–255

5. Document realm hash table / resource management
    - The realm hash table (Lua-side resource registry) is undocumented
    - Unclear how resources are managed / when they are released
    - Document in HISTORY.md
    - Document in `lua/doc/guide.md`

6. Group all drawing state under `pico.set.draw.*`
    - Move color, style, alpha, font under `pico.set.draw`
    - Consider `pico.draw { ... }` block with auto push/pop

7. Review and complete guide
    - [#103](https://github.com/fsantanna/pico-sdl/issues/103)

8. `pico.set` vs multi-arg setters
    - `set` dispatch calls `field(v)` with single argument
    - Multi-arg setters (`expert`, `video`) lose extra arguments
    - See `.claude/plans/set.md`
    - [#102](https://github.com/fsantanna/pico-sdl/issues/102)

9. README pico-lua cross image
    - [#101](https://github.com/fsantanna/pico-sdl/issues/101)

10. Support screenshots of layers
    - [#98](https://github.com/fsantanna/pico-sdl/issues/98)

11. Non-blocking video I/O
    - See `.claude/plans/select.md`
    - [#93](https://github.com/fsantanna/pico-sdl/issues/93)

12. Help aid
    - [#72](https://github.com/fsantanna/pico-sdl/issues/72)

13. Expert mode should disable all aids
    - [#71](https://github.com/fsantanna/pico-sdl/issues/71)

14. Fullscreen vs accelerated rendering
    - Redraw does not work in fullscreen + accelerated
    - Send example `bug.c` to SDL mailing list
    - [#65](https://github.com/fsantanna/pico-sdl/issues/65)

15. Create a logo for the project
    - [#46](https://github.com/fsantanna/pico-sdl/issues/46)

16. ThorVG integration
    - Replace SDL2_gfx (and later SDL2_ttf) with ThorVG
    - SwCanvas renders to ARGB8888, uploaded to SDL_Texture
    - Adds SVG, anti-aliasing, gradients, bezier, stroke, transforms
    - Phase 2: thickness / stroke width for drawing primitives
      ([#62](https://github.com/fsantanna/pico-sdl/issues/62))
    - See `.claude/plans/thorvg.md` (4 phases)

17. Networking support (`pico-sdl-net`)
    - SDL3 needed due to blocking calls in SDL2_net

18. Layout system
    - Dispatch mechanism
    - Click handling

19. Default DejaVu font
    - Consider replacing Tiny font
    - Decide default font size
    - Predefined font options (tiny, dejavu, etc)
    - Test other fonts (done — see `done/2026-04-dejavu-font.md`)

21. `pico_get_image/text` should accept ref parameter
    - See `lua/tst/image_pct` commented tests

23. Layer scene graph (up-layer)
    - **BLOCKER**: realm frees keys on remove
      (`src/realm.hc:74`), so borrowed id pointers in
      `up`/`dn`/`nxt` dangle — `realm_get` would `memcmp`
      freed memory.
      Need id interning, refcount, arena, or eager detach.
      See `.claude/plans/up-layer.md` §10.
    - Explicit `up` (parent) id at create; realm-as-sole-owner
    - Auto-composite traversal in `pico_output_present`
    - Free-mode `output.draw.layer` for detached layers only
    - String-keyed `pico.mouse.get/set(layer)` and `pico.vs(l1, l2)`
    - Post-composite clear with per-type `view.keep` defaults
    - Closes #22 (`'w'` vs `r` divergence)
    - Depends on `sub-layer-parent.md` (eliminates `Pico_Layer.parent`)
    - See `.claude/plans/up-layer.md`, `.claude/plans/sub-layer-parent.md`

24. Consider splitting `pico_set_view` into separate setters
    - Currently an all-in-one with 9 optional params
    - Alternatives: `pico_set_alpha`, `pico_set_target`, etc.
    - See item 6 (`pico.set.draw.*` group)

22. `'w'` vs `r`-relative divergence near edges
    - `pico_output_draw_pixel({'w', ...})` snaps through screen log grid
      (5 win px / log px) while collision via `up = &r` is continuous
    - Near edges of a distorted `r`, render and collision disagree
    - Repro: `tst/mouse-w-click.c` (mouse `'w' (400, 382)`)
    - See `.claude/plans/w-bug.md` open follow-ups for fix options
