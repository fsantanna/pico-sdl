# 2026-05-os-window

Park: parent of `window` is the **OS window manager (desktop)**.
Today the OS-window properties (opacity, position, size, fullscreen)
are scattered across `pico_set_window_*` / `Pico_Window` fields.
Unify them into `window.effect` + `window.scene.dst`.

## Context

`window` is now a regular layer (per `2026-05-base-layer`), but its
OS-side properties still live in `Pico_Window {fs, show, title}` and
the dedicated `pico_set_window_*` API. Folding them into
`window.effect` / `window.scene.dst` makes window-on-OS look
identical to layer-on-parent.

## Sub-changes

1. **opacity**: `window.effect.alpha` → `SDL_SetWindowOpacity` on change.
2. **position + size**: `window.scene.dst` → `SDL_SetWindowPosition`
   + `SDL_SetWindowSize` on change. Today `dst` is unused for window.
3. **fullscreen**: fold `Pico_Window.fs` into `scene.dst` (e.g. mode
   sentinel `'!' {0,0,0,0}` = fullscreen, else specific rect on desktop).

After landing, `Pico_Window` collapses to `{title, ...sdl-handles}`.

## Status

**Parked.** Tracked here so it doesn't get lost. No code changes pending.
