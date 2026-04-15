# Plan: Universal `'w'` mode

## Context

Currently `'w'` mode exists only for `pico_get_mouse` (line 712),
where it returns raw physical/window pixel coordinates.
The goal is to support `'w'` as a first-class coordinate mode
across all position, dimension, and rect APIs.

This replaces TODO item 26 (`pico_set_target`).
The global `G.tgt` approach is fragile (save/restore discipline)
and doesn't compose.
With `'w'` as a per-struct mode, each operation declares its own
coordinate space — no global state needed.

`'w'` only provides absolute window pixels (like `'!'` but in
window space).
If `'%'` or `'#'` in window space is ever needed, the user can
create a layer matching the window dimensions — that's standard
layer usage, no new API required.
This makes `pico_set_target` unnecessary; TODO item 26 can be
dropped.

## Key Insight

`'w'` is semantically identical to `'!'` (absolute pixels), but
resolves against **window dimensions** instead of layer dimensions.
The difference is in the `r0` base rect, not in the switch math.

In every conversion function, when `base == NULL`:

```c
r0 = { 0, 0,
    (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
    (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
};
```

For `'w'`, we force `r0` to use `S.win.dim` regardless of `G.tgt`.
Then the case math is the same as `'!'`.

## Files to modify

- `src/pico.c` — 5 switch sites + r0 logic
- `src/pico.h` — update mode comment (line 51-54)
- `src/events.h` — already lists `'w'` (no change)

## Implementation

### Step 1: Update mode documentation in `pico.h`

Add `'w'` to the MODES comment block (line 51-56):

```
// 'w': win (physical window pixels)
```

### Step 2: Add `'w'` cases in `src/pico.c`

For each of the 5 functions, two changes:

**a) r0 base override** — at the top of each function, when
`base == NULL` and mode is `'w'`, force window dimensions:

```c
if (base == NULL) {
    if (MODE == 'w') {
        r0 = (SDL_FRect) { 0, 0, S.win.dim.w, S.win.dim.h };
    } else {
        r0 = (SDL_FRect) {
            0, 0,
            (G.tgt == 0) ? S.win.dim.w : S.layer->view.dim.w,
            (G.tgt == 0) ? S.win.dim.h : S.layer->view.dim.h,
        };
    }
}
```

**b) switch case** — add `case 'w':` as fallthrough to `case '!'`:

```c
case 'w':
case '!':
    ...
    break;
```

#### Affected functions (line numbers):

| Function              | Line | r0 check | switch |
| --------------------- | ---- | -------- | ------ |
| `_sdl_dim`            | 195  | 201      | 212    |
| `_sdl_pos`            | 234  | 240      | 250    |
| `_sdl_rect`           | 275  | 282      | 292    |
| `_cv_pos_flt_rel`     | 373  | 375      | 388    |
| `pico_cv_rect_abs_rel`| 419  | 423      | 435    |

### Step 3: Simplify `pico_get_mouse`

The `'w'` branch (line 712) currently skips `_cv_phy_log` and
returns raw `phy.x/y`.
With `'w'` supported in `_cv_pos_flt_rel`, the special case may
be removable — but only if the conversion round-trips correctly.
Verify before removing.

### Step 4: Remove `G.tgt`, add `G.presenting`

`G.tgt` served two roles: coordinate base selection and recursion
guard. With `'w'` mode, the first role is gone. The second remains:
`_show_grid` (called during present) uses draw functions that
trigger auto-present — the guard prevents infinite recursion.

**Changes:**

- Replace `int tgt` with `int presenting` in the `G` struct
  (line 70)
- `pico_set_window` (line 1033): remove `G.tgt=0/1` sandwich,
  pass dim with `'w'` mode to `pico_cv_dim_rel_abs`
- `_pico_output_present` (line 1710): `if (G.presenting) return;`
- `_pico_output_present` (line 1723): `G.presenting = 1;` +
  `SDL_SetRenderTarget(G.ren, NULL);`
- `_pico_output_present` (line 1775): `G.presenting = 0;` +
  `SDL_SetRenderTarget(G.ren, G.main.tex);`
- Init value (line 558): remove `.tgt = 1`
- Remove all 5 `G.tgt` references in r0 base logic (replaced
  by `'w'` mode handling in Step 2)
- `G.main.view.dst` must use `'w'` mode so that
  `pico_cv_rect_rel_abs(&dst, NULL)` at line 1764 resolves
  against window dimensions during present

## Verification

```bash
make tests
```

- Existing tests must pass (no regression)
- Write a test that uses `'w'` mode in a `Pico_Rel_Rect` and
  verifies it resolves against window dimensions
- Test `pico_get_mouse('w', NULL)` still works as before
- Test coordinate conversion: `pico_cv_pos_rel_abs` with `'w'` mode

## TODO

- `'w'` mode for drawing functions
    - Currently `'w'` only works for input (mouse) and
      coordinate conversion
    - Drawing with `'w'` should convert window pixels to layer
      coordinates via the view's src/dst mapping (reverse of
      `_cv_phy_log`), then draw on the main layer
    - The result appears at the correct window position when
      the layer is presented
- Add same tests for lua (`lua/tst/`)

## Status

- [x] Step 1: Update mode docs in `pico.h`
- [x] Step 2: Add `'w'` to 5 functions in `pico.c`
- [x] Step 3: `pico_get_mouse` — keep as-is (phy-to-log
  conversion is not the same as window-base resolution)
- [x] Step 4: Remove `G.tgt`, add `G.presenting`
  - Used explicit window base for `dst` in present (like other
    call sites) instead of changing `dst.mode` to `'w'`
  - Used explicit window base in `pico_set_window` dim conversion
- [x] Verification: tests pass
