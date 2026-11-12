#include "pico.h"
#include "_pico.h"
#include "../check.h"

/* pico_layer_free test.
 *
 * free-01: a red box is attached under world and visible after
 *          present.
 * free-02: after pico_layer_free, the box is detached from world and
 *          released; world shows only the black clear color.
 *
 * To prove the free actually does something:
 *   - comment out pico_layer_free() → free-02 still has the red box
 *     → fail
 */
int main (void) {
    pico_open(NULL);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();

    assert(G.live == 0);
    Pico_Layer* box = pico_layer_empty (
        pico_layer_world(), 1, (Pico_Rel_Dim){'!', {16, 16}}, NULL
    );
    assert(G.live == 1);
    Pico_Layer* old = pico_set_layer(box);
    pico_set_scene_dst(
        (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
    );
    pico_set_effect_color(PICO_COLOR_RED);
    pico_output_clear();
    pico_set_layer(old);

    pico_output_present(1);
    _pico_check("free-01");
    assert(pico_layer_world()->hier.dn.fst == box);
    assert(box->hier.up == pico_layer_world());

    pico_layer_free(box);
    assert(pico_layer_world()->hier.dn.fst == NULL);
    assert(pico_layer_world()->hier.dn.lst == NULL);
    assert(G.live == 0);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_present(1);
    _pico_check("free-02");

    // siblings: freeing the middle / first / last child keeps the chain
    {
        Pico_Rel_Dim d = {'!', {4, 4}};
        Pico_Layer* W = pico_layer_world();
        Pico_Layer* a = pico_layer_empty(W, 1, d, NULL);
        Pico_Layer* b = pico_layer_empty(W, 1, d, NULL);
        Pico_Layer* c = pico_layer_empty(W, 1, d, NULL);
        assert(W->hier.dn.fst==a && a->hier.nxt==b && b->hier.nxt==c);
        assert(W->hier.dn.lst == c);

        pico_layer_free(b);
        assert(W->hier.dn.fst==a && a->hier.nxt==c && W->hier.dn.lst==c);
        pico_layer_free(c);
        assert(W->hier.dn.fst==a && a->hier.nxt==NULL && W->hier.dn.lst==a);
        pico_layer_free(a);
        assert(W->hier.dn.fst==NULL && W->hier.dn.lst==NULL);
    }

    // sub-layers: the source counts its borrowers
    {
        Pico_Layer* src = pico_layer_empty (
            NULL, 0, (Pico_Rel_Dim){'!', {8, 8}}, NULL
        );
        Pico_Layer* sub = pico_layer_sub (
            NULL, src, &(Pico_Rel_Rect){'%', {.5, .5, .5, .5}, PICO_ANCHOR_C}
        );
        assert(src->subs == 1);
        pico_layer_free(sub);
        assert(src->subs == 0);
        pico_layer_free(src);
    }

    // fonts and sounds are counted as well
    {
        Pico_Font* font = pico_font_load("../res/DejaVuSans.ttf");
        Pico_Sound* sound = pico_sound_load("../res/start.wav");
        assert(G.live == 2);
        pico_output_sound(sound);
        pico_sound_free(sound);
        pico_font_free(font);
        assert(G.live == 0);
    }

    pico_close();
    return 0;
}
