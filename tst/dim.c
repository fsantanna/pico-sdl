#include "pico.h"

int main (void) {
    pico_init(1);
    pico_set_title("Pct-To-Pos");
    pico_output_clear();

    Pico_Pos p = pico_pos(50, 50);

    for (int i = 0; i <= 10; i++)
    {
        printf("%d%% screen size\n", 10*i);
        Pico_Dim d = pico_dim(10*i, 10*i);
        pico_output_draw_rect((Pico_Rect){p.x,p.y,d.x,d.y});
        pico_input_event(NULL, PICO_KEYDOWN);
    }

    {
        pico_output_clear();
        Pico_Rect r = {p.x,p.y,50,30};
        pico_set_color_draw((Pico_Color){100,100,100,255});
        pico_output_draw_rect(r);
        pico_set_color_draw((Pico_Color){255,255,255,255});

        for (int i = 0; i <= 11; i++)
        {
            printf("%d%% of rect\n", 10*i);
            Pico_Dim d = pico_dim_ext((Pico_Dim){r.w,r.h}, 10*i, 10*i);
            pico_output_draw_rect((Pico_Rect){p.x,p.y,d.x,d.y});
            pico_input_event(NULL, PICO_KEYDOWN);
        }
    }

    

    pico_init(0);
    return 0;
}