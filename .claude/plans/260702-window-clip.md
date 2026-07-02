# window-clip

A clip set on a **parent** layer does not constrain the composite of a
child onto it.
So a `window` clip does not scissor the `world -> window` composite:
the whole child texture is copied, overwriting the window everywhere.

This blocks a clean "reveal" transition (draw an old-screen background
on the window, clip the window to a growing box, let the live `world`
show only inside it).
Downstream: `pingus` fade (`main.atm` `On`) wants exactly this.

## Symptom

In EXPERT mode (which `atmos.env.pico` uses), a parent/window clip is
ignored by the composite.

Repro (pingus `_clip.atm`): green drawn full on `window`, `window`
clip set to a centre box, `world` drawn full red, present.

- expected: green field with a red box in the centre
- actual:   full red (world overwrites the whole window)

Related existing test: `tst/clip-expert.c` already documents a sibling
of this -- its `clip-expert-02` comment says the clip is IGNORED in
expert mode ("full blue -> clip not applied").

## Root cause

The composite path retargets to the parent but never re-applies the
parent's `scene.clip`, unlike every direct-draw path.

```c
// src/layer.c:55  _pico_draw_all_pos  (composite CUR onto UP)
G.layer = UP;
SDL_SetRenderTarget(G.window.ren, UP->tex);   // retarget resets SDL clip
_pico_layer_output(CUR, NULL);                // clip never re-applied
```

Compare the direct-draw paths, which DO re-apply after retargeting:

```c
// src/get-set.c:236  pico_set_layer   (and pico_output_clear)
SDL_SetRenderTarget(G.window.ren, G.layer->tex);
Pico_Abs_Rect r = _pico_abs_rect(G.layer->scene.clip, NULL, NULL);
SDL_RenderSetClipRect(G.window.ren, &r);      // <-- missing in _pico_draw_all_pos
```

`_pico_layer_output` issues the blit with whatever SDL clip is active;
since the retarget cleared it and nobody restored `UP`'s clip, the
child copies in full.

## Plan

- [ ] 1. FIRST: failing test `tst/window-clip.c` (+ `lua` mirror) that
      pins the desired behaviour; confirm it FAILS on current code.
- [ ] 2. Fix `_pico_draw_all_pos` (`src/layer.c`): apply `UP->scene.clip`
      before `_pico_layer_output`.
- [ ] 3. Re-check `tst/clip-expert.c` `clip-expert-02`: does the same
      fix make it pass, or is the expert direct-draw clip a separate
      path? Note the finding.
- [ ] 4. Regen affected baselines; run full suite (C + lua); confirm no
      regressions (default clips are full, so untouched callers are
      byte-identical).

## Failing test (do this first)

New `tst/window-clip.c`, modelled on `tst/clip-expert.c`:

```c
#include "pico.h"
#include "../check.h"

int main (void) {
    Pico_Rel_Rect full = { '%', {0.5,0.5, 1.0,1.0}, PICO_ANCHOR_C };
    Pico_Rel_Rect box  = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C };

    pico_init(1);
    pico_set_window_title("Window Clip");
    pico_set_expert(1, 0);
    pico_set_pencil_style(PICO_STYLE_FILL);

    // GREEN background painted on the WINDOW layer (full)
    pico_set_layer("window");
    pico_set_scene_clip(full);
    pico_set_pencil_color((Pico_Color){0x00,0xFF,0x00,0xFF});
    pico_output_draw_rect(full);

    // clip the WINDOW to a centred box; the composite should honour it
    pico_set_scene_clip(box);

    // WORLD: full red -> should reach the window only inside the box
    pico_set_layer("world");
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00,0xFF});
    pico_output_draw_rect(full);

    pico_output_present(1);
    _pico_check("window-clip-01");
    // want: GREEN field, RED centred box
    // now:  full RED (window clip ignored by the composite)

    pico_init(0);
    return 0;
}
```

- run `make T=window-clip gen` and inspect `tst/out/window-clip-01.png`
  -- it should currently be full red (the bug made visible).
- the `asr/window-clip-01.png` baseline (green field + red box) is the
  desired post-fix image; author/regen it only AFTER the fix, so the
  assertion goes red -> green across the fix.
- mirror as `lua/tst/window-clip.lua`.

## Fix

`src/layer.c` `_pico_draw_all_pos`, add the clip re-apply that every
other retargeting path already has:

```c
static void _pico_draw_all_pos (Pico_Layer* UP, Pico_Layer* CUR) {
    G.layer = UP;
    SDL_SetRenderTarget(G.window.ren, UP->tex);
    Pico_Abs_Rect r = _pico_abs_rect(UP->scene.clip, NULL, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);   // <-- add
    _pico_layer_output(CUR, NULL);
}
```

Once the composite is clipped to `UP`'s box, the child is copied only
inside it; the window keeps its background outside -> the reveal works.

## Notes / risks

- Clip is in the PARENT's coordinate space; fine when parent and child
  share dims (window/world both = window size). Revisit if a child has
  a different `scene.dim`.
- `world` still composites `BLENDMODE_NONE`; irrelevant once clipped --
  outside the box nothing is copied, so alpha never matters.
- Default `scene.clip` is full (`{.5,.5,1,1}`), so applying it for the
  normal (unclipped) composite is a no-op -> existing baselines should
  be unchanged; only tests that set a non-full parent clip move.
- Traverse composites the whole hierarchy; the clip must be set per
  parent visited (it is, since `UP` varies), not once globally.

## Downstream (pingus)

Unblocks a window-based fade: screenshot -> `window` background, grow a
`window` clip, `world` (live screen) reveals through the box.
Cleaner than the current world-clip `Fade` (no drawing the screenshot
into `world`, no snap-before-scene draw-order dependency).
