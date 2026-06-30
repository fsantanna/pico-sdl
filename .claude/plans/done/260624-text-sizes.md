# text-sizes

New visual test `tst/text-sizes.c`.

## Goal

- 500x500 world/window
- 10x "Hello World!" text output
- 1 per line
- small to big

## Steps

- [x] Create `tst/text-sizes.c`
    - `pico_init(1)`
    - window + world scene dim 500x500
    - loop 10 lines, NW anchor, growing height
    - stack each line below the previous

## Notes

- Follow `tst/font.c` / `tst/colors.c` style.
- Abs `'!'` rects for text.
