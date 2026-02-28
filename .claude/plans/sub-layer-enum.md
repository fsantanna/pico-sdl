# Plan: sub-layer-enum

## Goal

Add `PICO_LAYER_SUB` to the `PICO_LAYER` enum so that layer type
discrimination uses `type` consistently, replacing `parent != NULL`
checks. The `parent` pointer remains for data access only.

## Changes — DONE

### 1. Add enum value — `src/pico.c:54`
### 2. Set `.type = PICO_LAYER_SUB` — `src/pico.c:1255`
### 3. Replace 5 discrimination checks — lines 583, 926, 1235, 1612, 1665
### 4. Update struct comment — `src/pico.c:62`
### 5. Update valgrind suppression — `valgrind.supp:97`

## Status

- [x] All edits applied
- [ ] Test
- [ ] Commit / Push / PR
