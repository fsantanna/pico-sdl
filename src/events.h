#ifndef PICO_EVENTS_H
#define PICO_EVENTS_H

#include <SDL2/SDL_events.h>

typedef SDL_Event Pico_Event;

typedef enum PICO_EVENT {
    PICO_ANY = 0, /**< Wait for any event */

    /* Application events */
    PICO_QUIT = SDL_QUIT,                                       /**< User-requested quit */
    PICO_APP_TERMINATING = SDL_APP_TERMINATING,                 /**< The application is being terminated by the OS */
    PICO_APP_LOWMEMORY = SDL_APP_LOWMEMORY,                     /**< The application is low on memory, free memory if possible. */
    PICO_APP_WILLENTERBACKGROUND = SDL_APP_WILLENTERBACKGROUND, /**< The application is about to enter the background */
    PICO_APP_DIDENTERBACKGROUND = SDL_APP_DIDENTERBACKGROUND,   /**< The application did enter the background and may not get CPU for some time */
    PICO_APP_WILLENTERFOREGROUND = SDL_APP_WILLENTERFOREGROUND, /**< The application is about to enter the foreground */
    PICO_APP_DIDENTERFOREGROUND = SDL_APP_DIDENTERFOREGROUND,   /**< The application is now interactive */

    PICO_LOCALECHANGED = SDL_LOCALECHANGED, /**< The user's locale preferences have changed. */

    /* Display events */
    PICO_DISPLAYEVENT = SDL_DISPLAYEVENT, /**< Display state change */

    /* Window events */
    PICO_WINDOWEVENT = SDL_WINDOWEVENT, /**< Window state change */
    PICO_SYSWMEVENT = SDL_SYSWMEVENT,   /**< System specific event */

    /* Keyboard events */
    PICO_KEYDOWN = SDL_KEYDOWN, /**< Key pressed */
    PICO_KEYUP = SDL_KEYUP, /**< Key released */
    PICO_TEXTEDITING = SDL_TEXTEDITING, /**< Keyboard text editing (composition) */
    PICO_TEXTINPUT = SDL_TEXTINPUT, /**< Keyboard text input */
    PICO_KEYMAPCHANGED = SDL_KEYMAPCHANGED,  /**< Keymap changed due to input language or keyboard layout. */
    //PICO_TEXTEDITING_EXT = SDL_TEXTEDITING_EXT, /**< Extended keyboard text editing (composition) */

    /* Mouse events */
    PICO_MOUSEMOTION = SDL_MOUSEMOTION,         /**< Mouse moved */
    PICO_MOUSEBUTTONDOWN = SDL_MOUSEBUTTONDOWN, /**< Mouse button pressed */
    PICO_MOUSEBUTTONUP = SDL_MOUSEBUTTONUP,     /**< Mouse button released */
    PICO_MOUSEWHEEL = SDL_MOUSEWHEEL,           /**< Mouse wheel motion */

    /* Joystick events */
    PICO_JOYAXISMOTION = SDL_JOYAXISMOTION,         /**< Joystick axis motion */
    PICO_JOYBALLMOTION = SDL_JOYBALLMOTION,         /**< Joystick trackball motion */
    PICO_JOYHATMOTION = SDL_JOYHATMOTION,           /**< Joystick hat position change */
    PICO_JOYBUTTONDOWN = SDL_JOYBUTTONDOWN,         /**< Joystick button pressed */
    PICO_JOYBUTTONUP = SDL_JOYBUTTONUP,             /**< Joystick button released */
    PICO_JOYDEVICEADDED = SDL_JOYDEVICEADDED,       /**< A new joystick has been inserted into the system */
    PICO_JOYDEVICEREMOVED = SDL_JOYDEVICEREMOVED,   /**< An opened joystick has been removed */
    //PICO_JOYBATTERYUPDATED = SDL_JOYBATTERYUPDATED, /**< Joystick battery level change */

    /* Game controller events */
    PICO_CONTROLLERAXISMOTION = SDL_CONTROLLERAXISMOTION,                 /**< Game controller axis motion */
    PICO_CONTROLLERBUTTONDOWN = SDL_CONTROLLERBUTTONDOWN,                 /**< Game controller button pressed */
    PICO_CONTROLLERBUTTONUP = SDL_CONTROLLERBUTTONUP,                     /**< Game controller button released */
    PICO_CONTROLLERDEVICEADDED = SDL_CONTROLLERDEVICEADDED,               /**< A new Game controller has been inserted into the system */
    PICO_CONTROLLERDEVICEREMOVED = SDL_CONTROLLERDEVICEREMOVED,           /**< An opened Game controller has been removed */
    PICO_CONTROLLERDEVICEREMAPPED = SDL_CONTROLLERDEVICEREMAPPED,         /**< The controller mapping was updated */
    PICO_CONTROLLERTOUCHPADDOWN = SDL_CONTROLLERTOUCHPADDOWN,             /**< Game controller touchpad was touched */
    PICO_CONTROLLERTOUCHPADMOTION = SDL_CONTROLLERTOUCHPADMOTION,         /**< Game controller touchpad finger was moved */
    PICO_CONTROLLERTOUCHPADUP = SDL_CONTROLLERTOUCHPADUP,                 /**< Game controller touchpad finger was lifted */
    PICO_CONTROLLERSENSORUPDATE = SDL_CONTROLLERSENSORUPDATE,             /**< Game controller sensor was updated */
    //PICO_CONTROLLERSTEAMHANDLEUPDATED = SDL_CONTROLLERSTEAMHANDLEUPDATED, /**< Game controller Steam handle has changed */

    /* Touch events */
    PICO_FINGERDOWN = SDL_FINGERDOWN,
    PICO_FINGERUP = SDL_FINGERUP,
    PICO_FINGERMOTION = SDL_FINGERMOTION,

    /* Gesture events */
    PICO_DOLLARGESTURE = SDL_DOLLARGESTURE,
    PICO_DOLLARRECORD = SDL_DOLLARRECORD,
    PICO_MULTIGESTURE = SDL_MULTIGESTURE,

    /* Clipboard events */
    PICO_CLIPBOARDUPDATE = SDL_CLIPBOARDUPDATE, /**< The clipboard or primary selection changed */

    /* Drag and drop events */
    PICO_DROPFILE = SDL_DROPFILE,         /**< The system requests a file open */
    PICO_DROPTEXT = SDL_DROPTEXT,         /**< text/plain drag-and-drop event */
    PICO_DROPBEGIN = SDL_DROPBEGIN,       /**< A new set of drops is beginning (NULL filename) */
    PICO_DROPCOMPLETE = SDL_DROPCOMPLETE, /**< Current set of drops is now complete (NULL filename) */

    /* Audio hotplug events */
    PICO_AUDIODEVICEADDED = SDL_AUDIODEVICEADDED,     /**< A new audio device is available */
    PICO_AUDIODEVICEREMOVED = SDL_AUDIODEVICEREMOVED, /**< An audio device has been removed. */

    /* Sensor events */
    PICO_SENSORUPDATE = SDL_SENSORUPDATE, /**< A sensor was updated */

    /* Render events */
    PICO_RENDER_TARGETS_RESET = SDL_RENDER_TARGETS_RESET, /**< The render targets have been reset and their contents need to be updated */
    PICO_RENDER_DEVICE_RESET = SDL_RENDER_DEVICE_RESET,   /**< The device has been reset and all textures need to be recreated */

    PICO_SDL_USEREVENT = SDL_USEREVENT, /**< For expert users. See SDL_EventType::SDL_USEREVENT */
} PICO_EVENT;

#endif // PICO_EVENTS_H
