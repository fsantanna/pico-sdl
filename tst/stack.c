#include "pico.h"
#include "_pico.h"
#include "../check.h"

/* push/pop scope test.
 *
 * stack-01: inside a push, a red box is attached under world and
 *           visible after present.
 * stack-02: after pop, the box is freed; world shows only the
 *           black clear color.
 *
 * To prove pop actually does something:
 *   - comment out pico_pop() → stack-02 still has the red box → fail
 *   - comment out pico_push() → box at depth 0, pop trips
 *     "pop without matching push" assert → abort
 *   - add a second pico_pop() → same depth assert → abort
 */
int main (void) {
    pico_init(1);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();

    pico_push();

    pico_layer_empty(
        "world", "box", 1, (Pico_Rel_Dim){'!', {16, 16}}, NULL
    );
    const char* old = pico_set_layer("box");
    pico_set_scene_dst(
        (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
    );
    pico_set_effect_color(PICO_COLOR_RED);
    pico_output_clear();
    pico_set_layer(old);

    pico_output_present(1);
    _pico_check("stack-01");
    assert(realm_get(G.realm, strlen("box")+1, "box") != NULL);

    pico_pop();
    assert(realm_get(G.realm, strlen("box")+1, "box") == NULL);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();
    pico_output_present(1);
    _pico_check("stack-02");

    pico_init(0);
    return 0;
}
