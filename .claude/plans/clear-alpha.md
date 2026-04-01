# Plan: `pico_set_color_clear_alpha`

Support alpha in the clear color, primarily useful for layers.

## Summary

- Add `pico_set_color_clear_alpha(Pico_Color_A color)` to the C API
- Change internal storage of clear color from `Pico_Color` to
  `Pico_Color_A`
- `pico_set_color_clear(Pico_Color)` stays, sets alpha to 255
- `pico_output_clear` uses the stored alpha instead of hardcoded
  `0xFF`
- Add getter `pico_get_color_clear_alpha(void)` returning
  `Pico_Color_A`
- Existing `pico_get_color_clear(void)` stays, returns `Pico_Color`
  (drops alpha)
- Push/pop saves and restores the full `Pico_Color_A`
- Lua bindings: add `pico.set.color.clear_alpha(color_a)` accepting
  a table with alpha

## Changes

### 1. `src/pico.c` — internal state

- [ ] `Pico_State.color.clear`: change type `Pico_Color` →
  `Pico_Color_A`
- [ ] `S.color.clear` default: ensure initialized to
  `{0, 0, 0, 255}` (black, opaque)

### 2. `src/pico.c` — set functions

- [ ] `pico_set_color_clear(Pico_Color color)`: convert to
  `Pico_Color_A` with `a=255`, store in `S.color.clear`
- [ ] Add `pico_set_color_clear_alpha(Pico_Color_A color)`: store
  directly in `S.color.clear`

### 3. `src/pico.c` — get functions

- [ ] `pico_get_color_clear(void)`: extract `Pico_Color` from
  `S.color.clear` (drop alpha), return it
- [ ] Add `pico_get_color_clear_alpha(void)`: return `S.color.clear`
  as `Pico_Color_A`

### 4. `src/pico.c` — `pico_output_clear`

- [ ] Line 1338: replace hardcoded `0xFF` with `S.color.clear.a`

### 5. `src/pico.c` — push/pop

- [ ] Already stores `S.color.clear` — type change propagates
  automatically since `Pico_State.color.clear` becomes
  `Pico_Color_A`

### 6. `src/pico.h` — declarations

- [ ] Add `void pico_set_color_clear_alpha(Pico_Color_A color);`
- [ ] Add `Pico_Color_A pico_get_color_clear_alpha(void);`

### 7. `lua/pico.c` — Lua bindings

- [ ] Modify `l_set_color_clear`: duck type the argument
    - String → `c_color_s` → `pico_set_color_clear` (no alpha)
    - Table without `a` field → `c_color_t` →
      `pico_set_color_clear` (no alpha)
    - Table with `a` field → `c_color_a_t` →
      `pico_set_color_clear_alpha` (with alpha)
- [ ] No new Lua-side function or registration needed

### 8. Tests

- [ ] Add test for `pico_set_color_clear_alpha` (C or Lua)
- [ ] Verify layers with transparent clear composite correctly

## Notes

- Automatic clears in `pico_init` and `pico_set_layer` use
  `pico_output_clear`, which reads `S.color.clear`. Since the
  default alpha is 255, these remain opaque — no special handling
  needed.
- `pico_set_color_clear` resets alpha to 255, preventing stale
  alpha from a previous `pico_set_color_clear_alpha` call.
