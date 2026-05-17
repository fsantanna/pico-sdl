#ifndef PICO_COLORS_H
#define PICO_COLORS_H

#include <SDL2/SDL.h>

/// @brief RGBA color with per-pixel alpha channel.
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Pico_Color;

extern const Pico_Color PICO_COLOR_BLACK;
extern const Pico_Color PICO_COLOR_WHITE;
extern const Pico_Color PICO_COLOR_GRAY;
extern const Pico_Color PICO_COLOR_SILVER;
extern const Pico_Color PICO_COLOR_RED;
extern const Pico_Color PICO_COLOR_GREEN;
extern const Pico_Color PICO_COLOR_BLUE;
extern const Pico_Color PICO_COLOR_YELLOW;
extern const Pico_Color PICO_COLOR_CYAN;
extern const Pico_Color PICO_COLOR_MAGENTA;
extern const Pico_Color PICO_COLOR_ORANGE;
extern const Pico_Color PICO_COLOR_PURPLE;
extern const Pico_Color PICO_COLOR_PINK;
extern const Pico_Color PICO_COLOR_BROWN;
extern const Pico_Color PICO_COLOR_LIME;
extern const Pico_Color PICO_COLOR_TEAL;
extern const Pico_Color PICO_COLOR_NAVY;
extern const Pico_Color PICO_COLOR_MAROON;
extern const Pico_Color PICO_COLOR_OLIVE;

extern const Pico_Color PICO_COLOR_TRANSPARENT;

Pico_Color pico_color_darker  (Pico_Color clr, float pct);
Pico_Color pico_color_lighter (Pico_Color clr, float pct);
Pico_Color pico_color_mix     (Pico_Color c1, Pico_Color c2);
Pico_Color pico_color_alpha   (Pico_Color clr, Uint8 a);
Pico_Color pico_color_hex     (uint32_t hex);

#endif
