# Window Bulk Redesign

## Context

`pico_get_window` / `pico_set_window` use individual output
params (`title`, `fs`, `dim`) and NULL/-1 sentinels for
"unchanged". This is asymmetric with the other bulks
(`pico_get/set_draw`, `_show`, `_view`) that take a single
struct. Promote window state to a first-class bulk struct
matching the rest of the API.

## Status: complete

All steps 1–6 done. `make tests` and `cd lua/ && make tests`
both pass.

## Final struct

```c
typedef struct {
    Pico_Abs_Dim dim;
    int          fs;
    int          show;
    const char*  title;
} Pico_Window;
```

Alpha-sorted fields. `show` (window visibility) absorbs the
orphan `pico_get/set_window_show` pair into the bulk.

Name collision: `events.h` previously had a `Pico_Window`
struct for resize event data. Resolved by inlining it as an
anonymous `struct { int w, h; } window;` member inside
`Pico_Event` (field name preserved so `e.window.w/h` callers
still work).

## Final API

```c
void         pico_get_window       (Pico_Window* win);
Pico_Abs_Dim pico_get_window_dim   (void);
int          pico_get_window_fs    (void);
int          pico_get_window_show  (void);
const char*  pico_get_window_title (void);

void pico_set_window       (Pico_Window win);
void pico_set_window_dim   (Pico_Rel_Dim* dim);
void pico_set_window_fs    (int fs);
void pico_set_window_show  (int on);
void pico_set_window_title (const char* title);
```

Notes:
- `pico_set_window_dim` takes `Pico_Rel_Dim*` for
  consistency with `pico_set_view_dim` and all other
  `Pico_Rel_*` args in the API.
- Bulk `pico_set_window` is a **full replacement** (matches
  `pico_set_view` / `_show` / `_draw` style). It
  unconditionally delegates to individuals; caller must
  provide all 4 fields:

```c
void pico_set_window (Pico_Window win) {
    _pico_guard();
    pico_set_window_title(win.title);
    pico_set_window_fs(win.fs);
    if (!win.fs) {
        Pico_Rel_Dim rel = {'!', {win.dim.w, win.dim.h}, NULL};
        pico_set_window_dim(&rel);
    }
    pico_set_window_show(win.show);
}
```

The `!win.fs` guard is required because
`pico_set_window_dim` asserts `!S.win.fs` (can't resize in
fullscreen). No earlier diff-against-current logic — `_fs`
itself early-returns when `fs == cur.fs`; the other
individuals are cheap.

- Fullscreen save/restore: `pico_set_window_fs` keeps a
  `static Pico_Abs_Dim _old` for pre-fullscreen dim.

## Lua binding

`l_set_window` follows `l_set_view`'s individual-delegation
pattern (NOT read-modify-write):

```c
static int l_set_window (lua_State* L) {
    Pico_Rel_Dim* xdim   = NULL;
    int           xfs    = -1;
    int           xshow  = -1;
    const char*   xtitle = NULL;

    // ... getfield each, populate locals ...

    if (xtitle != NULL) { pico_set_window_title(xtitle); }
    if (xfs    != -1)   { pico_set_window_fs   (xfs);    }
    if (xdim   != NULL) { pico_set_window_dim  (xdim);   }
    if (xshow  != -1)   { pico_set_window_show (xshow);  }
    return 0;
}
```

Lua table key is `fullscreen` (not `fs`) for backward compat.

## Steps

### 1. Struct + events.h rename + bulk get/set [x]

- `src/pico.h` — added `Pico_Window` typedef in Types section
- `src/events.h` — inlined anonymous `{int w, h;} window;`
  inside `Pico_Event` (freed `Pico_Window` name)
- `src/pico.c` — reimplemented bulk get/set

### 2. Individual getters [x]

- `pico_get_window_dim/fs/title` in `src/pico.c`
- `pico_get_window_show` already existed

### 3. Individual setters [x]

- `pico_set_window_dim/fs/title` in `src/pico.c`
- `pico_set_window_show` already existed
- `pico_set_window_fs` preserves fullscreen `_old` restore

### 4. Lua bindings [x]

- `l_get_window` uses bulk struct, adds `show` field to
  returned table
- `l_set_window` uses view-style individual delegation

### 5. Caller migration [x]

- C tests (`tst/*.c`, 35 files + `tst/todo/*.c` 4 files)
- Rules applied:
  - 1-arg old call → individual setter
  - 2+ arg old call → bulk
    `pico_set_window((Pico_Window){ .dim={W,H}, .fs=0,
    .show=1, .title="..." })` with all 4 fields explicit
- Inlined former `Pico_Rel_Dim phy` decls that were only
  used for window dim setup
- Internal callers in `src/pico.c` migrated:
  - `pico_set_dim` → calls `pico_set_window_dim`
  - WIN_RESIZE event handler → calls `pico_set_window_dim`
- `tst/todo/scale.c` NOT migrated — pre-existing broken
  (uses removed `Pico_Dim`/`pico_set_scale`/etc.)

### 6. Doxygen + docs [x]

- Bulk get/set: single `@brief`, no `@sa` beyond the pair
- Individuals: undocumented (have-bulk rule)
- `lua/doc/api.md` — updated `pico.get/set.window` entries
  to add `show` field, alpha-ordered

## Verification

Both suites pass:
```bash
make tests
cd lua/ && make tests
```

During development, every `gcc -c` of `src/pico.c`, each
`tst/*.c`, and the Lua `.so` (`-Wall -Werror`) all compiled
clean.

`valgrind.supp` `sdl-init` line 244 still points at
`SDL_Init` call — no update needed.

## Resolved open questions

1. `dim` rel/abs split: individual takes `Pico_Rel_Dim*`,
   bulk stores `Pico_Abs_Dim`. Confirmed OK.
2. Fullscreen save/restore: `_old` static now lives inside
   `pico_set_window_fs`.
3. `show` in bulk: bulk delegates to `pico_set_window_show`
   which calls `_pico_output_present(0)` on visibility=1.
   Consistent behavior preserved.
4. Alpha order in struct: strict alpha (dim, fs, show,
   title).
5. Bulk semantics: full-replacement (no diffing), matching
   `pico_set_view`.
6. Name collision with `events.h`: resolved by inlining
   anonymous struct inside `Pico_Event`.

## Restart point

Unlock edits (`touch .claude/ok` if hook is locked) and run:

```bash
make tests
cd lua/ && make tests
```

If any fail:
- Visual regressions likely in: `size_pct`, `size_raw`,
  `view_raw`, `get-set`, `anchor_*`, `tile*`, `buffer_*`
- `get-set.lua` uses `pico.equal(a,b)` helper (< 0.001
  tolerance) for float comparisons — if `Pico_Window.dim`
  float roundtrip bites, use that instead of `==`
- Check that at init, the bulk full-replacement pattern
  doesn't accidentally hide window (callers must set
  `.show=1` explicitly)

## Side-plan still pending

- `expert.md` — `pico_set_expert` ms semantics (separate
  from window work)
