# Plan: Bare-metal Raspberry Pi target via Circle

Analysis and revised plan for porting pico-sdl to bare-metal RPi using
[Circle](https://github.com/rsta2/circle) (+ circle-stdlib), taking
[Pineapple](https://github.com/ry755/pineapple) as proof that Lua 5.4
runs on that stack.

## Key finding: the backend is a compositor, not a blitter

The original proposal maps `SDL_RenderCopy` to "direct framebuffer
writes". The code says otherwise: pico-sdl's entire rendering model is
**render-to-texture composition** on `SDL_Renderer`:

- every `Pico_Layer` owns an `SDL_Texture` with
  `SDL_TEXTUREACCESS_TARGET` (`_pico_tex_create`, `pico.c:50`);
- drawing always goes through `SDL_SetRenderTarget` + clip rect
  (`layer.c:25`, `output.c`);
- layer composition uses `SDL_RenderCopyEx` with rotation, flip,
  alpha-mod and blend modes (`layer.c:328-333`);
- present mirrors `window.tex` to the screen (`_pico_output_present`,
  `output.c:266`);
- screenshots use `SDL_RenderReadPixels` (`output.c:340`).

So the core of the port is a small **software renderer** implementing
this model over plain RGBA32 buffers in RAM. That sounds bigger than
"blit sprites", but two facts contain it:

1. Under `PICO_TESTS` pico-sdl already runs on `SDL_RENDERER_SOFTWARE`
   (`pico.c:135`), so the semantics to reproduce are *already*
   software-renderer semantics — no GPU-only behavior involved.
2. The visual regression suite (`check.h`, `make tests`, `asr/` images)
   gives bit-level ground truth. The renderer can be written and
   validated **entirely on desktop Linux**, before Circle is involved.

## SDL2 surface actually used (next-step item, now done)

107 distinct `SDL_*` symbols across `src/` + `lua/pico.c`. By group:

### 1. Renderer core — the real work (~25 symbols)
`SDL_CreateTexture` (RGBA32, TARGET|STREAMING), `SDL_DestroyTexture`,
`SDL_QueryTexture`, `SDL_Set/GetRenderTarget`, `SDL_RenderSetClipRect`,
`SDL_SetRenderDrawColor`, `SDL_SetRenderDrawBlendMode`,
`SDL_SetTextureBlendMode`, `SDL_SetTextureAlphaMod`, `SDL_RenderClear`,
`SDL_RenderFillRect`, `SDL_RenderDrawRect/Line/Point/Points`,
`SDL_RenderCopy`, `SDL_RenderCopyEx` (rotation+flip+alpha),
`SDL_RenderReadPixels`, `SDL_RenderPresent`.
Hard 20%: `RenderCopyEx` (scaled + rotated blit with blending).

### 2. SDL2_gfx primitives (6 functions)
`(filled)EllipseRGBA`, `(filled)PolygonRGBA`, `(filled)TrigonRGBA`
(`output.c`). SDL2_gfx is zlib-licensed and implemented *on top of* the
renderer API — vendoring its source over our shim likely works
unchanged.

### 3. Window management — mostly no-ops on bare metal
`SDL_CreateWindow/DestroyWindow`, fullscreen, resizable, title,
show/hide, `SDL_WINDOWEVENT_RESIZED` (`pico.c`, `get-set.c`). On a
fixed-mode framebuffer these become stubs; "fullscreen" is the only
mode.

### 4. Events & input (`input.c`)
`SDL_WaitEvent(Timeout)` drives everything (`pico_input_event_timeout`
is the single event pump; delays are timeouts with no event filter).
Plus `SDL_GetMouseState`, `SDL_GetModState`, `SDL_WarpMouseInWindow`,
`SDL_PushEvent(SDL_QUIT)`, `SDL_Pump/FlushEvents`.
→ Circle: `CUSBKeyboardDevice` raw-mode handler + `CUSBMouseDevice`,
feeding a small event queue with the same shape; wait-with-timeout via
`CTimer` + `CScheduler::Yield()`.

### 5. Timing
`SDL_GetTicks` (input.c, get-set.c, video.c) → `CTimer::GetClockTicks`
/ ms wrapper. No threads are used anywhere in pico-sdl.

### 6. Keys (`keys.h`)
`PICOK_*` are aliases of `SDLK_*` numeric values. Keep the values,
translate USB HID codes → SDLK numbers in the platform glue; `keys.h`
and user code stay intact.

### 7. Surfaces & images (`mem.c`, `output.c`)
`SDL_Surface`, `SDL_CreateRGBSurfaceWithFormat(From)`,
`SDL_BlitSurface`, `SDL_FreeSurface`, `IMG_Load` path,
`IMG_SavePNG` (screenshots) → `stb_image` + `stb_image_write` (or
lodepng) over an internal surface struct.

### 8. Text (`mem.c`)
`TTF_OpenFont(RW)`, `TTF_RenderText_Solid`, `TTF_SizeText`,
`TTF_FontLineSkip`. Default font is already embedded in the binary
(`tiny_ttf.h`, 19 KB TTF array, loaded via `SDL_RWFromConstMem`) —
ideal for bare metal, zero filesystem needed for text.
→ `stb_truetype` (solid 8-bit render matches `RenderText_Solid`).

### 9. Audio (`output.c`, `mem.c`)
`Mix_OpenAudio(22050, S16, 2ch)`, `Mix_LoadWAV`, `Mix_PlayChannel`,
`Mix_FreeChunk`. WAV-only in practice → tiny software mixer feeding
`CHDMISoundBaseDevice`/`CPWMSoundBaseDevice`.

### 10. Video (`video.c`, optional)
`SDL_PIXELFORMAT_YV12` streaming texture + `SDL_UpdateYUVTexture` →
software YUV→RGB. Defer to a late phase.

### 11. Public header leak (`pico.h`)
`pico.h` includes `SDL2/SDL.h` and typedefs `SDL_Rect/Point/FPoint`
into `Pico_*`, plus `SDL_FLIP_*`. The bare-metal build needs a small
compat header defining just these types/constants so user programs and
`lua/pico.c` (whose only SDL call is `SDL_GetKeyName`) compile
unmodified.

## Revised architecture

```
        desktop                    bare metal
┌───────────────────┐        ┌───────────────────┐
│  Lua API (lua/)   │        │  Lua API (lua/)   │  unchanged
├───────────────────┤        ├───────────────────┤
│  pico-sdl (src/)  │        │  pico-sdl (src/)  │  unchanged
├───────────────────┤        ├───────────────────┤
│       SDL2        │        │ sdl_shim: soft    │  new, portable C,
│                   │        │ renderer + events │  testable on Linux
├───────────────────┤        ├───────────────────┤
│   Linux + X11     │        │ Circle glue (C++) │  fb/USB/timer/sound
└───────────────────┘        └───────────────────┘
```

Instead of rewriting `geom.c`/`layer.c` against Circle classes
(proposal step 3), keep **all** of `src/` unchanged and implement the
SDL subset above as a shim. `geom.c` contains no real SDL calls (only
struct types); the coupled files are `pico.c`, `output.c`, `layer.c`,
`mem.c`, `input.c`, `get-set.c`, `video.c` — and they stay untouched.
This splits the port into two independently testable halves.

## Phases

### Phase 0 — Bring-up
Build Pineapple/Circle, boot in QEMU (Circle ships QEMU support, see
`circle/doc/qemu.txt`; note QEMU's raspi USB emulation is limited —
plan on serial console for early testing, real HW for input).

### Phase 1 — `sdl_shim` on desktop (no Circle)
Portable C: RGBA32 target buffers, clip, blend, draw color state;
fill/draw primitives; `RenderCopy(Ex)`; `ReadPixels`; event queue fed
by a pluggable platform layer (desktop backend: X11 or even SDL2 itself
for window+input only). Validate with the existing `make tests` /
`asr/` images against real-SDL2-software output.

### Phase 2 — Circle platform glue
Framebuffer present (`CBcmFrameBuffer`, single scaled blit of
`window.tex`), USB keyboard/mouse → event queue + HID→SDLK table,
`CTimer` ticks, wait-with-timeout pump, window stubs. Filesystem via
circle-stdlib (FatFS/SD) so `fopen`-based paths (images, sounds,
video) keep working.

### Phase 3 — Assets
`stb_truetype` (default font from `tiny_ttf.h`), `stb_image(_write)`,
WAV loader + mixer on Circle sound device.

### Phase 4 — Lua target
Swap Pineapple's raw interpreter for `lua/pico.c` bindings: boot →
`main.lua` on SD card ("fantasy console" mode).

### Phase 5 (optional) — `video.c` YUV playback.

## Sizing

- shim renderer: ~1.5–2k lines of C (pico-sdl core itself is ~3.3k);
  most functions are trivial; `RenderCopyEx` and text are the hard part
- Circle glue: ~1k lines of C++
- `src/` and `lua/` diffs: near zero (build flags + compat header)

Performance is a non-issue: the default logical canvas is 100×100;
present is one nearest-neighbor scale per frame. RPi CPUs handle this
in software with huge margin.

## Risks / decisions

- **License**: linking Circle/circle-stdlib makes the bare-metal
  *binary* GPL-3. Keep it as a separate target (e.g. `bare/` dir or
  branch); the shim itself can stay under pico-sdl's license since it
  is Circle-independent by design.
- **QEMU input**: USB HID under QEMU raspi is unreliable; validate
  rendering in QEMU, input on hardware.
- **Blocking waits without threads**: `SDL_WaitEventTimeout` semantics
  must be reproduced with a poll+yield loop; audio must be fed by IRQ
  or DMA queue since there is no mixer thread.
- **Screenshot path** (`output.c:364`) relies on a 5 ms delay hack —
  revisit on the shim rather than reproducing it.

## Next steps

- [x] Exact SDL2 call inventory (this document)
- [ ] Decide target layout: `bare/` subdir vs. separate branch/fork
      (license isolation suggests subdir with its own Makefile is fine —
      GPL applies to the linked binary, not the repo)
- [ ] Phase 0: Pineapple boot in QEMU
- [ ] Phase 1: shim skeleton + `make tests` parity on desktop
