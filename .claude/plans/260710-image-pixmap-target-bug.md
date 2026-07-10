# `image`/`pixmap` layers silently drop children onto the grandparent

## Context

Follow-up to `260709-layer-up-dim.md`. That plan diagnosed the
`:quadro` (`up=image`) child-misplacement bug as "aspect distortion,
not a bug." Offscreen testing (below) found the real cause: it's a
render-target bug, not a `%`-resolution/aspect issue.

## Repro (offscreen, no display needed)

```c
// world 100x100
pico_layer_pixmap("world", "quadro", (Pico_Abs_Dim){40,10}, gray); // 40x10 dim
pico_set_layer("quadro");
pico_set_scene_dst((Pico_Rel_Rect){'%', {0.75,0.5,0.3,0.3}, PICO_ANCHOR_C}); // box (60,35)-(90,65)

pico_layer_pixmap("quadro", "child", (Pico_Abs_Dim){2,2}, red);
pico_set_layer("child");
pico_set_scene_dst((Pico_Rel_Rect){'%', {0.5,0.5,0.2,0.2}, PICO_ANCHOR_C}); // centered square

pico_output_present(1);
```

Run headless: `xvfb-run -a ./a.out` (link against `src/libpico-sdl.a`,
built with `PICO_TESTS=1` to force `SDL_RENDERER_SOFTWARE`), then read
back pixels with `SDL_RenderReadPixels` / `pico_output_screenshot`.

**Expected**: red lands inside quadro's box, world-log
`~(72,47)-(78,53)`.
**Observed**: red lands at world-log `(16,4)-(24,6)` — `child`'s raw
dst-in-quadro pixel coords, dumped straight onto **world**, outside
quadro's box entirely. A direct screenshot of `quadro`'s own texture
shows no red at all — the child was never actually drawn onto it.

## Root cause

- `_pico_draw_all_pos` (`layer.c`, composite-walk callback) does
  `SDL_SetRenderTarget(ren, UP->tex)` before drawing a child onto
  `UP`, with **no return-value check**.
- `pixmap` layers (`mem.c:159`, `SDL_CreateTextureFromSurface`),
  `image` layers (`mem.c:217`, `IMG_LoadTexture`), and `shot` layers
  (`mem.c:174`, `mem.c:273`) all get `SDL_TEXTUREACCESS_STATIC`
  textures — SDL only allows `SDL_SetRenderTarget` on
  `SDL_TEXTUREACCESS_TARGET` textures. Only `empty` layers (and
  window/world) go through `_pico_tex_create` (`pico.c:52`), which
  requests `TARGET` access.
- So `SDL_SetRenderTarget` silently fails and the renderer stays
  pointed at whatever was active one level up (the grandparent).
  Any child attached to an `image`/`pixmap`/`shot` parent (`up=`)
  gets baked onto the **grandparent's** canvas, at the parent's raw
  internal pixel coordinates reinterpreted as grandparent
  coordinates.

## Fix applied (pushed to `claude/latest-plan-review-vaddh0`)

`src/layer.c`, `_pico_draw_all_pos` only:

```c
static void _pico_draw_all_pos (Pico_Layer* UP, Pico_Layer* CUR) {
    G.layer = UP;
    pico_assert_0(SDL_SetRenderTarget(G.window.ren, UP->tex));
    ...
```

Turns the silent misplacement into an immediate, clear abort:
`SDL error: Texture not created with SDL_TEXTUREACCESS_TARGET`.

**Did not** add the same assert to the sibling `_pico_draw_all_pre`
(which primes `CUR`'s own texture as target before recursing into
*its* children) — that call is a harmless no-op for any leaf layer
(no children of its own), and asserting there broke plain
`image`/`pixmap` leaf layers and `pico_layer_sub` crops (which share
their source's static texture) in `layer-hier.c`. `_pico_draw_all_pos`
alone already catches the real bug, at the point it actually matters
(a layer being drawn *into*).

Net diff: 2 commits, ~6 lines (`0236c23` then correction `c72d28f`).

## What this fix does *not* do

`image`/`pixmap`/`shot` layers still **cannot** host children — the
assert just makes that limitation loud instead of silently wrong.
Making them capable of hosting children (so the `up=:quadro` pattern
from `260709-layer-up-dim.md` works as originally intended) would
need those layers' content copied/rendered into a `TARGET`-capable
texture instead of the surface-derived static one. Not attempted here
— separate, larger change (also revives the aspect-ratio question
from `260709-layer-up-dim.md` §"Idiomatic pattern," which becomes
relevant again once children-of-image actually render).

## Test suite status

`make tests` (full, under `xvfb-run` + valgrind) fails on
`tst/expert_fps.c:61` (`assert(dt2>=25 && dt2<=30)`), reproducibly, in
both a pre-fix and post-fix full run. This is **pre-existing,
unrelated flakiness**:
- `expert_fps.c` only exercises `pico_set_expert`/`pico_get_expert`/
  `pico_input_event` real-time frame-pacing; no layer hierarchy, no
  `image`/`pixmap` parenting.
- Passes cleanly standalone (`./pico-sdl tst/expert_fps.c`, exit 0).
- The assert is a 25-30ms wall-clock window under valgrind (10-50x
  slowdown) stacked after dozens of prior instrumented tests — a
  fragile thing to assert in this sandboxed container regardless of
  the change under test.

Not filed as a TODO item yet — flag if it recurs.

## Pending / next steps

- [ ] Decide whether `image`/`pixmap`-as-parent should become a
      supported pattern (needs a `TARGET`-capable render path) or
      stay documented-unsupported (update `260709-layer-up-dim.md`'s
      "Idiomatic pattern" section + user-facing docs to say
      "`image`/`pixmap` layers cannot have children," not "aspect
      distortion warning").
- [ ] If supported: revisit the aspect-ratio question from
      `260709-layer-up-dim.md` — once children actually composite
      through an `image` parent, does `%` size/tile math still need
      decoupling from the parent's raw pixel `dim` vs. its displayed
      `dst` box? (See prior conversation: position is unaffected by
      dim/dst aspect mismatch — per-axis stretch cancels dim out of
      final position math — but *size* of a "square" child is not:
      `final = fraction * dst`, so a fraction-of-dim square renders
      at `dst`'s aspect, not 1:1, whenever `dim` and `dst` aspects
      differ.)
- [ ] Optionally file `tst/expert_fps.c` timing-flakiness as its own
      TODO item if it keeps failing under full-suite valgrind runs.
