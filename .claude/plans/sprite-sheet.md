# Plan: Sprite Sheet

## Context

Sprite sheet support — load one image, create multiple sub-layers
each showing a sub-region, sharing the parent's texture.

1. **C**: `pico_layer_sub(name, parent, crop)` — sub-layer from any
   layer, chaining supported (sub of sub)
2. **Lua**: `pico.layer.images(path, t)` — polymorphic wrapper
   (explicit name→rect + grid auto-split), returns list of names
3. **Enum**: `PICO_LAYER_SUB` for type discrimination (replaces
   `parent != NULL` checks)

## Status

- [x] C core (parent field, resolve chain, sub creation, guards)
- [x] Lua bindings (`l_layer_sub`, `M.layer.images`)
- [x] Test file `lua/tst/sheet.lua`
- [x] `PICO_LAYER_SUB` enum + 5 discrimination checks replaced
- [ ] Manual testing & verification
- [ ] Commit / Push / PR
