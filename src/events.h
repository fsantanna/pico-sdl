#ifndef PICO_EVENTS_H
#define PICO_EVENTS_H

#include <SDL2/SDL_events.h>

typedef enum PICO_EVENT {
    PICO_EVENT_NONE              = -1,
    PICO_EVENT_ANY               =  0,
    PICO_EVENT_QUIT,
    PICO_EVENT_WINDOW,
    PICO_EVENT_KEY_DN,
    PICO_EVENT_KEY_UP,
    PICO_EVENT_MOUSE_MOTION,
    PICO_EVENT_MOUSE_BUTTON_DN,
    PICO_EVENT_MOUSE_BUTTON_UP,
} PICO_EVENT;

typedef struct {
    int w, h;
} Pico_Window;

typedef struct {
    int key;
    unsigned ctrl  : 1;
    unsigned shift : 1;
    unsigned alt   : 1;
} Pico_Keyboard;

typedef struct {
    char mode;              // 'w', '!', '%', '#'
    float x, y;
    unsigned left   : 1;
    unsigned right  : 1;
    unsigned middle : 1;
} Pico_Mouse;

typedef struct {
    PICO_EVENT type;
    union {
        Pico_Window   window;
        Pico_Keyboard keyboard;
        Pico_Mouse    mouse;
    };
} Pico_Event;

#endif // PICO_EVENTS_H
