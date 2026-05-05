# pico-sdl: dangling `up` chain after Lua GC

## Symptom

`atmos y.atm` segfaults (no error printed by valgrind before the fault),
while the equivalent `y.c` runs fine.

Minimal reproduction (`/x/x/fabrica/y.atm`):

```atmos
require "atmos.env.pico"
set pico = require "pico"

val up = @{ '%', x=0.25, y=0.5, w=0.3, h=0.8 }

pico.layer.empty(:root, :A, @{w=10, h=10})

pico.zet.layer(:A)
pico.zet.show @{ color = :white }
pico.zet.view @{
    target = @{'%',x=0.3,y=0.5,h=0.5,up=up},
}

await(false)
```

The C equivalent (`/x/x/fabrica/y.c`) does not crash because `up` is a
stack-allocated `Pico_Rel_Rect` whose lifetime spans `pico_input_loop`.

## Root cause

The Lua binding allocates `Pico_Rel_Rect` instances as Lua **userdata**.
The pico-sdl core stores rects by **shallow copy**, keeping a raw
`Pico_Rel_Rect* up` pointer.
Once the C function returns, the userdata become unreachable from Lua,
the GC eventually frees them, and the stored `up` pointer dangles.
The next `_pico_output_present` walks the chain and dereferences it.

### Trace

| step | file:line                  | what happens                                                                              |
|------|----------------------------|-------------------------------------------------------------------------------------------|
| 1    | `lua/pico.c:1103`          | `xdst = C_rel_rect(L, ...)` reads the target Lua table                                    |
| 2    | `lua/pico.c:241,247`       | recursive `C_rel_rect` allocates userdata for `up`, then for target; `target.up` = &userdata payload |
| 3    | `lua/pico.c:1116`          | `pico_set_view_dst(NULL, *xdst)` passes the target struct by value                        |
| 4    | `src/pico.c:752-755`       | `L->view.dst = dst;` shallow copy — `up` pointer kept verbatim                            |
| 5    | `lua/pico.c:1118`          | `return 0` — userdata pushed during the call become unreachable                           |
| 6    | next GC cycle              | userdata freed → `L->view.dst.up` dangles                                                 |
| 7    | next `_pico_output_present`| chain walk dereferences freed pointer → SEGV                                              |

## Confirmation test (cheap)

In `y.atm`, force GC right after the view setup:

```atmos
pico.zet.view @{ target = @{'%',x=0.3,y=0.5,h=0.5,up=up} }
collectgarbage("collect")
await(false)
```

If the segfault becomes immediate (instead of on the first frame after
some delay), the GC hypothesis is confirmed.

## Affected setters

Anything in pico-sdl that **stores** a `Pico_Rel_Rect`/`Pico_Rel_Dim`/
`Pico_Rel_Pos` whose `up` may be set:

- `pico_set_view_dst`     (`src/pico.c:752`)
- `pico_set_view_src`     (`src/pico.c:758`)
- `pico_set_view_clip`    (`src/pico.c:728`)
- `pico_set_view_dim`     (`src/pico.c:734`) — already does conversion to abs
- `pico_set_view`         (`src/pico.c:711`) — bulk setter
- analogous show/draw paths if they store rects (audit before fixing)

## Fix options

| option | layer            | summary                                                                                                                | tradeoff                                                                |
|--------|------------------|------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------|
| A      | C core           | deep-copy the `up` chain on store; free previous chain on overwrite; free on layer destroy                             | robust, language-agnostic; touches teardown paths                       |
| B      | Lua binding      | per-layer Lua registry table anchoring the userdata; release on overwrite/layer destroy                                | smaller core change; only fixes Lua callers; needs hook on layer destroy |
| C      | C core eager-resolve | resolve relative-rect to absolute at set time and store the absolute form (no `up` pointer kept)                       | simplest if absolute is sufficient; loses dynamic re-evaluation if parent rect changes |

Recommendation: **Option A** (C core deep-copy) for correctness across
all bindings. If `up`-aware semantics are not actually used for stored
rects, **Option C** is even simpler.

## Implementation sketch (Option A)

In `src/pico.c`:

```c
static Pico_Rel_Rect* _rect_chain_clone (const Pico_Rel_Rect* src) {
    if (src == NULL) {
        return NULL;
    } else {
        Pico_Rel_Rect* dst = malloc(sizeof(Pico_Rel_Rect));
        *dst = *src;
        dst->up = _rect_chain_clone(src->up);
        return dst;
    }
}

static void _rect_chain_free (Pico_Rel_Rect* r) {
    if (r != NULL) {
        _rect_chain_free(r->up);
        free(r);
    }
}
```

Setter pattern:

```c
void pico_set_view_dst (const char* layer, Pico_Rel_Rect dst) {
    _pico_guard();
    Pico_Layer* L = _pico_layer_null(layer);
    Pico_Rel_Rect* old_up = L->view.dst.up;
    L->view.dst = dst;
    L->view.dst.up = _rect_chain_clone(dst.up);
    _rect_chain_free(old_up);
    _pico_output_present(0);
}
```

Apply the same pattern to `pico_set_view_src`, `pico_set_view_clip`,
the bulk `pico_set_view`, and any show/draw setters that store rects.
Add `_rect_chain_free` calls to layer teardown so we don't leak on
layer destruction.

## Open questions

- Does `pico_set_show_*` or `pico_set_draw_*` ever store a rect with
  `up` chain? Audit before declaring fix complete.
- Is there a similar issue for `Pico_Rel_Dim.up` and `Pico_Rel_Pos.up`?
  Likely yes for any setter that stores them by shallow copy.
- Should `_pico_layer_destroy` (or equivalent) free stored rect chains?

## References

- `/x/pico-sdl/src/pico.c`        — core setters
- `/x/pico-sdl/lua/pico.c`        — Lua binding (`C_rel_rect`, `l_set_view`)
- `/x/x/fabrica/y.atm`            — minimal repro (Atmos)
- `/x/x/fabrica/y.c`              — control case (pure C, no crash)

## Status

- [x] Symptom reproduced
- [x] Root cause identified in source
- [x] Confirmation via forced GC
    - regression test: `lua/tst/view-target-up-gc.lua` (wired into `lua/Makefile`)
    - test runs under valgrind via `pico-lua` script (`PICO_TESTS=1`)
    - valgrind output confirms the plan's trace exactly:
        - alloc: `lua_newuserdatauv` <- `C_rel_rect (pico.c:247)` <- `l_set_view (pico.c:1103)`
        - free:  `free` <- `lua_gc`
        - read:  `_f1 (aux.hc:41-43)` <- `pico_cv_rect_rel_abs (pico.c:139)` <- `_pico_output_present (pico.c:1584)` <- `pico_output_clear`
    - failure mode: 12 `Invalid read` reports inside the freed 96-byte userdata
        block; `make test` exits with code 1
    - with fix: should produce zero valgrind errors and clean exit
- [x] Audit other setters for the same pattern
    - vulnerable storage: only `Pico_Layer_View.{dst,src,clip}` (3 fields)
    - vulnerable setters: `pico_set_view`, `pico_set_view_clip`,
      `pico_set_view_dst`, `pico_set_view_src`
    - `pico_set_view_dim`, `pico_set_window_dim` already assert `up==NULL`
    - `pico_set_mouse`, `pico_output_draw_*` consume immediately (no store)
    - `Pico_Layer_Show` and `Pico_Layer_Draw` contain no rects/dims/positions
    - `_free_layer` (`src/mem.hc:68`) does NOT free chains — must be extended
- [ ] Choose fix option (A / B / C)
- [ ] Implement
- [x] Add regression test in pico-sdl test suite
