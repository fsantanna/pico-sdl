# 2026-05 layer-utils

Add an optional **layer reference** to coordinate-conversion, hit-test,
and mouse helpers, so callers can compute against a named layer
without first switching the current render target.

## Goal

Today these helpers implicitly use `S.layer` (the current target) for
view-derived inputs (`tile`, `dst`, `clip`, `dim`).
Make the layer choice explicit and overridable, NULL = current.

## Affected functions (15)

### `pico_cv_*` (11)

| function           | reads from layer view |
|--------------------|-----------------------|
| `dim_rel_abs`      | tile, dim             |
| `dim_abs_rel`      | tile, dim             |
| `dim_rel_rel`      | tile, dim             |
| `pos_rel_abs`      | tile, dim             |
| `pos_abs_rel`      | tile, dim             |
| `pos_rel_rel`      | tile, dim             |
| `pos_rel_win`      | dst, dim              |
| `pos_win_rel`      | dst, src, dim         |
| `rect_rel_abs`     | tile, clip, dim       |
| `rect_abs_rel`     | tile, dim             |
| `rect_rel_rel`     | tile, dim             |

### `pico_vs_*` (2)

| function     | currently |
|--------------|-----------|
| `pos_rect`   | hardcodes `base = NULL` |
| `rect_rect`  | hardcodes `base = NULL` |

### Mouse (2)

| function          | reason |
|-------------------|--------|
| `pico_get_mouse`  | window → rel uses layer dst/dim/tile |
| `pico_set_mouse`  | rel → window symmetric to get        |

## C API design

First param `const char* layer`, NULL = current layer.
Matches the existing convention in `pico_get_view`, `pico_set_show_*`,
`pico_get_draw`, etc.

```c
Pico_Abs_Rect pico_cv_rect_rel_abs (
    const char*          layer,        // NEW: NULL = current
    const Pico_Rel_Rect* rect,
    Pico_Abs_Rect*       base          // NULL = layer's view dim
);

int pico_vs_pos_rect (
    const char*    layer,              // NEW
    Pico_Rel_Pos*  pos,
    Pico_Rel_Rect* rect
);

Pico_Mouse pico_get_mouse (
    const char*    layer,              // NEW
    char           mode,
    Pico_Rel_Rect* rect
);

void pico_set_mouse (
    const char*    layer,              // NEW
    Pico_Rel_Pos*  pos
);
```

`base` and `layer` are **orthogonal**.
`layer` selects whose view feeds the transform.
`base` overrides only the absolute reference rectangle.

## Lua API design

Use type / mode-alphabet sniffing to stay backward compatible.

### `pico.cv.*`, `pico.vs.*`

If first arg is a string and **not** a Pico_Rel table, treat as layer.

```lua
pico.vs.pos_rect(pos, rect)            -- current layer
pico.vs.pos_rect("hud", pos, rect)     -- explicit layer
pico.cv.rect(r1)                       -- current layer
pico.cv.rect("hud", r1, r2)            -- explicit layer
```

### `pico.get.mouse`, `pico.set.mouse`

Mode is a single char from a fixed set (`!`, `%`, `#`, `w`).
If first string arg is **not** in that set, treat as layer.

```lua
pico.get.mouse('%')                    -- current
pico.get.mouse('hud', '%')             -- explicit layer
pico.get.mouse('%', rect)              -- target rect
pico.get.mouse('hud', '%', rect)       -- both
pico.set.mouse(pos)
pico.set.mouse('hud', pos)
```

If modes are ever extended, keep the dispatcher's mode set explicit.

## Tradeoff considered

Alt: keep current signatures, add `_in` variants
(`pico_vs_pos_rect_in`, etc.).
Mirrors the `_mode` variant pattern in `pico_layer_*` /
`pico_output_draw_text_mode`.

| approach          | pro                            | con                              |
|-------------------|--------------------------------|----------------------------------|
| first-arg NULL    | one symbol, matches state APIs | every C call site adds `NULL,`   |
| `_in` variant     | zero churn at call sites       | doubles symbol count, drift risk |

**Pick:** first-arg NULL.
Long-term it is the API the rest of the codebase already uses for
layer-targeted functions.

## Migration

1. Update C signatures in `src/pico.h` and `src/pico.c`
   for the 15 functions listed above.
2. Internal helpers: replace implicit `S.layer` with the resolved
   layer argument; keep NULL → `S.layer` resolution centralized.
3. Sweep call sites under `src/`, `tst/`, `lua/`, `app/`,
   prepending `NULL,` where current behavior is intended.
4. Update Lua wrappers in `lua/pico.c` (`l_cv_*`, `l_vs_*`,
   `l_get_mouse`, `l_set_mouse`) with sniffing dispatch.
5. Add visual regression tests with a non-current layer ref to
   verify the layer argument actually changes the result.
6. Update doxygen `@param layer` lines and `@sa` cross-refs.
7. Update the user guide section that documents these helpers.

## Open questions

- Should `pos`/`rect` `up` chain inherit the resolved layer when
  it is set, or stay independent?
  Current proposal: independent (layer arg only affects view
  resolution, not `up` traversal).
- Should `pico_set_mouse` accept a layer if `pos` already carries
  an `up` chain that points at a layer-derived rect?
  Current proposal: yes, for symmetry with `get`.

## Backlog (out-of-scope follow-ups)

- Introduce a `Mode` type in `lua/doc/api.md`:
    - **Mode**: `'!'` pixels | `'%'` percentage | `'#'` tiles | `'w'` window
- Reuse `Mode` across Pos / Rect / Dim / Mouse type definitions
  (currently each inlines its own subset; Pos/Rect/Dim wrongly omit `'w'`).
- Then mark per-operation exceptions where a narrower set applies
  (e.g. layer creators use `'!'|'%'|'#'|'='`, Color uses `'!'|'%'`).
- Once `layer` arg lands, reword `(omit for current layer)` clauses to
  disambiguate "current layer" vs "layer arg".

## Progress

- [ ] C signatures updated
- [ ] internal helpers thread `layer` through
- [ ] call-site sweep
- [ ] Lua wrappers updated
- [ ] tests
- [ ] docs
