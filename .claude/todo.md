# TODO

## Accept colors as strings in buffer and other calls

Allow color strings (e.g., `'red'`, `'blue'`) in buffer operations and other
functions that accept colors, not just `{r, g, b}` tables.

## Accept empty text string

`pico_output_draw_text("")` currently asserts. Fix to ignore empty strings
instead of failing.

## Fonts should be resources

Fonts are always opened and closed (or remain open forever?). They should become
resources with keys like `"/font/tiny/10"` to be GC'ed via the hash table TTL.
