# Plan: CLI Options (Issue #100)

## Context

Issue #100 requests `--version` and `--help` for both `pico-sdl`
and `pico-lua`.
`pico-lua` already has `--version` (v0.4-dev).
Need to add `--help` to both and `--version` to `pico-sdl`.

## Changes

### 1. `pico-sdl` -- add `--version` and `--help`

- File: `pico-sdl`
- Add `VERSION=v0.4-dev` near the top
- Add option handling block before the existing logic:
    - `--version` prints `pico-sdl v0.4-dev` and exits
    - `--help` prints minimal usage and exits

```
Usage: pico-sdl [options] <file.c>

Options:
  --help       Show this help
  --version    Show version
```

### 2. `pico-lua` -- add `--help`

- File: `lua/pico-lua`
- Add `--help` option after the existing `--version` block

```
Usage: pico-lua [options] <file.lua>

Options:
  --help       Show this help
  --version    Show version
```

## Verification

```bash
./pico-sdl --version    # pico-sdl v0.4-dev
./pico-sdl --help       # usage text
lua/pico-lua --version  # pico-lua v0.4-dev
lua/pico-lua --help     # usage text
```
