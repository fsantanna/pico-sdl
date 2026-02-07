#ifndef PICO_COLORS_H
#define PICO_COLORS_H

/// @brief RGB color without alpha channel.
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Pico_Color;

/// @brief RGBA color with per-pixel alpha channel.
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Pico_Color_A;

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

extern const Pico_Color_A PICO_COLOR_TRANSPARENT;

#endif // PICO_COLORS_H

#ifdef PICO_COLORS_C

/// @defgroup Predefined Colors
/// @brief Predefined color constants for common colors.
/// @{

// Grayscale
const Pico_Color PICO_COLOR_BLACK   = {0, 0, 0};
const Pico_Color PICO_COLOR_WHITE   = {255, 255, 255};
const Pico_Color PICO_COLOR_GRAY    = {128, 128, 128};
const Pico_Color PICO_COLOR_SILVER  = {192, 192, 192};

// Primary colors
const Pico_Color PICO_COLOR_RED     = {255, 0, 0};
const Pico_Color PICO_COLOR_GREEN   = {0, 255, 0};
const Pico_Color PICO_COLOR_BLUE    = {0, 0, 255};

// Secondary colors
const Pico_Color PICO_COLOR_YELLOW  = {255, 255, 0};
const Pico_Color PICO_COLOR_CYAN    = {0, 255, 255};
const Pico_Color PICO_COLOR_MAGENTA = {255, 0, 255};

// Common colors
const Pico_Color PICO_COLOR_ORANGE  = {255, 165, 0};
const Pico_Color PICO_COLOR_PURPLE  = {128, 0, 128};
const Pico_Color PICO_COLOR_PINK    = {255, 192, 203};
const Pico_Color PICO_COLOR_BROWN   = {165, 42, 42};
const Pico_Color PICO_COLOR_LIME    = {0, 255, 0};
const Pico_Color PICO_COLOR_TEAL    = {0, 128, 128};
const Pico_Color PICO_COLOR_NAVY    = {0, 0, 128};
const Pico_Color PICO_COLOR_MAROON  = {128, 0, 0};
const Pico_Color PICO_COLOR_OLIVE   = {128, 128, 0};

// Special colors
const Pico_Color_A PICO_COLOR_TRANSPARENT = {0, 0, 0, 0};

/// @}

#endif // PICO_COLORS_C
