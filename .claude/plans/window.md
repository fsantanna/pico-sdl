# Window Bulk Redesign

## Context

`pico_get_window` / `pico_set_window` use individual output
params (`title`, `fs`, `dim`) and NULL/-1 sentinels for
"unchanged". This is asymmetric with the other bulks
(`pico_get/set_draw`, `_show`, `_view`) that take a single
struct. Promote window state to a first-class bulk struct
matching the rest of the API.

## New struct

```c
typedef struct Pico_Window {
    Pico_Abs_Dim dim;
    int          fs;
    int          show;
    const char*  title;
} Pico_Window;
```

Field order alpha-sorted within the struct (dim, fs, show,
title), matching convention from other layer structs.

`show` (window visibility) absorbs the orphan
`pico_get/set_window_show` pair into the bulk.

## New API

Mirror the bulk pattern of show/draw/view (alpha-ordered
within each section).

### Bulk

```c
void pico_get_window (Pico_Window* win);
void pico_set_window (Pico_Window win);
```

Setter replaces all fields. Caller does
read-modify-write for partial updates:

```c
Pico_Window win;
pico_get_window(&win);
win.title = "new";
pico_set_window(win);
```

### Individuals

```c
Pico_Abs_Dim pico_get_window_dim   (void);
int          pico_get_window_fs    (void);
int          pico_get_window_show  (void);
const char*  pico_get_window_title (void);

void pico_set_window_dim   (Pico_Rel_Dim* dim);
void pico_set_window_fs    (int fs);
void pico_set_window_show  (int on);
void pico_set_window_title (const char* title);
```

Note: `pico_set_window_dim` takes `Pico_Rel_Dim*` for
input flexibility (consistent with
`pico_set_view_dim`); the bulk getter/setter store
absolute dimensions.

### Removed

- `pico_set_window (const char* title, int fs, Pico_Rel_Dim* dim)`
  (3-param form with NULL/-1 sentinels)
- `pico_get_window (const char** title, int* fs, Pico_Abs_Dim* dim)`
  (3-param output form)

`pico_get/set_window_show` keep the same names but become
"individual" entries within the window family.

## Steps

### 1. Define struct + bulk getter/setter [ ]

- `src/pico.h` | add `Pico_Window` typedef in colors.h-style
  layer-types section (or inline near window APIs)
- `src/pico.h` | replace 3-param `pico_get/set_window` with
  bulk `(Pico_Window*)` / `(Pico_Window)` versions
- `src/pico.c` | reimplement bulk get/set; bulk set must
  diff against current state to apply title/fs/dim/show
  changes via internal helpers (avoid redundant SDL calls)

### 2. Add individual getters [ ]

- `src/pico.h` | declare `pico_get_window_dim/fs/title`
- `src/pico.c` | implement (read from `S.win` / SDL state)
- `pico_get_window_show` already exists

### 3. Add individual setters [ ]

- `src/pico.h` | declare `pico_set_window_dim/fs/title`
- `src/pico.c` | implement; extract the per-field SDL logic
  from current monolithic `pico_set_window`
- `pico_set_window_show` already exists

### 4. Lua bindings [ ]

- `lua/pico.c` | rewrite `l_get_window` / `l_set_window` to
  use bulk struct form; field order: dim, fs, show, title
- `lua/pico.c` | drop sentinel-based partial set; use bulk
  read-modify-write; partial Lua tables already handled by
  bulk setter pattern (only set fields present in table)

### 5. Caller migration [ ]

- C tests (`tst/*.c`) | replace
  `pico_set_window("title", -1, &dim)` →
  `pico_set_window_title("title")` +
  `pico_set_window_dim(&dim)` (or build full struct)
- Lua tests/docs | already use `pico.set.window {…}` table
  form which still works after binding update

### 6. Doxygen [ ]

- `src/pico.h` | document bulk get/set with `@sa` to each
  other and to individuals' family
- Individuals: leave undocumented (per project convention:
  individuals with bulk are not documented)

## Verification

- `make tests` and `cd lua/ && make tests`
- `tst/dim.c` exercises window dim
- visual tests verify title/fs unchanged where expected

## Open questions

- `dim` storage: keep `Pico_Abs_Dim` (current behavior),
  set via `Pico_Rel_Dim*` individual?
- `fs` toggle is currently coupled with dim resize logic;
  splitting into individual setters needs to preserve the
  fullscreen save/restore behavior (`_old` static in
  `pico_set_window`)
