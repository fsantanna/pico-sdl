#include "colors.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

/// @defgroup Predefined Colors
/// @brief Predefined color constants for common colors.
/// @{

// Grayscale
const Pico_Color PICO_COLOR_BLACK   = {0,   0,   0,   0xFF};
const Pico_Color PICO_COLOR_WHITE   = {255, 255, 255, 0xFF};
const Pico_Color PICO_COLOR_GRAY    = {128, 128, 128, 0xFF};
const Pico_Color PICO_COLOR_SILVER  = {192, 192, 192, 0xFF};

// Primary colors
const Pico_Color PICO_COLOR_RED     = {255, 0,   0,   0xFF};
const Pico_Color PICO_COLOR_GREEN   = {0,   255, 0,   0xFF};
const Pico_Color PICO_COLOR_BLUE    = {0,   0,   255, 0xFF};

// Secondary colors
const Pico_Color PICO_COLOR_YELLOW  = {255, 255, 0,   0xFF};
const Pico_Color PICO_COLOR_CYAN    = {0,   255, 255, 0xFF};
const Pico_Color PICO_COLOR_MAGENTA = {255, 0,   255, 0xFF};

// Common colors
const Pico_Color PICO_COLOR_ORANGE  = {255, 165, 0,   0xFF};
const Pico_Color PICO_COLOR_PURPLE  = {128, 0,   128, 0xFF};
const Pico_Color PICO_COLOR_PINK    = {255, 192, 203, 0xFF};
const Pico_Color PICO_COLOR_BROWN   = {165, 42,  42,  0xFF};
const Pico_Color PICO_COLOR_LIME    = {0,   255, 0,   0xFF};
const Pico_Color PICO_COLOR_TEAL    = {0,   128, 128, 0xFF};
const Pico_Color PICO_COLOR_NAVY    = {0,   0,   128, 0xFF};
const Pico_Color PICO_COLOR_MAROON  = {128, 0,   0,   0xFF};
const Pico_Color PICO_COLOR_OLIVE   = {128, 128, 0,   0xFF};

// Special colors
const Pico_Color PICO_COLOR_TRANSPARENT = {0, 0, 0, 0};

/// @}

Pico_Color pico_color_darker (Pico_Color clr, float pct) {
    if (pct < 0) {
        return pico_color_lighter(clr, -pct);
    }
    float X = MAX(0, 1-pct);
    return (Pico_Color) { clr.r*X, clr.g*X, clr.b*X, clr.a };
}

Pico_Color pico_color_lighter (Pico_Color clr, float pct) {
    if (pct < 0) {
        return pico_color_darker(clr, -pct);
    }
    return (Pico_Color) {
        (clr.r + (255 - clr.r) * pct),
        (clr.g + (255 - clr.g) * pct),
        (clr.b + (255 - clr.b) * pct),
        clr.a
    };
}

Pico_Color pico_color_mix (Pico_Color c1, Pico_Color c2) {
    return (Pico_Color) {
        (c1.r + c2.r) / 2,
        (c1.g + c2.g) / 2,
        (c1.b + c2.b) / 2,
        (c1.a + c2.a) / 2
    };
}

Pico_Color pico_color_alpha (Pico_Color clr, Uint8 a) {
    clr.a = a;
    return clr;
}

Pico_Color pico_color_hex (uint32_t hex) {
    if (hex > 0xFFFFFF) {
        uint8_t r = (hex >> 24) & 0xFF;
        uint8_t g = (hex >> 16) & 0xFF;
        uint8_t b = (hex >> 8)  & 0xFF;
        uint8_t a =  hex        & 0xFF;
        return (Pico_Color) { r, g, b, a };
    } else {
        uint8_t r = (hex >> 16) & 0xFF;
        uint8_t g = (hex >> 8)  & 0xFF;
        uint8_t b =  hex        & 0xFF;
        return (Pico_Color) { r, g, b, 0xFF };
    }
}
