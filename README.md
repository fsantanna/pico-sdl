# pico-sdl

[![Tests][badge]][tests]

[badge]: https://github.com/fsantanna/pico-sdl/actions/workflows/tests.yml/badge.svg
[tests]: https://github.com/fsantanna/pico-sdl/actions/workflows/tests.yml

***A 2D graphics library for C and Lua***

[
    [`v0.2`](https://github.com/fsantanna/pico-sdl/tree/v0.2) |
    [`v0.1`](https://github.com/fsantanna/pico-sdl/tree/v0.1)
]

This is the unstable `main` branch.
Please, switch to stable [`v0.2`](https://github.com/fsantanna/pico-sdl/tree/v0.2).
<!--
-->

`pico-sdl` is a C graphics library for developing 2D games and applications.

See also [pico-lua][pico-lua], the official Lua binding for `pico-sdl`.

[pico-lua]: https://github.com/fsantanna/pico-sdl/tree/main/lua

`pico-sdl` is designed around 3 groups of APIs:

- `pico_output_*` for output operations,
    such as drawing shapes and playing audio.
- `pico_input_*` for input events,
    such as waiting time and key presses.
- `pico_get_*` and `pico_set_*` for the library state,
    such as modifying the drawing color, and window title.

The following example draws an `X` on screen gradually with instant feedback:

<picture>
<img align="right" src="x.gif">
</picture>

```
#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_size (
        (Pico_Dim) {160,160},   // physical screen size
        (Pico_Dim) {16,16}      //  logical screen size (10x10 pixel logical size)
    );
    for (int i=0; i<16; i++) {
        pico_output_draw_pixel((Pico_Pos) {i, i});
        pico_output_draw_pixel((Pico_Pos) {15-i, i});
        pico_input_delay(100);
    }
    pico_input_delay(1000);
    pico_init(0);
    return 0;
}
```

`pico-sdl` targets educational use, being guided by the following principles:

- Standardized APIs, as described above
- Immediate display through single-buffer rendering
- Sensible default settings, such as initial colors and a built-in font
- Runtime visual aids, such as a grid for logical pixels and zoom & scroll support
- Prioritizing simplicity over flexibility

In particular, as the example above illustrates, immediate display allows that
students view the result of their operations at any point, in real time.

`pico-sdl` is implemented as a [facade][1] over [SDL][2], which is a flexible
and lower level API.
This allows programmers to fallback to SDL whenever required.

`pico-sdl` is inspired by a number of graphics programming environments:

- [BASIC][3]: immediate display
- [Logo][4]: simplicity, immediate display
- [PICO-8][5]: simplicity, self containment

[1]: https://en.wikipedia.org/wiki/Facade_pattern
[2]: https://en.wikipedia.org/wiki/Simple_DirectMedia_Layer
[3]: https://en.wikipedia.org/wiki/BASIC
[4]: https://en.wikipedia.org/wiki/Logo_(programming_language)
[5]: https://en.wikipedia.org/wiki/PICO-8

# Install & Execute

## Install

### Linux

Install SDL:

```
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev
```

Clone `pico-sdl`:

```
git clone https://github.com/fsantanna/pico-sdl
cd pico-sdl/
```

### Windows

Download [pico-sdl-win64.zip]() from the releases page and extract it.

Run pico-sdl-ide.ps1 with powershell.

## Execute

### Linux
```
./pico-sdl tst/main.c
```

### Windows

You can hit F5 to compile and run the active file.

You can also open vscode terminal to use pico-sdl-cmd:
```
pico-sdl-cmd {path to your program}
```

# Testing

## Running Tests

```bash
make tests
```

For headless environments (CI/CD, servers without displays), use Xvfb:

```bash
xvfb-run make tests
```

### Visual Regression Testing

Tests use `check.h` for pixel-perfect screenshot comparison.

**Modes** (controlled by environment variables):
- `PICO_CHECK_INT` - Interactive mode (pause for visual inspection, default: enabled)
- `PICO_CHECK_ASR` - Assert mode (compare against expected images, default: disabled)

**Examples:**

```bash
# Interactive mode (default) - pause for visual inspection
./pico-sdl tst/anchor_pct.c

# Generate mode - write to out/ without pause or assertion
PICO_CHECK_INT= ./pico-sdl tst/anchor_pct.c
# Then manually copy verified images: cp tst/out/*.png tst/asr/

# Assert mode - compare against expected images (for CI/CD)
PICO_CHECK_INT= PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c

# Interactive with assertion - pause AND assert
PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c

# Headless testing (for CI/CD)
PICO_CHECK_INT= PICO_CHECK_ASR=1 xvfb-run ./pico-sdl tst/anchor_pct.c
```

# Documentation

You can read the documentation [here](https://pgvalle.github.io/pico-sdl/html/index.html)
