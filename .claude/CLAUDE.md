# CLAUDE.md

- In `valgrind.supp`, last clause `sdl-init`, line `src:pico.c:N`.
    - `src:pico.c:N` points to file `src/pico.c` at line `N`
    - `N` is the line in `pico_init` that calls `SDL_Init`
    - `N` must be updated automatically whenever `src/pico.c` changes

## Build and Run Commands

### C Programs

Compile and run a C program:

```bash
./pico-sdl path/to/program.c
```

This script:

- Compiles the program with `src/pico.c`
- Links against SDL2 libraries:
  `-lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx`
- Runs the executable from the program's directory

Manual compilation:

```bash
gcc -Wall -g -o output program.c src/pico.c \
    -I src \
    -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx
```

### Lua Bindings

The `lua/` directory contains Lua bindings for pico-sdl.

Run a Lua program:

```bash
pico-lua path/to/program.lua
```

Build Lua native module manually:

```bash
gcc -shared -o pico_native.so -fPIC src/pico.c lua/pico.c \
    -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image
```

### Documentation

Build API documentation (requires Doxygen <= 1.9.4):

```bash
cd doc
./build.sh
```

## Testing

### Running Tests

```bash
make tests
```

For headless environments (CI/CD, servers without displays), use Xvfb:

```bash
xvfb-run make tests
```

### Visual Regression Testing

Visual tests use `check.h` to compare rendered output against expected images.

**Compilation modes** (can be combined):

- No defines - Generate mode: writes to `out/`, no pause, no assert
- `-DPICO_CHECK_INT` - Interactive: pauses for visual inspection
- `-DPICO_CHECK_ASR` - Assert: compares against expected images in `asr/`
- Both defines - Interactive with assertion: pause AND assert

**Directory structure:**
- `tst/asr/` - Expected (reference) images (manually maintained)
- `tst/out/` - Generated output images (auto-created during tests)

**Naming convention:**
- Format: `filename-XX.png` (e.g., `anchor_pct-01.png`, `anchor-02.png`)
- `filename` matches the test file name (or a common prefix for shared tests)
- `XX` is the sequential test number (01, 02, 03, ...)

**Workflow:**

1. **Developing new tests** (generate mode):
   ```bash
   PICO_CHECK_INT= ./pico-sdl tst/newtest.c
   # Review images in tst/out/
   # If correct, manually copy to asr/:
   cp tst/out/newtest-*.png tst/asr/
   ```

2. **Interactive development** (default):
   ```bash
   ./pico-sdl tst/anchor_pct.c  # Pauses for visual inspection
   ```

3. **Automated testing** (CI/CD):
   ```bash
   PICO_CHECK_INT= PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c
   make tests  # uses ASR mode
   ```

4. **Interactive with validation**:
   ```bash
   PICO_CHECK_ASR=1 ./pico-sdl tst/anchor_pct.c
   # Pauses AND asserts against asr/ files
   ```

**Headless Testing:**

Xvfb (X Virtual Frame Buffer) provides a virtual display server for headless
environments:
- Performs full graphical rendering in memory
- Produces real pixel data for screenshot comparison
- Essential for CI/CD pipelines and automated testing

```bash
xvfb-run ./pico-sdl tst/anchor_pct.c
make tests  # automatically uses xvfb and ASR mode
```

**In test code:**

```c
#include "pico.h"
#include "../check.h"

int main(void) {
    pico_init(1);
    pico_output_clear();

    // Using percentage mode
    Pico_Rel_Rect r = { '%', {0.5, 0.5, 0.4, 0.4}, PICO_ANCHOR_C, NULL };
    pico_output_draw_rect(&r);
    _pico_check("anchor_pct-01");  // Writes to out/, compares with asr/

    pico_init(0);
    return 0;
}
```

### Valgrind

- `valgrind.supp` contains the `sdl-init` suppression that references a
  specific line in `src/pico.c` (the `SDL_Init` call in `pico_init`).
- Whenever that line number changes, update the `src:pico.c:<line>` reference
  in `valgrind.supp` accordingly.
