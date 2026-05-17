#ifndef PICO_VIDEO_H
#define PICO_VIDEO_H

#include <stdio.h>

#include "pico.h"
#include "layers.h"

typedef struct {
    Pico_Layer base;
    FILE*      fp;
    int        fps;
    struct {
        unsigned char* y;
        unsigned char* u;
        unsigned char* v;
    } plane;
    struct {
        int y;
        int uv;
        int frame;
    } size;
    long       data_offset;
    struct {
        int total;
        int cur;
        int done;
    } frame;
    Uint32     t0;
} Pico_Layer_Video;

void _pico_free_layer_video (Pico_Layer_Video*);

#endif
