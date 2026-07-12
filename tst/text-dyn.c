#include "pico.h"
#include "_pico.h"

// White-box test for '~' text fingerprints: unchanged content must
// keep the SAME texture (no re-raster); changed content must
// replace it.

int main (void) {
    pico_init(1);

    Pico_Rel_Rect r = { '!', {50, 50, 0, 10}, PICO_ANCHOR_C };

    // dyn: same key + same text -> same texture (cache hit)
    {
        pico_output_draw_text_dyn("/dyn", "hello", r);
        SDL_Texture* t1 = _pico_layer_name("/dyn")->tex;
        pico_output_draw_text_dyn("/dyn", "hello", r);
        SDL_Texture* t2 = _pico_layer_name("/dyn")->tex;
        assert(t1 == t2);
    }

    // dyn: changed text -> re-raster (native width must grow)
    {
        pico_output_draw_text_dyn("/dyn", "hello", r);
        int w1 = _pico_layer_name("/dyn")->scene.dim.w;
        pico_output_draw_text_dyn("/dyn", "hello world", r);
        int w2 = _pico_layer_name("/dyn")->scene.dim.w;
        assert(w2 > w1);
    }

    // dyn: changed color -> re-raster (fp includes pencil color),
    // then stable again on repeat
    {
        pico_output_draw_text_dyn("/dyn", "hello", r);
        SDL_Texture* t1 = _pico_layer_name("/dyn")->tex;
        Pico_Color old = pico_get_pencil_color();
        pico_set_pencil_color((Pico_Color){0xFF, 0x00, 0x00, 0xFF});
        pico_output_draw_text_dyn("/dyn", "hello", r);
        SDL_Texture* t2 = _pico_layer_name("/dyn")->tex;
        assert(t2 != t1);
        pico_output_draw_text_dyn("/dyn", "hello", r);
        assert(_pico_layer_name("/dyn")->tex == t2);
        pico_set_pencil_color(old);
    }

    // layer API: pico_layer_text_mode('~') gets the same behavior
    {
        Pico_Rel_Dim d = { '!', {0, 10} };
        pico_layer_text_mode('~', NULL, "/lay", d, "abc");
        SDL_Texture* t1 = _pico_layer_name("/lay")->tex;
        pico_layer_text_mode('~', NULL, "/lay", d, "abc");
        SDL_Texture* t2 = _pico_layer_name("/lay")->tex;
        assert(t1 == t2);
        pico_layer_text_mode('~', NULL, "/lay", d, "abcd");
        SDL_Texture* t3 = _pico_layer_name("/lay")->tex;
        pico_layer_text_mode('~', NULL, "/lay", d, "abcd");
        assert(_pico_layer_name("/lay")->tex == t3);
    }

    // fix: same content twice -> shared entry keeps its texture
    // (auto-key embeds font/height/color/text)
    {
        pico_set_pencil_color((Pico_Color){1, 2, 3, 0xFF});
        pico_output_draw_text_fix("fixed", r);
        SDL_Texture* t1 = _pico_layer_name("/text/null/10/1.2.3/fixed")->tex;
        pico_output_draw_text_fix("fixed", r);
        SDL_Texture* t2 = _pico_layer_name("/text/null/10/1.2.3/fixed")->tex;
        assert(t1 == t2);
    }

    pico_init(0);
    return 0;
}
