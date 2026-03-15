# Plan: Embedded Fonts in SDL

## Reference

How to embed a font directly into the executable (no external
file needed) using SDL2_ttf.

## Technique

### Step 1: Convert font to C array

Use `xxd` to convert a TTF file into a byte array:

```bash
xxd -i minha_fonte.ttf > fonte_data.h
```

Generates:

```c
unsigned char minha_fonte_ttf[] = {
  0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x80,
  // ... rest of bytes
};
unsigned int minha_fonte_ttf_len = 12345;
```

### Step 2: Load from memory with SDL_RWops

Use `SDL_RWFromConstMem` to create a memory stream and
`TTF_OpenFontRW` to load the font:

```c
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "fonte_data.h"

SDL_RWops* rw = SDL_RWFromConstMem(minha_fonte_ttf, minha_fonte_ttf_len);
TTF_Font* fonte = TTF_OpenFontRW(rw, 1, 24);
```

## API Reference

```c
TTF_Font* TTF_OpenFontRW(SDL_RWops* src, int freesrc, int ptsize);
```

- `src`: SDL_RWops with font data
- `freesrc`: 1 = auto-close, 0 = manual
- `ptsize`: font size in points

## Notes

- pico-sdl already uses this technique via `src/tiny_ttf.h`
- Font is compiled directly into the executable
- No external files needed — single binary distribution

## Status

- [x] Reference documented
