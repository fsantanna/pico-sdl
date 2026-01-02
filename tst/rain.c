#include "pico.h"
#include <stdlib.h>
#include <time.h>

typedef struct {
    int delay;
    Pico_Rect_Pct r;
} Drop;

#define DROP_COUNT 100
#define DROP_WIDTH 0.005
#define DROP_HEIGHT 0.02
#define DROP_SPEED 0.005
#define MAX_DELAY 100

void init_drop(Drop* drop) {
    drop->delay = rand() % MAX_DELAY;
    drop->r.x = (float)rand() / RAND_MAX;
    drop->r.y = -DROP_HEIGHT;
    drop->r.w = DROP_WIDTH;
    drop->r.h = DROP_HEIGHT;
    drop->r.anchor = (Pico_Pct){PICO_ANCHOR_CENTER, PICO_ANCHOR_BOTTOM};
    drop->r.up = NULL;
}

int main(void) {
    srand(time(NULL));
    pico_init(1);
    pico_set_title("Rain Simulation");

    Drop drops[DROP_COUNT];
    for (int i = 0; i < DROP_COUNT; i++) {
        init_drop(&drops[i]);
    }

    pico_set_color_clear((Pico_Color){20, 20, 40});
    pico_set_color_draw((Pico_Color){100, 100, 255});

    while (1) {
        Pico_Event e;
        if (pico_input_event_timeout(&e, PICO_QUIT, 16)) {
            break;
        }

        pico_output_clear();

        for (int i = 0; i < DROP_COUNT; i++) {
            Drop* drop = &drops[i];

            if (drop->delay > 0) {
                drop->delay--;
            } else {
                drop->r.y += DROP_SPEED;

                if (drop->r.y >= 1.0) {
                    init_drop(drop);
                } else {
                    pico_output_draw_rect_pct(&drop->r);
                }
            }
        }
    }

    pico_init(0);
    return 0;
}
