#include "pico.h"
#include "../check.h"

int main (void) {
    pico_init(1);
    pico_set_window_title("Text Sizes");
    pico_set_layer("window");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {500, 500} });
    pico_set_layer("world");
    pico_set_scene_dim((Pico_Rel_Dim){ '!', {500, 500} });

    pico_output_clear();
    pico_set_pencil_font("../res/DejaVuSans.ttf");
    pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});

    // 12 lines, small to big, stacked top-to-bottom
    {
        int y = 5;
        for (int i = 1; i <= 12; i++) {
            int h = 5 * i;
            Pico_Rel_Rect r = { '!', {5, y, 0, h}, PICO_ANCHOR_NW };
            pico_output_draw_text_fix("Hello World!", r);
            y += h + 4;
        }
        _pico_check("text-sizes-01");
    }

    // repro of intro.atm flicker: typewriter, redrawn (cleared) each
    // frame as the prefix grows. r1 '!' and r2 '%' at the SAME integer
    // height (22px = 0.044*500). 22 still jitters: ptsize lands the
    // glyph surface at H0 != 22, so box_h != H0 and the auto-width
    // re-quantises per char (20 happens to match H0 -> stays stable).
    // Stacked so the boxes share a border (r1 bottom = r2 top = y211).
    {
        //const char* msg = "The quick brown fox jumps over the lazy dog. Yes!";
        const char* msg = "For a long time, the Pingus have lived hapilly.";
        Pico_Rel_Rect r1 = { '!', {20, 200, 0, 22}, PICO_ANCHOR_W };
        Pico_Rel_Rect r2 = { '%', {0.04, 0.444, 0, 0.044}, PICO_ANCHOR_W };
        char buf[128];
        int n = strlen(msg);
        for (int i = 1; i <= n; i++) {
            memcpy(buf, msg, i);
            buf[i] = '\0';
            pico_output_clear();

            // texts (white)
            pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
            pico_output_draw_text_fix(buf, r1);
            pico_output_draw_text_fix(buf, r2);

            // enclosing boxes (red stroke), sized to the measured text
            pico_set_pencil_color((Pico_Color){255, 0, 0, 0xFF});
            pico_set_pencil_style(PICO_STYLE_STROKE);
            Pico_Rel_Dim m1 = { '!', {0, 22} };
            pico_get_text(&m1, buf);
            pico_output_draw_rect((Pico_Rel_Rect){ '!', {20, 200, m1.w, 22}, PICO_ANCHOR_W });
            Pico_Rel_Dim m2 = { '%', {0, 0.044} };
            pico_get_text(&m2, buf);
            pico_output_draw_rect((Pico_Rel_Rect){ '%', {0.04, 0.444, m2.w, 0.044}, PICO_ANCHOR_W });
            pico_set_pencil_style(PICO_STYLE_FILL);

            // capture start / middle / end of the reveal
            if (i == 1)   _pico_check("text-sizes-02");
            if (i == n/2) _pico_check("text-sizes-03");
            if (i == n)   _pico_check("text-sizes-04");

            pico_input_delay(50);
        }
    }

    // vertical-snap harness: the SAME '!' typewriter reveal at three
    // y-anchors (NW top, C middle, SE bottom). A fixed green guide
    // marks each anchor's reference y; a red box shows the measured
    // text. The reveal is captured at three lengths -- before, at, and
    // after the first descender ('g' in "long", length 10). NW should
    // stay glued to its guide across all three; if C and SE drift off
    // their guide when the descender enters, the box height is
    // content-dependent and the vertical snap is real.
    {
        const char* msg = "For a long time, the Pingus have lived hapilly.";
        int h = 22;
        Pico_Anchor an[3] = { PICO_ANCHOR_NW, PICO_ANCHOR_C, PICO_ANCHOR_SE };
        int ax[3] = { 20, 250, 480 };
        int ay[3] = { 100, 250, 400 };
        int lens[3] = { 6, 10, (int)strlen(msg) };
        char buf[128];
        for (int c = 0; c < 3; c++) {
            int L = lens[c];
            memcpy(buf, msg, L);
            buf[L] = '\0';
            pico_output_clear();
            for (int a = 0; a < 3; a++) {
                // fixed green guide at the anchor reference y
                pico_set_pencil_color((Pico_Color){0, 255, 0, 0xFF});
                pico_output_draw_line((Pico_Rel_Pos){ '!', {5, ay[a]} }, (Pico_Rel_Pos){ '!', {495, ay[a]} });

                // revealed text (white)
                pico_set_pencil_color((Pico_Color){255, 255, 255, 0xFF});
                pico_output_draw_text_fix(buf, (Pico_Rel_Rect){ '!', {ax[a], ay[a], 0, h}, an[a] });

                // measured box (red stroke)
                Pico_Rel_Dim m = { '!', {0, h} };
                pico_get_text(&m, buf);
                pico_set_pencil_color((Pico_Color){255, 0, 0, 0xFF});
                pico_set_pencil_style(PICO_STYLE_STROKE);
                pico_output_draw_rect((Pico_Rel_Rect){ '!', {ax[a], ay[a], m.w, h}, an[a] });
                pico_set_pencil_style(PICO_STYLE_FILL);
                pico_input_delay(50);
            }
            if (c == 0) _pico_check("text-sizes-05");
            if (c == 1) _pico_check("text-sizes-06");
            if (c == 2) _pico_check("text-sizes-07");
        }
    }

    // Y-snap repro (port conditions from intro.atm): '%' mode, small
    // fractional height, W (center-y) anchor. Overlay two reveal
    // lengths that share the prefix "For a lo" at the SAME anchor:
    // length 8 (no descender) in RED, length 10 ("For a long", the 'g'
    // descender) in CYAN. If the box height is content-independent the
    // shared caps coincide exactly (cyan fully covers red). A 1px RED
    // fringe above/below the cyan "For a lo" IS the vertical snap.
    {
        const char* msg = "But then one day, things began to change slowly:";
        Pico_Rel_Rect r = { '%', {0.15, 0.60, 0, 0.025}, PICO_ANCHOR_W };
        char buf[128];
        pico_output_clear();

        // green guide at the anchor y
        pico_set_pencil_color((Pico_Color){0, 255, 0, 0xFF});
        pico_output_draw_line((Pico_Rel_Pos){ '%', {0.0, 0.60} }, (Pico_Rel_Pos){ '%', {1.0, 0.60} });

        // length 8 "For a lo" (no descender) in RED
        pico_set_pencil_color((Pico_Color){255, 0, 0, 0xFF});
        memcpy(buf, msg, 128);
        buf[16] = '\0';
        pico_output_draw_text_fix(buf, r);

        _pico_check("text-sizes-08");

        // length 10 "For a long" ('g' descender) in CYAN, same anchor
        pico_set_pencil_color((Pico_Color){0, 255, 255, 0xFF});
        memcpy(buf, msg, 128);
        buf[strlen(msg)+1] = '\0';
        pico_output_draw_text_fix(buf, r);

        _pico_check("text-sizes-09");
    }

    pico_init(0);
    return 0;
}
