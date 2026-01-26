#ifndef PICO_ANCHORS_H
#define PICO_ANCHORS_H

#define PICO_ANCHOR_LEFT   0
#define PICO_ANCHOR_CENTER 0.5
#define PICO_ANCHOR_RIGHT  1
#define PICO_ANCHOR_TOP    0
#define PICO_ANCHOR_MIDDLE 0.5
#define PICO_ANCHOR_BOTTOM 1

typedef SDL_FPoint Pico_Anchor;

extern const Pico_Anchor PICO_ANCHOR_X;
extern const Pico_Anchor PICO_ANCHOR_C;
extern const Pico_Anchor PICO_ANCHOR_NW;
extern const Pico_Anchor PICO_ANCHOR_N;
extern const Pico_Anchor PICO_ANCHOR_NE;
extern const Pico_Anchor PICO_ANCHOR_E;
extern const Pico_Anchor PICO_ANCHOR_SE;
extern const Pico_Anchor PICO_ANCHOR_S;
extern const Pico_Anchor PICO_ANCHOR_SW;
extern const Pico_Anchor PICO_ANCHOR_W;

#endif // PICO_ANCHORS_H

#ifdef PICO_ANCHORS_C

const Pico_Anchor PICO_ANCHOR_X  = { .x=-1, .y=-1 };
const Pico_Anchor PICO_ANCHOR_C  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_MIDDLE };
const Pico_Anchor PICO_ANCHOR_NW = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_N  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_NE = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_TOP    };
const Pico_Anchor PICO_ANCHOR_E  = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_MIDDLE };
const Pico_Anchor PICO_ANCHOR_SE = { .x=PICO_ANCHOR_RIGHT,  .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_S  = { .x=PICO_ANCHOR_CENTER, .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_SW = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_BOTTOM };
const Pico_Anchor PICO_ANCHOR_W  = { .x=PICO_ANCHOR_LEFT,   .y=PICO_ANCHOR_MIDDLE };

#endif // PICO_ANCHORS_C
