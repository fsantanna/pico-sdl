// scene.clip scissors draw.rect in PLAIN mode (check 01) but is IGNORED
// in EXPERT mode (check 02 comes out full blue instead of red+blue-box).
//
// Build/run:  make T=clip-expert gen   (inspect tst/out/clip-expert-*.png)

#include "pico.h"
#include "../check.h"

int main (void) {
    Pico_Rel_Rect full = { '%', {0.5,0.5, 1.0,1.0}, PICO_ANCHOR_C };
    Pico_Rel_Rect box  = { '%', {0.5,0.5, 0.5,0.5}, PICO_ANCHOR_C };

    pico_init(1);
    pico_set_window_title("Clip x Expert");
    pico_set_pencil_style(PICO_STYLE_FILL);

    // PLAIN: red full, then blue full clipped to a centered half-box
    pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00,0xFF});
    pico_output_draw_rect(full);
    pico_set_scene_clip(box);
    pico_set_pencil_color((Pico_Color){0x00,0x00,0xFF,0xFF});
    pico_output_draw_rect(full);
    _pico_check("clip-expert-01");   // expected: red with a centered blue box

    // EXPERT: identical sequence, app-style manual clear/present
    pico_set_expert(1, 0);
    pico_set_scene_clip(full);
    pico_output_clear();
    pico_set_pencil_color((Pico_Color){0xFF,0x00,0x00,0xFF});
    pico_output_draw_rect(full);
    pico_set_scene_clip(box);
    pico_set_pencil_color((Pico_Color){0x00,0x00,0xFF,0xFF});
    pico_output_draw_rect(full);
    pico_output_present(1);
    _pico_check("clip-expert-02");   // BUG: full blue -> clip not applied

    pico_init(0);
    return 0;
}
