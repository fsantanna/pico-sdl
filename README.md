# pico-sdl

`pico-sdl` is a C graphics library for developing 2D games and applications.

`pico-sdl` is designed around 3 groups of APIs:

- `pico_output_*` for output operations,
    such as drawing shapes and playing audio.
- `pico_input_*` for input events,
    such as waiting time and key presses.
- `pico_get_*` and `pico_set_*` for the library state,
    such as modifying the drawing color, and window title.

The following example draws an `X` slowly on screen with instant feedback:

<picture>
<img align="right" src="x.gif">
</picture>

```
#include <pico.h>

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

# TODO

```
gcc -o main.exe tst/main.c src/pico.c src/hash.c -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
./main.exe
```
