# TODO

## rot-flip test failing on CI

The `rot-flip.c` test passes locally but fails on GitHub CI due to pixel-perfect
comparison with different graphics drivers.

**Problem:**
- CI uses `ubuntu-latest` with software Mesa rendering
- Local machine uses GPU driver
- Rotation rendering produces slightly different pixels

**Options:**

1. **Use CI-generated images** — Download `test-screenshots` artifact from failed
   workflow and copy to `tst/asr/`

2. **Add tolerance** — Modify `_pico_cmp_files` in `check.h` to compare
   pixel-by-pixel with a threshold instead of byte-exact `memcmp`

3. **Skip rotation tests on CI** — Not recommended

## Accept colors as strings in buffer and other calls

Allow color strings (e.g., `'red'`, `'blue'`) in buffer operations and other
functions that accept colors, not just `{r, g, b}` tables.

## Accept empty text string

`pico_output_draw_text("")` currently asserts. Fix to ignore empty strings
instead of failing.

## Fonts should be resources

Fonts are always opened and closed (or remain open forever?). They should become
resources with keys like `"/font/tiny/10"` to be GC'ed via the hash table TTL.
