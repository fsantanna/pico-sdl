# w-bug: `'w'` Pos round-trip drift

## Symptom

`tst/mouse-rect-click.c` and the reduced `tst/x.c` show that a pixel
drawn with `pico_output_draw_pixel(&{'w',{418,392},PICO_ANCHOR_C,NULL})`
does not land at window pixel `(418,392)`.
In `tst/out/mouse-rect-click-05.png` the drawn pixel's NW corner is at
`(420,390)` — a `(+2,-2)` offset in image coordinates
(roughly `(+1,-1)` in logical window coordinates if the captured PNG
is at 2x scale).

The `pico_set_mouse` / `pico_get_mouse` round trip is internally
self-consistent in `'%'` space (the assertions still pass), but the
visual pixel drawn at the same `'w'` coordinate ends up offset.
This means there is a disagreement between two consumers of the same
`'w'` coordinate — most likely a precision loss in
`win <-> wld` conversion.

## Suspected root cause

`src/aux.hc:71-89` — `_pos_win_to_wld` and `_pos_wld_to_win`:

```c
static SDL_FPoint _pos_win_to_wld (SDL_FPoint win) {
    Pico_Abs_Rect dst = pico_cv_rect_rel_abs (
        &S.layer->view.dst, &(Pico_Abs_Rect){0, 0, S.win.dim.w, S.win.dim.h}
    );
    Pico_Abs_Rect src = pico_cv_rect_rel_abs(&S.layer->view.src, NULL);
    float rx = (win.x - dst.x) / (float)dst.w;
    float ry = (win.y - dst.y) / (float)dst.h;
    return (SDL_FPoint) { src.x + rx*src.w, src.y + ry*src.h };
}
```

Both helpers go through `pico_cv_rect_rel_abs`, which truncates
`view.dst` / `view.src` to integer pixel rects via `_abs_rect`.
Once `dst` is integer-truncated, the round trip
`win -> wld -> win` is no longer the identity: it can drift by up to
one logical pixel per leg.

For a `'w'` `Pico_Rel_Pos` the path is:

1. `_sdl_pos` `'w'` case (`aux.hc:170-176`): `win -> wld`
2. `pico_cv_pos_rel_win` (`pico.c:155-159`): `wld -> win`

That's two legs, so a `(+/-1, +/-1)` integer drift is plausible.

## Secondary issues found in the same area

| # | File / line | Issue |
|---|---|---|
| 1 | `src/pico.c:1232`        | Stray `printf(">>> ...")` debug in `pico_output_draw_pixel`. |
| 2 | `src/pico.c:1234`        | Comment uses `/` instead of `//` (does this even compile?). The TODO about `4.5->4` is also stale: `_abs_pos` already does `floorf(f+0.5f)`, so 4.5 -> 5. |
| 3 | `src/aux.hc:170-188`     | `_sdl_pos` `'w'` and `'%'` Pos cases subtract `pos->anchor.{x,y}` (a fraction in `{0,0.5,1}`) as if it were a pixel offset. For a Pos there is no extent, so this is meaningless. Sub-pixel error only — not the cause of the visible offset, but should be fixed. |
| 4 | `src/pico.c:466,477`     | `pico_get_mouse` and `pico_set_mouse` overwrite the caller's anchor with `PICO_ANCHOR_NW`. Matches the TODO comment in commit `f06682c`. |

## Minimal round-trip repro

A test that demonstrates the bug without buttons, layers, or
rendering — pure `'w'` <-> `win` round trip on the screen layer:

```c
#include "pico.h"
#include <stdio.h>
#include <assert.h>

int main (void) {
    pico_init(1);

    /* A 'w' position with arbitrary integer coords. */
    Pico_Rel_Pos in = { 'w', {418, 392}, PICO_ANCHOR_NW, NULL };

    /* rel -> win  (what set_mouse / draw_pixel use) */
    SDL_Point win = pico_cv_pos_rel_win(&in, NULL);
    printf("rel(418,392) -> win(%d,%d)\n", win.x, win.y);

    /* win -> rel  (what get_mouse uses) */
    Pico_Rel_Pos out = { 'w', {0,0}, PICO_ANCHOR_NW, NULL };
    pico_cv_pos_win_rel(win, &out, NULL);
    printf("win(%d,%d) -> rel(%g,%g)\n", win.x, win.y, out.x, out.y);

    /* Identity expected. */
    assert(win.x == 418 && win.y == 392);
    assert(out.x == 418 && out.y == 392);

    pico_init(0);
    return 0;
}
```

Expected output if conversions are lossless:
```
rel(418,392) -> win(418,392)
win(418,392) -> rel(418,392)
```

Predicted actual output: at least one leg drifts by 1 px because
`_pos_win_to_wld` / `_pos_wld_to_win` go through integer-truncated
`Pico_Abs_Rect dst/src`.

How to run (for the user, not me):
```bash
./pico-sdl tst/w-bug.c
```

## Fix sketch (not yet applied)

1. Make `_pos_win_to_wld` / `_pos_wld_to_win` stay in `float` space.
   Use `_sdl_rect`-style `SDL_FRect` for `dst` and `src` instead of
   `pico_cv_rect_rel_abs` -> `Pico_Abs_Rect`.
2. Remove the stray `printf` and fix the broken `/` comment in
   `pico_output_draw_pixel`.
3. Decide on anchor semantics for a bare `Pico_Rel_Pos`:
    - Option A: ignore anchor entirely for Pos.
    - Option B: anchor refers to the unit cell `(1,1)`,
      so `C` => subtract `0.5` *only if* drawing rounds toward NW.
    Either way, the current literal-pixel subtraction in `_sdl_pos`
    `'w'` and `'%'` Pos cases must change.
4. Drop the `// TODO` anchor overwrite in `pico_set_mouse` /
   `pico_get_mouse` once #3 is decided.

## Checklist

- [ ] Add `tst/w-bug.c` from the snippet above.
- [ ] Run it; record actual output.
- [ ] Fix `_pos_win_to_wld` / `_pos_wld_to_win` to use float rects.
- [ ] Re-run `tst/w-bug.c`; assert identity.
- [ ] Re-run `tst/mouse-rect-click.c`; regenerate `tst/asr/`
      and confirm it matches `/tmp/asr/`.
- [ ] Remove debug `printf` and fix comment in
      `pico_output_draw_pixel`.
- [ ] Resolve anchor-on-Pos semantics (`_sdl_pos` `'w'` / `'%'`).
- [ ] Drop anchor overwrite in `pico_set_mouse` / `pico_get_mouse`.
