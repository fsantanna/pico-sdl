# Plan: test set/get style (pico-sdl#48)

- repo: `fsantanna/pico-sdl`
- issue: https://github.com/fsantanna/pico-sdl/issues/48
- PR message must include `fix #48`

## Files to Create

### `tst/style.c`

```c
#include "pico.h"
#include "../check.h"
#include <assert.h>

int main (void) {
    pico_init(1);
    pico_set_window(
        "Style", -1,
        &(Pico_Rel_Dim){ '!', {640, 480}, NULL }
    );
    pico_set_view(
        -1,
        &(Pico_Rel_Dim){ '!', {64, 48}, NULL },
        NULL, NULL, NULL, NULL
    );

    // shapes used across tests
    Pico_Rel_Rect rect = {
        '!', {5, 5, 25, 18}, PICO_ANCHOR_NW, NULL
    };
    Pico_Rel_Rect oval = {
        '!', {34, 5, 25, 18}, PICO_ANCHOR_NW, NULL
    };
    Pico_Rel_Pos t1 = {
        '!', {5, 26}, PICO_ANCHOR_NW, NULL
    };
    Pico_Rel_Pos t2 = {
        '!', {5, 43}, PICO_ANCHOR_NW, NULL
    };
    Pico_Rel_Pos t3 = {
        '!', {30, 43}, PICO_ANCHOR_NW, NULL
    };
    Pico_Rel_Pos poly[] = {
        {'!', {34, 26}, PICO_ANCHOR_NW, NULL},
        {'!', {59, 30}, PICO_ANCHOR_NW, NULL},
        {'!', {50, 43}, PICO_ANCHOR_NW, NULL},
        {'!', {38, 43}, PICO_ANCHOR_NW, NULL},
    };

    puts("default fill");
    {
        pico_output_clear();
        pico_set_color_draw((Pico_Color){255, 255, 255});
        pico_output_draw_rect(&rect);
        pico_output_draw_oval(&oval);
        pico_output_draw_tri(&t1, &t2, &t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-01");
    }

    puts("stroke");
    {
        pico_output_clear();
        pico_set_style(PICO_STYLE_STROKE);
        pico_set_color_draw((Pico_Color){255, 255, 255});
        pico_output_draw_rect(&rect);
        pico_output_draw_oval(&oval);
        pico_output_draw_tri(&t1, &t2, &t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-02");
    }

    puts("back to fill");
    {
        pico_output_clear();
        pico_set_style(PICO_STYLE_FILL);
        pico_set_color_draw((Pico_Color){255, 255, 255});
        pico_output_draw_rect(&rect);
        pico_output_draw_oval(&oval);
        pico_output_draw_tri(&t1, &t2, &t3);
        pico_output_draw_poly(4, poly);
        _pico_check("style-03");
    }

    puts("get style");
    {
        pico_set_style(PICO_STYLE_FILL);
        assert(pico_get_style() == PICO_STYLE_FILL);
        pico_set_style(PICO_STYLE_STROKE);
        assert(pico_get_style() == PICO_STYLE_STROKE);
        pico_set_style(PICO_STYLE_FILL);
        assert(pico_get_style() == PICO_STYLE_FILL);
    }

    pico_init(0);
    return 0;
}
```

### `lua/tst/style.lua`

```lua
require 'pico.check'

pico.init(true)
pico.set.window { title="Style", dim={'!', w=640, h=480} }
pico.set.view { dim={'!', w=64, h=48} }

local rect = {'!', x=5, y=5, w=25, h=18, anc='NW'}
local oval = {'!', x=34, y=5, w=25, h=18, anc='NW'}
local t1   = {'!', x=5,  y=26, anc='NW'}
local t2   = {'!', x=5,  y=43, anc='NW'}
local t3   = {'!', x=30, y=43, anc='NW'}
local poly = {
    {'!', x=34, y=26, anc='NW'},
    {'!', x=59, y=30, anc='NW'},
    {'!', x=50, y=43, anc='NW'},
    {'!', x=38, y=43, anc='NW'},
}

do
    print("default fill")
    pico.output.clear()
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-01")
end

do
    print("stroke")
    pico.output.clear()
    pico.set.style('stroke')
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-02")
end

do
    print("back to fill")
    pico.output.clear()
    pico.set.style('fill')
    pico.set.color.draw(255, 255, 255)
    pico.output.draw.rect(rect)
    pico.output.draw.oval(oval)
    pico.output.draw.tri(t1, t2, t3)
    pico.output.draw.poly(poly)
    pico.check("style-03")
end

pico.init(false)
```

## Files to Edit

### `Makefile`

- Add `style` to the `tests` target.

### `lua/Makefile`

- Add `style` to the `tests` target.

## PR

- Target repo: `fsantanna/pico-sdl`
- Branch from: `main`
- Title: `test set/get style`
- Body must include: `fix #48`
- Include this plan file (`.claude/style.md`) in the PR
