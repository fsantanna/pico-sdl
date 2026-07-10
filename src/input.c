#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "_pico.h"

///////////////////////////////////////////////////////////////////////////////
// INPUT
///////////////////////////////////////////////////////////////////////////////

static Pico_Keyboard _keyboard (int key, SDL_Keymod mod) {
    return (Pico_Keyboard) {
        .key   = key,
        .ctrl  = !!(mod & KMOD_CTRL),
        .shift = !!(mod & KMOD_SHIFT),
        .alt   = !!(mod & KMOD_ALT),
    };
}

Pico_Keyboard pico_get_keyboard (void) {
    _pico_guard();
    return _keyboard(0, SDL_GetModState());
}
Pico_Mouse pico_get_mouse (const char* layer, Pico_Rel_Pos* pos) {
    _pico_guard();
    assert((pos->mode=='!' || pos->mode=='%' || pos->mode=='#'));

    SDL_Point phy;
    Uint32 masks = SDL_GetMouseState(&phy.x, &phy.y);

    Pico_Mouse m = {
        .mode   = pos->mode,
        .anchor = pos->anchor,
        .left   = !!(masks & SDL_BUTTON(SDL_BUTTON_LEFT)),
        .right  = !!(masks & SDL_BUTTON(SDL_BUTTON_RIGHT)),
        .middle = !!(masks & SDL_BUTTON(SDL_BUTTON_MIDDLE)),
    };

    pico_cv_pos (
        layer, pos, "window",
        &(Pico_Rel_Pos){'!', {phy.x, phy.y}, PICO_ANCHOR_NW}
    );
    m.x = pos->x;
    m.y = pos->y;

    return m;
}
void pico_set_mouse (const char* layer, Pico_Rel_Pos pos) {
    _pico_guard();
    Pico_Rel_Pos w = {'!', {}, PICO_ANCHOR_NW};
    pico_cv_pos("window", &w, layer, &pos);
    Pico_Abs_Pos wi = _pico_abs_pos(w, NULL);
    SDL_WarpMouseInWindow(G.window.win, wi.x, wi.y);
    SDL_PumpEvents();
}

// Handles auto aids: quit/exit, window resize, ctrl+zoom/scroll/grid.
// Toggled via pico_set_aids.
//

static int _event_handler (Pico_Event* pico, int do_exit) {
    switch (pico->type) {
        case PICO_EVENT_QUIT: {
            if (G.aids && !G.expert.on && do_exit) {
                exit(0);
            }
            break;
        }

        case PICO_EVENT_WINDOW_RESIZE: {
            if (G.window.ing.fs) {
                G.window.ing.fs = 0;
            } else {
                Pico_Rel_Dim phy = { '!', {pico->window.w, pico->window.h} };
                const char* old = pico_set_layer("window");
                pico_set_scene_dim(phy);
                pico_set_layer(old);
            }
            break;
        }

        case PICO_EVENT_KEY_DN: {
            if (!G.aids || !pico->keyboard.ctrl) {
                break;
            }
            switch (pico->keyboard.key) {
                case SDLK_0: {
                    return 1;
                }
                case SDLK_MINUS: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.w += 0.1;
                    pct.h += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_EQUALS: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.w -= 0.1;
                    pct.h -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_LEFT: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.x -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_RIGHT: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.x += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_UP: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.y -= 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_DOWN: {
                    assert(G.layer == &G.world);
                    Pico_Rel_Rect pct = {'%', {0}, PICO_ANCHOR_C};
                    pico_cv_rect(NULL, &pct, NULL, &G.layer->scene.src);
                    pct.y += 0.1;
                    Pico_Rel_Rect r = G.layer->scene.src;
                    pico_cv_rect(NULL, &r, NULL, &pct);
                    pico_set_scene_src(r);
                    return 1;
                }
                case SDLK_g: {
                    assert(G.layer == &G.world);
                    pico_set_effect_grid(!G.world.effect.grid);
                    return 1;
                }
                case SDLK_s: {
                    pico_output_screenshot("window", NULL, NULL);
                    return 1;
                }
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

static void _sdl_to_pico (SDL_Event* sdl, Pico_Event* pico) {
    assert(sdl!=NULL && pico!=NULL && "bug found");

    PICO_EVENT _enum (SDL_Event* sdl) {
        switch (sdl->type) {
            case SDL_QUIT:
                return PICO_EVENT_QUIT;
            case SDL_KEYDOWN:
                return PICO_EVENT_KEY_DN;
            case SDL_KEYUP:
                return PICO_EVENT_KEY_UP;
            case SDL_MOUSEMOTION:
                return PICO_EVENT_MOUSE_MOTION;
            case SDL_MOUSEBUTTONDOWN:
                return PICO_EVENT_MOUSE_BUTTON_DN;
            case SDL_MOUSEBUTTONUP:
                return PICO_EVENT_MOUSE_BUTTON_UP;
            case SDL_WINDOWEVENT:
                if (sdl->window.event == SDL_WINDOWEVENT_RESIZED) {
                    return PICO_EVENT_WINDOW_RESIZE;
                } else {
                    return PICO_EVENT_NONE;
                }
            default:
                return PICO_EVENT_NONE;
        }
    }

    pico->type = _enum(sdl);
    switch (pico->type) {
        case PICO_EVENT_NONE:
            break;              // TODO: complete with all possible events

        case PICO_EVENT_QUIT:
            break;
        case PICO_EVENT_WINDOW_RESIZE:
            pico->window = (typeof(pico->window)) { sdl->window.data1, sdl->window.data2 };
            break;

        case PICO_EVENT_KEY_DN:
        case PICO_EVENT_KEY_UP:
            pico->keyboard = _keyboard(sdl->key.keysym.sym, sdl->key.keysym.mod);
            break;

        case PICO_EVENT_MOUSE_MOTION:
        case PICO_EVENT_MOUSE_BUTTON_DN:
        case PICO_EVENT_MOUSE_BUTTON_UP: {
            // report mouse pos in window pixels regardless of current layer
            Pico_Rel_Pos tmpl = { .mode='!', .anchor=PICO_ANCHOR_C };
            pico->mouse = pico_get_mouse("window", &tmpl);
            break;
        }

        default:
            printf(">>> %d\n", pico->type);
            assert(0 && "bug found");
            break;
    }
}

int pico_input_event_timeout (Pico_Event* evt, int type, int timeout) {
    _pico_guard();
    int t0 = SDL_GetTicks();
    int ti = t0;
    while (1) {
        Pico_Event out;
        SDL_Event sdl;
        int has = (timeout == -1) ? SDL_WaitEvent(&sdl) : SDL_WaitEventTimeout(&sdl, timeout);
        int tn = SDL_GetTicks();
        if (!has && timeout!=-1) {
            if (evt != NULL) {
                evt->type = PICO_EVENT_NONE;
            }
            return tn - t0;
        }

        _sdl_to_pico(&sdl, &out);
        if (_event_handler(&out, 1)) {
            // continue
        } else if (out.type == PICO_EVENT_NONE) {
            // continue
        } else if (type==PICO_EVENT_ANY || out.type==type) {
            if (evt != NULL) {
                *evt = out;
            }
            return tn - t0;
        } else {
            // continue
        }

        if (timeout != -1) {
            int dt = tn - ti;
            timeout = MAX(0, timeout-dt);
            ti = tn;
        }
    }
}

int pico_input_event (Pico_Event* evt, int type) {
    _pico_guard();
    if (G.expert.ms==0 || G.expert.ms==-1) {
        return pico_input_event_timeout(evt, type, G.expert.ms);
    }

    int now = SDL_GetTicks();
    int cur = (G.expert.t0 + G.expert.ms) - now;
    if (cur <= 0) {
        while (G.expert.t0+G.expert.ms <= now) {
            G.expert.t0 += G.expert.ms;
        }
        cur = 0;
    }
    Pico_Event xevt;
    if (evt == NULL) {
        evt = &xevt;
    }
    int dt = pico_input_event_timeout(evt, type, cur);
    if (evt->type == PICO_EVENT_NONE) {
        G.expert.t0 += G.expert.ms;
    }
    return dt;
}

int pico_input_delay (int ms) {
    _pico_guard();
    return pico_input_event_timeout(NULL, PICO_EVENT_NONE, ms);
}

void pico_input_loop (void) {
    _pico_guard();
    pico_input_event(NULL, PICO_EVENT_QUIT);
}

