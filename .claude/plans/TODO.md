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

4. Create `extra/` directory
    - Move `check.h` from `tst/`
    - Video generator (yet to come)

5. Support percentage mode for alpha
    - `pico.set.alpha('%', 0.5)` — normalized 0.0–1.0
    - Currently only raw 0–255

6. Rename alpha to transparency (invert values)
    - SDL convention: 255=opaque, 0=transparent
    - Invert so 0=opaque, 255=fully transparent

7. Add ttl-GC to history and guide
    - Document TTL-based garbage collection in HISTORY.md
    - Document in `lua/doc/guide.md`

8. Group all drawing state under `pico.set.draw.*`
    - Move color, style, alpha, font under `pico.set.draw`
    - Consider `pico.draw { ... }` block with auto push/pop

9. `pico_set_view` present in non-expert mode
    - Already calls `_pico_output_present(0)`
    - Pros: navigation works without input loop
    - Cons: flicker on setup, multiple presents per logical setup
    - Guide: why pixels in 5.3 need clear? Why 6.1 doesn't work?

10. Review and complete guide
    - [#103](https://github.com/fsantanna/pico-sdl/issues/103)

11. `pico.set` vs multi-arg setters
    - `set` dispatch calls `field(v)` with single argument
    - Multi-arg setters (`expert`, `video`) lose extra arguments
    - See `.claude/plans/set.md`
    - [#102](https://github.com/fsantanna/pico-sdl/issues/102)

12. README pico-lua cross image
    - [#101](https://github.com/fsantanna/pico-sdl/issues/101)

13. Command-line options
    - [#100](https://github.com/fsantanna/pico-sdl/issues/100)

14. Support screenshots of layers
    - [#98](https://github.com/fsantanna/pico-sdl/issues/98)

15. Non-blocking video I/O
    - See `.claude/plans/select.md`
    - [#93](https://github.com/fsantanna/pico-sdl/issues/93)

16. Help aid
    - [#72](https://github.com/fsantanna/pico-sdl/issues/72)

17. Expert mode should disable all aids
    - [#71](https://github.com/fsantanna/pico-sdl/issues/71)

18. Fullscreen vs accelerated rendering
    - Redraw does not work in fullscreen + accelerated
    - Send example `bug.c` to SDL mailing list
    - [#65](https://github.com/fsantanna/pico-sdl/issues/65)

19. Thickness for drawing primitives
    - Overlaps with ThorVG Phase 2 (stroke width)
    - [#62](https://github.com/fsantanna/pico-sdl/issues/62)

20. Create a logo for the project
    - [#46](https://github.com/fsantanna/pico-sdl/issues/46)

21. Run manual tests for v0.3
    - Interactive C tests and guide examples
    - See `.claude/plans/release.md` step 2b

22. Announce v0.3
    - Twitter, students, SDL lists, Lua lists
    - See `.claude/plans/release.md` step 10

23. ThorVG integration
    - Replace SDL2_gfx (and later SDL2_ttf) with ThorVG
    - SwCanvas renders to ARGB8888, uploaded to SDL_Texture
    - Adds SVG, anti-aliasing, gradients, bezier, stroke, transforms
    - See `.claude/plans/thorvg.md` (4 phases)

24. Networking support (`pico-sdl-net`)
    - SDL3 needed due to blocking calls in SDL2_net

25. Layout system
    - Dispatch mechanism
    - Click handling

26. ~~`pico_set_target(LOG/PHY)`~~ DONE via `'w'` mode
    - `'w'` mode added to all coordinate APIs (win-mode-w.md)
    - `G.tgt` removed, replaced by `G.presenting` (guard only)
    - For `'%'`/`'#'` in window space: create a matching layer

27. Default DejaVu font
    - Consider replacing Tiny font
    - Decide default font size
    - Predefined font options (tiny, dejavu, etc)
    - Test other fonts

28. Unify panels, boxes, images, and text as layers
    - Images are also layers
    - Rotation: angle/anchor
    - Crop, flip, dimensions unified across types
    - Path identifies type; text uses string + font

29. `pico_get_image/text` should accept ref parameter
    - See `lua/tst/image_pct` commented tests
