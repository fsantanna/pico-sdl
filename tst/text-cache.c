#include "pico.h"
#include "_pico.h"

// White-box test for the text cache behind pico_output_draw_text /
// pico_get_text: unchanged content must keep the SAME texture (no
// re-raster); any change in (font, height, color, text) must produce
// a different entry. Entries are owned by pico and released at
// pico_close.

int main (void) {
    pico_open(NULL);

    Pico_Rel_Rect r = { '!', {50, 50, 0, 10}, PICO_ANCHOR_C };

    // same content twice -> same entry, same texture (cache hit)
    {
        pico_output_draw_text("hello", r);
        Pico_Layer* l1 = _pico_layer_text_fp(10, "hello");
        SDL_Texture* t1 = l1->tex;
        pico_output_draw_text("hello", r);
        Pico_Layer* l2 = _pico_layer_text_fp(10, "hello");
        assert(l1 == l2);
        assert(l2->tex == t1);
        assert(l1->cached);
    }

    // changed text -> another entry (native width must grow)
    {
        Pico_Layer* l1 = _pico_layer_text_fp(10, "hello");
        pico_output_draw_text("hello world", r);
        Pico_Layer* l2 = _pico_layer_text_fp(10, "hello world");
        assert(l1 != l2);
        assert(l2->scene.dim.w > l1->scene.dim.w);
    }

    // changed color -> another entry (fingerprint includes pencil
    // color), then stable again on repeat
    {
        Pico_Layer* l1 = _pico_layer_text_fp(10, "hello");
        Pico_Color old = pico_get_pencil_color();
        pico_set_pencil_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_text("hello", r);
        Pico_Layer* l2 = _pico_layer_text_fp(10, "hello");
        assert(l2 != l1);
        pico_output_draw_text("hello", r);
        assert(_pico_layer_text_fp(10, "hello") == l2);
        pico_set_pencil_color(old);
        assert(_pico_layer_text_fp(10, "hello") == l1);
    }

    // changed height -> another entry
    {
        Pico_Layer* l1 = _pico_layer_text_fp(10, "hello");
        Pico_Layer* l2 = _pico_layer_text_fp(20, "hello");
        assert(l1 != l2);
        assert(l2->scene.dim.h > l1->scene.dim.h);
    }

    // changed font -> another entry; same path -> same cached font
    {
        Pico_Layer* l1 = _pico_layer_text_fp(10, "hello");
        pico_set_pencil_font_path("../res/DejaVuSans.ttf");
        Pico_Font* f1 = pico_get_pencil_font();
        Pico_Layer* l2 = _pico_layer_text_fp(10, "hello");
        assert(l1 != l2);
        pico_set_pencil_font_path("../res/DejaVuSans.ttf");
        assert(pico_get_pencil_font() == f1);
        assert(_pico_layer_text_fp(10, "hello") == l2);

        // a loaded font is a distinct font, even for the same file
        Pico_Font* f2 = pico_font_load("../res/DejaVuSans.ttf");
        assert(f2 != f1);
        pico_set_pencil_font(f2);
        Pico_Layer* l3 = _pico_layer_text_fp(10, "hello");
        assert(l3!=l2 && l3!=l1);

        pico_set_pencil_font(NULL);
        assert(_pico_layer_text_fp(10, "hello") == l1);
        pico_font_free(f2);
    }

    // measuring shares the cache with drawing
    {
        Pico_Rel_Dim d = { '!', {0, 10} };
        Pico_Abs_Dim abs = pico_get_text(&d, "measure");
        Pico_Layer* l = _pico_layer_text_fp(10, "measure");
        assert(abs.w==l->scene.dim.w && abs.h==l->scene.dim.h);
    }

    // user-owned text layers bypass the cache: one texture each
    {
        Pico_Rel_Dim d = { '!', {0, 10} };
        Pico_Layer* a = pico_layer_text(NULL, d, "abc");
        Pico_Layer* b = pico_layer_text(NULL, d, "abc");
        assert(a!=b && a->tex!=b->tex);
        assert(!a->cached && G.live==2);
        pico_layer_free(a);
        pico_layer_free(b);
        assert(G.live == 0);
    }

    // cached entries need no free: pico_close releases them
    pico_close();
    return 0;
}
