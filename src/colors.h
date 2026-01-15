#ifndef PICO_COLORS_H
#define PICO_COLORS_H

/// @defgroup Predefined Colors
/// @brief Predefined color constants for common colors.
/// @{

// Grayscale
#define PICO_COLOR_BLACK   ((Pico_Color) {0, 0, 0})
#define PICO_COLOR_WHITE   ((Pico_Color) {255, 255, 255})
#define PICO_COLOR_GRAY    ((Pico_Color) {128, 128, 128})
#define PICO_COLOR_SILVER  ((Pico_Color) {192, 192, 192})

// Primary colors
#define PICO_COLOR_RED     ((Pico_Color) {255, 0, 0})
#define PICO_COLOR_GREEN   ((Pico_Color) {0, 255, 0})
#define PICO_COLOR_BLUE    ((Pico_Color) {0, 0, 255})

// Secondary colors
#define PICO_COLOR_YELLOW  ((Pico_Color) {255, 255, 0})
#define PICO_COLOR_CYAN    ((Pico_Color) {0, 255, 255})
#define PICO_COLOR_MAGENTA ((Pico_Color) {255, 0, 255})

// Common colors
#define PICO_COLOR_ORANGE  ((Pico_Color) {255, 165, 0})
#define PICO_COLOR_PURPLE  ((Pico_Color) {128, 0, 128})
#define PICO_COLOR_PINK    ((Pico_Color) {255, 192, 203})
#define PICO_COLOR_BROWN   ((Pico_Color) {165, 42, 42})
#define PICO_COLOR_LIME    ((Pico_Color) {0, 255, 0})
#define PICO_COLOR_TEAL    ((Pico_Color) {0, 128, 128})
#define PICO_COLOR_NAVY    ((Pico_Color) {0, 0, 128})
#define PICO_COLOR_MAROON  ((Pico_Color) {128, 0, 0})
#define PICO_COLOR_OLIVE   ((Pico_Color) {128, 128, 0})

/// @}

#endif // PICO_COLORS_H
