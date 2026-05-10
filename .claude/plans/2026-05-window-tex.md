# Window TARGET Texture (B2) + Aux-Clipping Port

## Status (2026-05-10)

**Phases A + B landed. Architectural pivot to expert-vs-default
dual-hier design supersedes the original "drop window.tex per-frame
clear" item.**

| step | status |
|---|---|
| `G.window.layer.tex` allocated as TARGET + destroyed at teardown | ✓ done |
| `pico_set_scene_dim` recreates `window.tex` on resize | ✓ done |
| Phase A: aux-clipping in `_pico_output_draw_layer`, bound from `SDL_GetRenderTarget`+`SDL_QueryTexture` | ✓ done |
| `pico_set_window_fs` no longer touches `scene.dim` (fs is render-time stretch) | ✓ done |
| `tst/layer-clip.{c,lua}` regression suite (6 cases: dst overflow, src overflow, world+child src.y) | ✓ done |
| `_show_grid` move to framebuffer | resolved-by-design (aux now uses target dim → no coord mismatch) |
| Phase B.4: replace bespoke blit with `_layer_traverse(&G.window.layer)` | ✓ done |
| Phase B.5/6: relax `pico_output_present` + auto-present gate to `world \|\| window` | ✓ done |
| `_show_grid` moved to per-window-child in `_pico_output_draw_layer` (post-aux src/dst params) | ✓ done |
| `_show_tile` + `_show_grid` relocated from `pico.c` to `layers.hc` | ✓ done |
| Rename `keep` → `clear` (inverted semantic; values flipped) | ✓ done |
| Drop per-frame `window.tex` clear → window persistent | **superseded** by dual-hier design |
| `tst/window.c` window-01 (commented out in Makefiles) | **deferred** until expert mode lands |

`make tests` and `cd lua && make tests` are green.

## Architectural Decision (2026-05-10): expert vs default dual-hier

Rather than dropping the per-frame `window.tex` clear (which would
expose the BLEND-mode transparency-accumulation issue when children
are composited multiple times), we split rendering into two modes:

| | non-expert (default, "student kit") | expert |
|---|---|---|
| auto-present | yes (on world only) | no (manual `pico_output_present()`) |
| `pico_set_layer("window")` | error: "requires expert mode" | allowed |
| `world.scene.clear` | 0 (canvas; draws persist) | 1 (regenerator; redraw each frame) |
| render path | traverse from world; world→fb (or trivial mirror) | traverse from window; full hier on `window.tex` |
| window-direct draws | not exposed | available |
| sibling-of-world layers | not exposed | available |

**Rationale**:
- Beginner mental model = "world is the canvas, draw and it stays."
- Expert mental model = "game-loop: redraw each frame, full window
  hierarchy, manual present cycle."
- Existing `pico_set_expert(on, fps)` already toggles auto-present;
  extending it to flip render path + `world.scene.clear` keeps
  the API minimal (one switch, three coupled behaviors).
- Avoids the architectural rabbit-hole of "two-tex layers" needed
  to make BLEND compositing idempotent under repeated auto-presents.

### Implementation outline

`pico_set_expert(on, fps)`:
- Toggle `G.expert.on`.
- Toggle `G.world.scene.clear = on ? 1 : 0`. Bypass the normal
  `pico_set_scene_clear` assert (which rejects WORLD).

`pico_set_layer`:
- If `key == "window"` and `!G.expert.on`: assert/error.

`_pico_output_present(force)`:
- If `G.expert.on`:
  - Window hier path: `_layer_traverse(&G.window.layer)`, mirror `window.tex → fb`.
- Else:
  - World hier path: target = fb (or trivial pass-through);
    `_pico_output_draw_layer(&G.world)` directly to fb, with
    Phase A aux for src/dst clipping.

`_pico_output_draw_layer`'s `_show_grid` call: keep the
`G.layer == &G.window.layer` gate (only direct window-children get
pixel grid + labels). Non-expert mode never has window children
besides world, so `_show_grid` fires for world.

### Implementation status (2026-05-10)

- ✓ `pico_set_expert(on, fps)` flips `G.world.scene.clear`.
- ✓ Auto-present off in expert via existing `G.expert.on` early-return.
- ✗ Window-draw restriction (`pico_set_layer("window")` or draw-on-window in non-expert) — **dropped**. Both placements proved messy:
    - set_layer assert: too coarse (internal callers like resize/screenshot/set_dim use it for housekeeping).
    - draw-time assert: needed `ing.out` exemption for internal `_show_grid` text/line draws — workable but added churn for marginal benefit.
    - Decision: don't enforce. Drawing on window in non-expert is "undefined" by convention; tests + docs steer users away.
- ✓ `tst/window.c` + `lua/tst/window.lua` re-enabled. Tests wrap with `pico_set_expert(1, -1)` + explicit `pico_output_present()` before `_pico_check`.

### Out-of-scope follow-ups

- **Two-tex layers** (clean BLEND compositing): each layer keeps
  `draws.tex` (user paints) + `composed.tex` (children composited
  onto draws each present). Eliminates accumulation-on-repeat-composite
  for transparent layers. Bigger refactor; flag for future.

## Context

This work unblocks plan-line 230 of
`.claude/plans/2026-05-base-layer.md` ("Retire bespoke world→window
blit; start `_layer_traverse` from `&G.window.layer`"), which a
prior attempt failed to land.

Today `G.window.layer.tex == NULL` so drawing on the window targets
the OS framebuffer directly. The bespoke `_pico_output_present`
silently bundled two responsibilities:

1. Per-frame `SDL_RenderClear` of the framebuffer + `aux()` clipping
   when `world.scene.dst` extends beyond window bounds (negative or
   oversized) — propagates the clip into src to keep aspect mapping
   correct.
2. Direct draws on the window-as-framebuffer have no persistent
   home; the per-frame clear wipes them, so user-drawn content on
   the window layer never appears (`tst/window.c` window-01 case
   currently fails for this reason).

Splitting these out:

- **(a)** Move aux-clipping into the general
  `_pico_output_draw_layer` so any over-extending dst is handled
  uniformly (both world→window during present and any user
  `pico_output_draw_layer` call).
- **(b) B2**: give the window a real `SDL_TEXTUREACCESS_TARGET`
  texture sized to window dim. Direct draws live on `window.tex`
  (persistent because `keep == -1`); present composites window's
  children onto `window.tex` via `_layer_traverse`, then a single
  final blit copies `window.tex → fb`. The framebuffer becomes a
  pure mirror of `window.tex`.

Outcome: `_pico_output_present` collapses to traverse + final blit
+ present, with no special-case for the world→window edge.
`tst/window.c` window-01 (red rect at 33% on window + world blue
at 66% — already designed, baseline `tst/asr/window-01.png` already
on disk) starts passing.

## Critical files

- `src/pico.c` — `pico_init`, `pico_init(0)` teardown,
  `pico_set_scene_dim` window branch, `_pico_output_present`,
  `pico_output_present`, auto-present eligibility check
- `src/layers.hc` — `_pico_output_draw_layer` (aux-clipping port)
- `.claude/plans/2026-05-base-layer.md` — update progress checklist
  when each phase lands

## Phase A — Port aux-clipping into `_pico_output_draw_layer`

**File**: `src/layers.hc:170-204`

Insert clipping between dst computation
(`SDL_Rect dst = _abs_rect(&rf);` at `:182`) and
`SDL_SetTextureAlphaMod` at `:190`.

Code (mirrors `pico.c:1566-1593`):

```c
void aux (SDL_Rect* a, SDL_Rect* b, int max_w, int max_h) {
    /* same body as in _pico_output_present */
}
int max_w = G.layer->scene.dim.w;   /* parent = current logical layer */
int max_h = G.layer->scene.dim.h;
if (dst.x < 0 || dst.y < 0 ||
    dst.x + dst.w > max_w || dst.y + dst.h > max_h)
{
    aux(&dst, &src, max_w, max_h);
    aux(&src, &dst, sup->w, sup->h);
}
```

Parent bounds via `G.layer->scene.dim`:
- During `_layer_traverse(UP)`, `G.layer = UP` (`layers.hc:82`) so
  the parent of any child being drawn is `G.layer`.
- During user `pico_output_draw_layer(name, rect)`, the current
  layer's tex is the active render target — `G.layer` is again the
  parent.

GCC nested-fn syntax already used elsewhere (`pico.c:1566`).

**Gate A**: full `make tests` + `cd lua && make tests`. Tests with
default world.scene.dst exactly fill window so `aux` is a no-op —
expect zero baseline diffs. Risk tests: `tst/navigate.c`,
`tst/view-target.c` (non-default world.scene.dst).

## Phase B — Window TARGET Texture

### B.1 — `pico_init` allocate window.tex

**File**: `src/pico.c:269-360`

After `G.window.ren` exists + `BlendMode` set (around `:343`),
**before** `G.world.tex = _tex_create(PICO_DIM_LOG)` at `:349`:

```c
G.window.layer.tex = _tex_create(G.window.layer.scene.dim);
SDL_SetTextureBlendMode(G.window.layer.tex, SDL_BLENDMODE_NONE);
SDL_SetRenderTarget(G.window.ren, G.window.layer.tex);
{
    Pico_Color c = G.window.layer.effect.color;
    SDL_SetRenderDrawColor(G.window.ren, c.r, c.g, c.b, c.a);
    SDL_RenderClear(G.window.ren);
}
```

`_tex_create` (`src/pico.c:73-80`) already produces TARGET RGBA32.

The existing `SDL_SetRenderTarget(ren, G.world.tex)` at `:351`
remains.

### B.2 — `pico_init(0)` teardown

**File**: `src/pico.c:362-381`

Add before world.tex destroy at `:371`:

```c
if (G.window.layer.tex != NULL) {
    SDL_DestroyTexture(G.window.layer.tex);
    G.window.layer.tex = NULL;
}
```

### B.3 — `pico_set_scene_dim` window branch

**File**: `src/pico.c:758-764`

Replace the "no texture" path with destroy/recreate:

```c
if (L == &G.window.layer) {
    assert(!G.window.pub.fs);
    SDL_DestroyTexture(L->tex);
    L->tex = _tex_create(di);
    SDL_SetTextureBlendMode(L->tex, SDL_BLENDMODE_NONE);
    SDL_SetWindowSize(G.window.win, di.w, di.h);
    SDL_SetRenderTarget(G.window.ren, L->tex);
    {
        Pico_Color c = L->effect.color;
        SDL_SetRenderDrawColor(G.window.ren, c.r, c.g, c.b, c.a);
        SDL_RenderClear(G.window.ren);
    }
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&L->scene.clip, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
    _pico_output_present(0);
}
```

Resize handler at `pico.c:987-997` already routes through this —
no other change needed.

### B.4 — `_pico_output_present` retire bespoke blit

**File**: `src/pico.c:1538-1614`

Replace body from `:1551` (`G.window.ing.out = 1;`) through `:1613`
(restore block) with:

```c
G.window.ing.out = 1;

/* composite window's children (incl. world) onto window.tex */
_layer_traverse(&G.window.layer);

/* final blit: window.tex → fb (full coverage) */
SDL_SetRenderTarget(G.window.ren, NULL);
SDL_RenderCopy(G.window.ren, G.window.layer.tex, NULL, NULL);

_show_grid();              /* debug overlay on fb */
SDL_RenderPresent(G.window.ren);

G.window.ing.out = 0;

/* restore current layer's render target + clip */
SDL_SetRenderTarget(G.window.ren, G.layer->tex);
{
    Pico_Abs_Rect r = pico_cv_rect_rel_abs(&G.layer->scene.clip, NULL);
    SDL_RenderSetClipRect(G.window.ren, &r);
}
```

The bespoke `aux()` + `RenderCopy(world.tex)` are gone — Phase A
handles aux in `_pico_output_draw_layer`. The per-frame
`SDL_RenderClear` of fb is also gone — `window.tex` persists
(`G.window.layer.scene.keep == -1` at `pico.c:313` skips
post-composite clear in `layers.hc:95`).

### B.5 — `pico_output_present()` explicit

**File**: `src/pico.c:1616-1620`

Relax assert:

```c
void pico_output_present (void) {
    _pico_guard();
    assert((G.layer == &G.world || G.layer == &G.window.layer)
        && "can only present from world or window");
    _pico_output_present(1);
}
```

### B.6 — Auto-present eligibility

**File**: `src/pico.c:1544`

Extend gate from world-only to world-or-window:

```c
} else if (G.layer != &G.world && G.layer != &G.window.layer) {
    return;
}
```

Auto-present fires after window-layer ops.

## Follow-up (after Phase B lands) — `_show_grid` to framebuffer

**File**: `src/pico.c` `_pico_output_present`

**Why**: Phase B drops the per-frame `window.tex` clear (window.keep
== -1 → traverse skips post-composite clear). Grid lines drawn into
`window.tex` would then accumulate across frames.

**Change**: Move `_show_grid()` after the final `RenderCopy(window.tex
→ fb)`. Body unchanged — already reads `G.layer` (layer-generic).

```c
_layer_traverse(&G.window.layer);        // → window.tex
SDL_SetRenderTarget(G.window.ren, NULL);
SDL_RenderCopy(G.window.ren, G.window.layer.tex, NULL, NULL);
_show_grid();                            // on fb, not window.tex
SDL_RenderPresent(G.window.ren);
```

**Side benefit**: with the auto-present gate extended to `world ||
window`, the grid naturally applies to whichever of the two is
current — not just world.

**Tradeoffs**:
- Layer (non-window) screenshots never saw grid (it lives on
  window.tex, not on `L->tex`) — unchanged.
- Window screenshots read fb directly → still see grid.

**Order**: not the next step; lands after B.4–B.6 prove green.

## Files NOT to change

- `src/pico.c:1643-1694` `pico_output_screenshot` — universal
  blit-through-TARGET fallback stays. With B2, window.tex is
  TARGET-access, so the fast path applies (no temp tex).
- `src/layers.hc:80-104` `_layer_traverse` — already correct.
- `src/mem.hc` — window is preallocated in `G`, not realm-managed.

## Verification

1. **After Phase A** — full suites, expect zero diffs:
   ```bash
   make tests
   cd lua && make tests
   ```
   World's default scene.dst covers window exactly; `aux` no-ops.

2. **After Phase B** — same suites + window-01 should match the
   existing baseline:
   ```bash
   make tests
   cd lua && make tests
   make int T=window     # visual confirmation
   ```
   On-disk `tst/asr/window-01.png` (gray + red NW @33% + blue SE
   @66%) is the post-refactor target.

3. **window-01 expected flow**:
   - init: window.tex created + cleared with `effect.color`
     (default `{0x77,0x77,0x77,0xFF}` until test overrides);
     world.tex created + cleared (world.color = black)
   - test: world.set.scene_dst SE; world.set.effect_color blue;
     world.output.clear → world.tex = blue → auto-present
     composites world (blue) onto window.tex at SE → blit → fb
     shows gray + blue SE
   - test: set.layer(window); set.effect_color `0x80`;
     output.clear → window.tex filled gray → auto-present
     composites world over window.tex at SE → window.tex = gray +
     blue SE
   - test: draw_rect red @33% → window.tex = gray + red NW + blue
     SE → auto-present composites world at SE again (overwrites
     overlap) → window.tex = gray + red NW (corner clobbered) +
     blue SE → blit → fb
   - `_pico_check` reads window.tex (TARGET) → matches baseline.

4. **If window-01 fails**, check:
   - window.tex initial clear color matches baseline gray
   - `_layer_traverse(&G.window.layer)` descends to world via
     `_layer_attach("window","world")` at `pico.c:332`
   - Phase A's aux doesn't corrupt the default-cover case

## Risks / edge cases

- **World BLENDMODE_NONE → window.tex**: overwrites in dst region
  (same effective behavior as the old bespoke blit onto fb).
- **`_show_grid` on fb**: stays after final blit so the debug
  overlay sits on top of presented content; not baked into
  `window.tex`.
- **`pico_set_layer("window")` + `output.clear`**: targets
  `window.tex` (now non-NULL); behavior parallels other layers.
- **Init ordering**: `G.window.layer.scene.dim = PICO_DIM_PHY` set
  in static initializer at `pico.c:314`, available before window.tex
  allocation.
- **Fullscreen**: `pico_set_scene_dim` window branch asserts
  `!G.window.pub.fs` — preserved.

## Phase ordering

1. Phase A in isolation; full test suites; expect zero diffs.
2. Phase B; full test suites + visual check on window-01.
3. Update `.claude/plans/2026-05-base-layer.md`:
   - Mark `tst/window.c` (line 230 entry) `[x]`
   - Mark `_pico_output_present` refactor (line 231) `[x]`
   - Move "Failed refactor attempt" section to historical / drop
     (lessons baked into this plan)
4. Confirm next pending items (`'w'` mode deletion, etc.) become
   unblocked.
