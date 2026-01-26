#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_title("Moving Around");
    pico_set_view(-1, -1,
        &(Pico_Rel_Dim){ '!', {640, 360}, NULL }, NULL,
        &(Pico_Rel_Dim){ '!', {64, 18}, NULL },
        NULL, NULL
    );
    srand(0);

    Pico_Rel_Pos pos = { '!', {32, 9}, PICO_ANCHOR_NW, NULL };
    for (int i=0; i<100; i++) {
        pico_set_color_draw((Pico_Color){0xFF,0xFF,0xFF});
        pico_output_draw_pixel(&pos);

        pico_input_delay(10);

        switch (i) {
            case 0:
                _pico_check("move-01");
                break;
            case 20:
                _pico_check("move-02");
                break;
            case 40:
                _pico_check("move-03");
                break;
            case 60:
                _pico_check("move-04");
                break;
            case 80:
                _pico_check("move-05");
                break;
        }

        //pico_set_color_draw((Pico_Color){0x00,0x00,0x00});
        //pico_output_draw_pixel(&pos);

        pos.x += rand()%3 - 1;
        pos.y += rand()%3 - 1;
    }

    pico_init(0);
}
