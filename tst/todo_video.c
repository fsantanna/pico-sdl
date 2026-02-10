/* video_player.c — simple video player
 *
 * Controls:
 *   +/-    : increase/decrease speed (0.5x steps)
 *   space  : pause/resume
 *   left   : seek backward 1 second
 *   right  : seek forward 1 second
 *   click  : seek to position on seek bar
 *   escape : quit
 *
 * Supports negative speed (reverse playback).
 */

#include "pico.h"
#include <stdio.h>

#define VIDEO_PATH "video.y4m"
#define BAR_H 10

int main (void) {
    pico_init(1);
    pico_set_expert(1);

    Pico_Video info = pico_get_video(VIDEO_PATH, NULL);
    int total = info.fps * 5;

    /* Window: video width scaled up, plus bar */
    int scale = 30;
    int win_w = info.dim.w * scale;
    int win_h = info.dim.h * scale + BAR_H * 2;
    pico_set_window(
        "Video Player", -1,
        &(Pico_Rel_Dim){'!', {win_w, win_h}, NULL}
    );
    pico_set_view(
        0,
        &(Pico_Rel_Dim){'!', {win_w, win_h}, NULL},
        NULL, NULL, NULL, NULL, NULL, NULL
    );

    pico_layer_video("vid", VIDEO_PATH);

    float speed = 1.0;
    int paused = 0;
    float frame_f = 0.0;
    int frame = 0;
    Uint32 last_tick = pico_get_ticks();

    while (1) {
        /* Timing */
        Uint32 now = pico_get_ticks();
        int dt = now - last_tick;
        last_tick = now;

        /* Advance frame by speed */
        if (!paused) {
            frame_f += speed * info.fps * dt / 1000.0;
            /* Clamp */
            if (frame_f < 0) {
                frame_f = 0;
            } else if (frame_f >= total) {
                frame_f = total - 1;
            }
            frame = (int)frame_f;
        }

        /* Sync video */
        pico_video_sync("vid", frame);

        /* Draw video */
        pico_set_color_clear(
            (Pico_Color){0x20, 0x20, 0x20}
        );
        pico_output_clear();
        pico_output_draw_layer(
            "vid",
            &(Pico_Rel_Rect){
                '!',
                {win_w / 2, (win_h - BAR_H * 2) / 2,
                 win_w, win_h - BAR_H * 2},
                PICO_ANCHOR_C, NULL
            }
        );

        /* Draw seek bar background */
        pico_set_color_draw(
            (Pico_Color){0x40, 0x40, 0x40}
        );
        pico_output_draw_rect(
            &(Pico_Rel_Rect){
                '!',
                {0, win_h - BAR_H * 2, win_w, BAR_H},
                PICO_ANCHOR_NW, NULL
            }
        );

        /* Draw seek bar progress */
        float pct = (float)frame / (total - 1);
        int bar_w = (int)(pct * win_w);
        pico_set_color_draw(
            (Pico_Color){0x00, 0xAA, 0xFF}
        );
        pico_output_draw_rect(
            &(Pico_Rel_Rect){
                '!',
                {0, win_h - BAR_H * 2, bar_w, BAR_H},
                PICO_ANCHOR_NW, NULL
            }
        );

        /* Draw info text */
        {
            char label[64];
            snprintf(label, sizeof(label),
                "frame %d/%d  speed %.1fx%s",
                frame, total - 1, speed,
                paused ? "  [PAUSED]" : "");
            pico_set_color_draw(
                (Pico_Color){0xFF, 0xFF, 0xFF}
            );
            pico_output_draw_text(
                label,
                &(Pico_Rel_Rect){
                    '!',
                    {2, win_h - BAR_H, 0, BAR_H},
                    PICO_ANCHOR_NW, NULL
                }
            );
        }

        pico_output_present();

        /* Events */
        Pico_Event evt;
        int timeout = 16;
        while (timeout > 0) {
            Uint32 before = pico_get_ticks();
            int has = pico_input_event_timeout(
                &evt, PICO_EVENT_ANY, timeout
            );
            if (!has) {
                break;
            }

            if (evt.type == PICO_EVENT_QUIT) {
                goto done;
            } else if (evt.type == PICO_EVENT_KEY_DOWN) {
                SDL_Keycode k = evt.key.keysym.sym;
                if (k == SDLK_ESCAPE) {
                    goto done;
                } else if (k == SDLK_PLUS
                        || k == SDLK_EQUALS
                        || k == SDLK_KP_PLUS) {
                    speed += 0.5;
                } else if (k == SDLK_MINUS
                        || k == SDLK_KP_MINUS) {
                    speed -= 0.5;
                } else if (k == SDLK_SPACE) {
                    paused = !paused;
                } else if (k == SDLK_LEFT) {
                    frame_f -= info.fps;
                    if (frame_f < 0) {
                        frame_f = 0;
                    }
                    frame = (int)frame_f;
                } else if (k == SDLK_RIGHT) {
                    frame_f += info.fps;
                    if (frame_f >= total) {
                        frame_f = total - 1;
                    }
                    frame = (int)frame_f;
                }
            } else if (evt.type == PICO_EVENT_MOUSE_BUTTON_DOWN) {
                int mx = evt.button.x;
                int my = evt.button.y;
                /* Click on seek bar area */
                if (my >= win_h - BAR_H * 2) {
                    float click_pct =
                        (float)mx / win_w;
                    if (click_pct < 0) {
                        click_pct = 0;
                    } else if (click_pct > 1) {
                        click_pct = 1;
                    }
                    frame_f = click_pct * (total - 1);
                    frame = (int)frame_f;
                }
            }

            timeout -= (pico_get_ticks() - before);
        }
    }

done:
    pico_init(0);
    return 0;
}
