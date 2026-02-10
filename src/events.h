#ifndef PICO_EVENTS_H
#define PICO_EVENTS_H

#include <SDL2/SDL_events.h>

typedef enum {
    PICO_EVENT_MOUSE_BUTTON_NONE   = 0,
    PICO_EVENT_MOUSE_BUTTON_LEFT   = SDL_BUTTON_LEFT,
    PICO_EVENT_MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    PICO_EVENT_MOUSE_BUTTON_RIGHT  = SDL_BUTTON_RIGHT
} PICO_EVENT_MOUSE_BUTTON;

typedef SDL_Event Pico_Event;

typedef enum PICO_EVENT {
    PICO_EVENT_ANY = 0, /**< Wait for any event */

    /* Application events */
    PICO_EVENT_QUIT = SDL_QUIT,                                   /**< User-requested quit */
    PICO_EVENT_APP_TERMINATING = SDL_APP_TERMINATING,             /**< The application is being terminated by the OS */
    PICO_EVENT_APP_MEMORY_LOW = SDL_APP_LOWMEMORY,                /**< The application is low on memory, free memory if possible. */
    PICO_EVENT_APP_BACKGROUND_WILL = SDL_APP_WILLENTERBACKGROUND, /**< The application is about to enter the background */
    PICO_EVENT_APP_BACKGROUND_DID = SDL_APP_DIDENTERBACKGROUND,   /**< The application did enter the background and may not get CPU for some time */
    PICO_EVENT_APP_FOREGROUND_WILL = SDL_APP_WILLENTERFOREGROUND, /**< The application is about to enter the foreground */
    PICO_EVENT_APP_FOREGROUND_DID = SDL_APP_DIDENTERFOREGROUND,   /**< The application is now interactive */

    PICO_EVENT_LOCALE_CHANGED = SDL_LOCALECHANGED, /**< The user's locale preferences have changed. */

    /* Display events */
    PICO_EVENT_DISPLAY = SDL_DISPLAYEVENT, /**< Display state change */

    /* Window events */
    PICO_EVENT_WINDOW = SDL_WINDOWEVENT, /**< Window state change */
    PICO_EVENT_SYSWM = SDL_SYSWMEVENT,   /**< System specific event */

    /* Keyboard events */
    PICO_EVENT_KEY_DOWN = SDL_KEYDOWN, /**< Key pressed */
    PICO_EVENT_KEY_UP = SDL_KEYUP, /**< Key released */
    PICO_EVENT_TEXT_EDITING = SDL_TEXTEDITING, /**< Keyboard text editing (composition) */
    PICO_EVENT_TEXT_INPUT = SDL_TEXTINPUT, /**< Keyboard text input */
    PICO_EVENT_KEY_MAP_CHANGED = SDL_KEYMAPCHANGED,  /**< Keymap changed due to input language or keyboard layout. */
    //PICO_EVENT_TEXT_EDITING_EXT = SDL_TEXTEDITING_EXT, /**< Extended keyboard text editing (composition) */

    /* Mouse events */
    PICO_EVENT_MOUSE_MOTION = SDL_MOUSEMOTION,           /**< Mouse moved */
    PICO_EVENT_MOUSE_BUTTON_DOWN = SDL_MOUSEBUTTONDOWN,  /**< Mouse button pressed */
    PICO_EVENT_MOUSE_BUTTON_UP = SDL_MOUSEBUTTONUP,      /**< Mouse button released */
    PICO_EVENT_MOUSE_WHEEL = SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

    /* Joystick events */
    PICO_EVENT_JOYSTICK_AXIS_MOTION = SDL_JOYAXISMOTION,       /**< Joystick axis motion */
    PICO_EVENT_JOYSTICK_BALL_MOTION = SDL_JOYBALLMOTION,       /**< Joystick trackball motion */
    PICO_EVENT_JOYSTICK_HAT_MOTION = SDL_JOYHATMOTION,         /**< Joystick hat position change */
    PICO_EVENT_JOYSTICK_BUTTON_DOWN = SDL_JOYBUTTONDOWN,       /**< Joystick button pressed */
    PICO_EVENT_JOYSTICK_BUTTON_UP = SDL_JOYBUTTONUP,           /**< Joystick button released */
    PICO_EVENT_JOYSTICK_DEVICE_ADDED = SDL_JOYDEVICEADDED,     /**< A new joystick has been inserted into the system */
    PICO_EVENT_JOYSTICK_DEVICE_REMOVED = SDL_JOYDEVICEREMOVED, /**< An opened joystick has been removed */
    //PICO_EVENT_JOYSTICK_BATTERY_UPDATED = SDL_JOYBATTERYUPDATED, /**< Joystick battery level change */

    /* Game controller events */
    PICO_EVENT_CONTROLLER_AXIS_MOTION = SDL_CONTROLLERAXISMOTION,       /**< Game controller axis motion */
    PICO_EVENT_CONTROLLER_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN,       /**< Game controller button pressed */
    PICO_EVENT_CONTROLLER_BUTTON_UP = SDL_CONTROLLERBUTTONUP,           /**< Game controller button released */
    PICO_EVENT_CONTROLLER_DEVICE_ADDED = SDL_CONTROLLERDEVICEADDED,     /**< A new Game controller has been inserted into the system */
    PICO_EVENT_CONTROLLER_DEVICE_REMOVED = SDL_CONTROLLERDEVICEREMOVED, /**< An opened Game controller has been removed */
    PICO_EVENT_CONTROLLER_DEVICE_REMAPPED = SDL_CONTROLLERDEVICEREMAPPED,   /**< The controller mapping was updated */
    PICO_EVENT_CONTROLLER_TOUCHPAD_DOWN = SDL_CONTROLLERTOUCHPADDOWN,       /**< Game controller touchpad was touched */
    PICO_EVENT_CONTROLLER_TOUCHPAD_MOTION = SDL_CONTROLLERTOUCHPADMOTION,   /**< Game controller touchpad finger was moved */
    PICO_EVENT_CONTROLLER_TOUCHPAD_UP = SDL_CONTROLLERTOUCHPADUP,           /**< Game controller touchpad finger was lifted */
    PICO_EVENT_CONTROLLER_SENSOR_UPDATE = SDL_CONTROLLERSENSORUPDATE,       /**< Game controller sensor was updated */
    //PICO_EVENT_CONTROLLER_STEAM_HANDLE_UPDATED = SDL_CONTROLLERSTEAMHANDLEUPDATED, /**< Game controller Steam handle has changed */

    /* Touch events */
    PICO_EVENT_FINGER_DOWN = SDL_FINGERDOWN,
    PICO_EVENT_FINGER_UP = SDL_FINGERUP,
    PICO_EVENT_FINGER_MOTION = SDL_FINGERMOTION,

    /* Gesture events */
    PICO_EVENT_GESTURE_DOLLAR = SDL_DOLLARGESTURE,
    PICO_EVENT_GESTURE_DOLLAR_RECORD = SDL_DOLLARRECORD,
    PICO_EVENT_GESTURE_MULTI = SDL_MULTIGESTURE,

    /* Clipboard events */
    PICO_EVENT_CLIPBOARD_UPDATE = SDL_CLIPBOARDUPDATE, /**< The clipboard or primary selection changed */

    /* Drag and drop events */
    PICO_EVENT_DROP_FILE = SDL_DROPFILE,         /**< The system requests a file open */
    PICO_EVENT_DROP_TEXT = SDL_DROPTEXT,         /**< text/plain drag-and-drop event */
    PICO_EVENT_DROP_BEGIN = SDL_DROPBEGIN,       /**< A new set of drops is beginning (NULL filename) */
    PICO_EVENT_DROP_COMPLETE = SDL_DROPCOMPLETE, /**< Current set of drops is now complete (NULL filename) */

    /* Audio hotplug events */
    PICO_EVENT_AUDIO_DEVICE_ADDED = SDL_AUDIODEVICEADDED,     /**< A new audio device is available */
    PICO_EVENT_AUDIO_DEVICE_REMOVED = SDL_AUDIODEVICEREMOVED, /**< An audio device has been removed. */

    /* Sensor events */
    PICO_EVENT_SENSOR_UPDATE = SDL_SENSORUPDATE, /**< A sensor was updated */

    /* Render events */
    PICO_EVENT_RENDER_TARGETS_RESET = SDL_RENDER_TARGETS_RESET, /**< The render targets have been reset and their contents need to be updated */
    PICO_EVENT_RENDER_DEVICE_RESET = SDL_RENDER_DEVICE_RESET,   /**< The device has been reset and all textures need to be recreated */

    PICO_EVENT_SDL_USER = SDL_USEREVENT, /**< For expert users. See SDL_EventType::SDL_USEREVENT */
} PICO_EVENT;

#endif // PICO_EVENTS_H
