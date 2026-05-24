#include "pico.h"
#include "../check.h"

/* Stack scope regression: replace a parented layer via realm mode '~'.
 *
 * Before detach-on-free: parent's hier.dn.fst/lst/nxt still cache the
 * old child's name pointer (strdup'd, freed by _pico_mem_free_layer).
 * The next draw walk hashes garbage in realm_get and crashes at
 * src/layer.c:39 assert(CUR != NULL).
 *
 * After the fix: free splices the child out of hier.up's child list
 * before releasing it. The re-attach from the '~' path re-inserts the
 * new layer cleanly; the walk shows a single 16x16 child under world.
 */
int main (void) {
    pico_init(1);

    pico_set_effect_color(PICO_COLOR_BLACK);
    pico_output_clear();

    // initial child under "world"
    pico_layer_empty(
        "world", "L", 1, (Pico_Rel_Dim){'!', {8, 8}}, NULL
    );

    // '~' replace: frees the old "L" (and its strdup'd name) and
    // re-attaches a new "L" with a fresh name pointer
    pico_layer_empty_mode(
        '~', "world", "L", 1, (Pico_Rel_Dim){'!', {16, 16}}, NULL
    );

    // paint the (new) child so it is visible in the check image
    const char* old = pico_set_layer("L");
    pico_set_scene_dst(
        (Pico_Rel_Rect){'%', {0.5, 0.5, 0.5, 0.5}, PICO_ANCHOR_C}
    );
    pico_set_effect_color(PICO_COLOR_RED);
    pico_output_clear();
    pico_set_layer(old);

    pico_output_present(1);
    _pico_check("stack-01");

    pico_init(0);
    return 0;
}
