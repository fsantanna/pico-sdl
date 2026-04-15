# Plan: Empty Text Test

Test empty string handling in `pico_get_text` and `pico_output_draw_text`.

## Context

Commits `61a6242` and `bd7dc6c` changed `pico_get_text_mode` and
`pico_output_draw_text_mode` to accept empty strings instead of asserting.
- `pico_get_text_mode`: returns `{0, 0}` for empty text
- `pico_output_draw_text_mode`: returns immediately for empty text

## Tasks

- [x] Create `tst/text_empty.c` with the following test cases:
    1. `pico_get_text("")` abs mode — assert returns `{0, 0}`
    2. `pico_get_text("")` pct mode — assert returns `{0, 0}`
    3. `pico_output_draw_text("")` abs mode — no crash, screen
       stays clear → `_pico_check("text_empty-01")`
    4. `pico_output_draw_text("")` pct mode — no crash, screen
       stays clear → `_pico_check("text_empty-02")`
    5. Draw non-empty text, then empty text over it — original
       text remains → `_pico_check("text_empty-03")`
- [ ] Generate reference images in `tst/out/` for review
- [ ] Copy approved images to `tst/asr/`
- [x] Create `lua/tst/text_empty.lua` — same 5 test cases in Lua
- [ ] Generate Lua reference images in `lua/tst/out/` for review
- [ ] Copy approved Lua images to `lua/tst/asr/`
