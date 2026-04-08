# up-layer

Make layer parent (`up`) explicit instead of inferred from `current`
at draw time.

## Idea

1. Layer carries an `up` pointer (parent layer).
    - Set at **create** time
    - Parent is an **explicit argument** to the create call
      (not captured implicitly from `current`)

2. On draw, assert `current == up`.
    - Enforces lexical parent: a layer can only be drawn inside
      the same parent it was created in
    - Catches misuse where a layer is drawn under a different
      parent, which would silently distort coords / clipping

3. `pico.get.mouse` / `pico.set.mouse` accept a layer pointer.
    - Use the passed layer's `up` chain to map window ↔ layer
      coordinates
    - Removes hidden dependency on whatever `current` happens
      to be at the call site
    - Allows mouse queries outside the draw block of that layer

## Rationale

- Hierarchy becomes explicit data, not call-order state
- Coordinate mapping for mouse no longer aliases `current`
- Related to TODO #22 (`'w'` vs `r`-relative divergence near
  edges) — an explicit hierarchy is the right substrate for a
  consistent window↔layer mapping
- See `.claude/plans/w-bug.md`

## Open questions

- Top-level layers: `up == NULL` or `up == screen`?
- Does the assert fire for nested draw of the same layer
  (re-entrancy)?
- Migration: every existing layer create call site must pass
  a parent — audit C and Lua APIs

## Pending

- [ ] Decide top-level `up` convention
- [ ] Update layer create signature (C + Lua)
- [ ] Add `up` field + assert in draw
- [ ] Update `get/set.mouse` to take layer ptr
- [ ] Audit call sites and tests
- [ ] Revisit `w-bug.md` once hierarchy is explicit
