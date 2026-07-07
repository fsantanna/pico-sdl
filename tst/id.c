#include "pico.h"
#include <stdio.h>
#include <string.h>

int main (void) {
    pico_init(1);

    printf("=== pico_id: monotonic ===\n");
    {
        int a = pico_id();
        int b = pico_id();
        assert(a >= 1);
        assert(b == a+1);
    }

    printf("=== constructors return the stored key ===\n");
    {
        const char* k = pico_layer_empty("world", "e1", 1, (Pico_Rel_Dim){'!', {50, 50}}, NULL);
        assert(strcmp(k, "e1") == 0);
        const char* old = pico_set_layer(k);
        assert(strcmp(pico_get_layer(), "e1") == 0);
        pico_set_layer(old);
    }
    {
        const char* k = pico_layer_text(NULL, "t1", (Pico_Rel_Dim){'!', {0, 20}}, "hi");
        assert(strcmp(k, "t1") == 0);
    }
    {
        static Pico_Color buf[4] = {
            {255, 0, 0, 255}, {0, 255, 0, 255},
            {0, 0, 255, 255}, {255, 255, 0, 255}
        };
        const char* k = pico_layer_pixmap(NULL, "p1", (Pico_Abs_Dim){2, 2}, buf);
        assert(strcmp(k, "p1") == 0);
    }

    printf("=== key==NULL: auto-generates /id-N ===\n");
    {
        const char* a = pico_layer_empty(NULL, NULL, 0, (Pico_Rel_Dim){'!', {10, 10}}, NULL);
        const char* b = pico_layer_empty(NULL, NULL, 0, (Pico_Rel_Dim){'!', {10, 10}}, NULL);
        assert(strncmp(a, "/id-", 4) == 0);
        assert(strncmp(b, "/id-", 4) == 0);
        assert(strcmp(a, b) != 0);

        const char* s = pico_layer_sub(NULL, NULL, a, &(Pico_Rel_Rect){'%', {.5, .5, 1, 1}, PICO_ANCHOR_C});
        assert(strncmp(s, "/id-", 4) == 0);

        const char* h = pico_layer_screenshot(NULL, NULL, a, NULL);
        assert(strncmp(h, "/id-", 4) == 0);

        const char* t = pico_layer_text(NULL, NULL, (Pico_Rel_Dim){'!', {0, 20}}, "auto");
        assert(strncmp(t, "/id-", 4) == 0);
    }

    printf("=== image: key==NULL defaults to path ===\n");
    {
        const char* k = pico_layer_image(NULL, NULL, "../res/open.png");
        assert(strcmp(k, "../res/open.png") == 0);
    }

    printf("\n=== ALL TESTS PASSED ===\n");

    pico_init(0);
    return 0;
}
