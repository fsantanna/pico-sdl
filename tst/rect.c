#include "pico.h"

int main() {
    pico_init(1);
    pico_set_size((Pico_Dim){200,200},(Pico_Dim){20,20});

    Pico_Pos pt = pico_pos(50, 50);
    Pico_Rect r = {pt.x,pt.y,4,4};

    {
        puts("on center anchor");
        pico_set_color_draw((Pico_Color){255,255,255,255});
        pico_output_clear();
        pico_output_draw_rect(r);
        
        for (int y = r.y-r.w/2-1; y < r.y+r.h/2+2; y++) {
            for (int x = r.x-r.w/2-1; x < r.x+r.w/2+2; x++) {
                Pico_Pos pt = {x, y};
                int in = pico_is_point_in_rect(pt, r);
                puts(in ? "in  " : "out ");
                pico_set_color_draw((Pico_Color){255,0,0,255});
                pico_output_draw_pixel(pt);
                pico_input_event(NULL, PICO_KEYDOWN);
                if (in) {
                    pico_set_color_draw((Pico_Color){255,255,255,255});
                } else {
                    pico_set_color_draw((Pico_Color){0,0,0,255});
                }
                pico_output_draw_pixel(pt);
            }
        }
    }

    {
        puts("on top-left anchor");
        pico_set_anchor((Pico_Anchor){PICO_LEFT,PICO_TOP});
        pico_set_color_draw((Pico_Color){255,255,255,255});
        pico_output_clear();
        pico_output_draw_rect(r);
        
        for (int y = r.y-1; y < r.y+r.h+1; y++) {
            for (int x = r.x-1; x < r.x+r.w+1; x++) {
                Pico_Pos pt = {x, y};
                int in = pico_is_point_in_rect(pt, r);
                puts(in ? "in  " : "out ");
                pico_set_color_draw((Pico_Color){255,0,0,255});
                pico_output_draw_pixel(pt);
                pico_input_event(NULL, PICO_KEYDOWN);
                if (in) {
                    pico_set_color_draw((Pico_Color){255,255,255,255});
                } else {
                    pico_set_color_draw((Pico_Color){0,0,0,255});
                }
                pico_output_draw_pixel(pt);
            }
        }
    }

    {
        Pico_Rect r = {pt.x,pt.y,4,4};
        puts("on bottom-right anchor");
        pico_set_anchor((Pico_Anchor){PICO_RIGHT,PICO_BOTTOM});
        pico_set_color_draw((Pico_Color){255,255,255,255});
        pico_output_clear();
        pico_output_draw_rect(r);
        
        for (int y = r.y-r.h; y < r.y+2; y++) {
            for (int x = r.x-r.w; x < r.x+2; x++) {
                Pico_Pos pt = {x, y};
                int in = pico_is_point_in_rect(pt, r);
                puts(in ? "in  " : "out ");
                pico_set_color_draw((Pico_Color){255,0,0,255});
                pico_output_draw_pixel(pt);
                pico_input_event(NULL, PICO_KEYDOWN);
                if (in) {
                    pico_set_color_draw((Pico_Color){255,255,255,255});
                } else {
                    pico_set_color_draw((Pico_Color){0,0,0,255});
                }
                pico_output_draw_pixel(pt);
            }
        }
    }

    pico_init(0);
    return 0;
}