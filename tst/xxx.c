#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define PICO_TITLE "pico-SDL"
#define PICO_DIM_PHY ((Pico_Dim) {640,360})
#define PICO_DIM_LOG ((Pico_Dim) {640,360})
#define PICO_HASH  128

typedef SDL_Point Pico_Pos;
typedef SDL_Point Pico_Dim;
typedef SDL_Rect  Pico_Rect;
typedef SDL_Color Pico_Color;
typedef SDL_Point Pico_Anchor;
typedef SDL_Point Pico_Flip;
typedef SDL_Point Pico_Pct;

#define PICO_LEFT   0
#define PICO_CENTER 0
#define PICO_RIGHT  0
#define PICO_TOP    0
#define PICO_MIDDLE 0
#define PICO_BOTTOM 0

#define PICO_DIM_KEEP ((Pico_Dim) {0,0})
#define PICO_CLIP_RESET ((Pico_Rect) {0,0,0,0})

#define pico_assert(x) if (!(x)) { fprintf(stderr,"%s\n",SDL_GetError()); assert(0 && "SDL ERROR"); }

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>

#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <assert.h>
#include <string.h>

#ifndef PICO_HASH_H
#define PICO_HASH_H

#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pico_hash;

typedef struct pico_hash pico_hash;

pico_hash* pico_hash_create (size_t num_buckets);
void pico_hash_destroy (pico_hash* table);
int pico_hash_add (pico_hash* table, const char* key, void* value);
int pico_hash_rem (pico_hash* table, const char* key);
void* pico_hash_get (pico_hash* table, const char* key);

#ifdef __cplusplus
}
#endif

#endif // PICO_HASH_H

typedef struct pico_hash_pair {
    char *key;
    void *value;
    struct pico_hash_pair *next;
} pico_hash_pair;

typedef struct pico_hash {
    pico_hash_pair **buckets;
    size_t num_buckets;
} pico_hash;

pico_hash* pico_hash_create (size_t num_buckets) {
    pico_hash *table = (pico_hash*)malloc(sizeof(pico_hash));
    if (table == NULL) {
        return NULL;
    }

    table->num_buckets = num_buckets;
    table->buckets = (pico_hash_pair**)calloc(table->num_buckets, sizeof(pico_hash_pair *));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void pico_hash_destroy (pico_hash *table) {
    pico_hash_pair *pair;
    pico_hash_pair *tmp;
    for (size_t i = 0; i < table->num_buckets; i++) {
        pair = table->buckets[i];
        while (pair != NULL) {
            tmp = pair;
            pair = pair->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(table->buckets);
    free(table);
}

size_t _pico_hash_hash (const char *str, size_t num_buckets) {
    size_t hash = 0;
    for (const char *p = str; *p != '\0'; p++) {
        hash = (hash << 5) + hash + *p;
    }
    return hash % num_buckets;
}

int pico_hash_add (pico_hash *table, const char *key, void *value) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Check if the key already exists in the hash table
    pico_hash_pair *pair = table->buckets[index];
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key already exists in the hash table, update the value
            pair->value = value;
            return 1;
        }
        pair = pair->next;
    }

    // Key does not exist in the hash table, add a new key-value pair
    pair = (pico_hash_pair*)malloc(sizeof(pico_hash_pair));
    if (pair == NULL) {
        return 0;
    }

    pair->key = strdup(key);
    if (pair->key == NULL) {
        free(pair);
        return 0;
    }

    pair->value = value;
    pair->next = table->buckets[index];
    table->buckets[index] = pair;

    return 1;
}

int pico_hash_rem (pico_hash* table, const char *key) {
    size_t index = _pico_hash_hash(key, table->num_buckets);

    // Search for the key-value pair in the linked list at the appropriate index
    pico_hash_pair *pair = table->buckets[index];
    pico_hash_pair *prev = NULL;
    while (pair != NULL) {
        if (strcmp(pair->key, key) == 0) {
            // Key found, remove the key-value pair from the linked list
            if (prev == NULL) {
                // Key-value pair is at the head of the linked list
                table->buckets[index] = pair->next;
            } else {
                // Key-value pair is not at the head of the linked list
                prev->next = pair->next;
            }
            free(pair->key);
            free(pair);
            return 1;
        }
        prev = pair;
        pair = pair->next;
    }

    // Key not found in the hash table
    return 0;
}

void* pico_hash_get (pico_hash* table, const char* key) {
  size_t index = _pico_hash_hash(key, table->num_buckets);

  // Search for the key-value pair in the linked list at the appropriate index
  pico_hash_pair *pair = table->buckets[index];
  while (pair != NULL) {
    if (strcmp(pair->key, key) == 0) {
      // Key found, return the value
      return pair->value;
    }
    pair = pair->next;
  }

  // Key not found in the hash table, return NULL
  return NULL;
}

#define SDL_ANY PICO_ANY
#define MAX(x,y) ((x) > (y) ? (x) : (y))

static SDL_Window*  WIN;
static int FS = 0;          // fullscreen pending (ignore RESIZED event)

#define REN (SDL_GetRenderer(WIN))

static pico_hash* _pico_hash;

typedef struct Pico_Ctx {
    int          alpha;
    struct {
        Pico_Dim phy;
        Pico_Dim log;
    }            dim;
    int          grid;
    char*        name;
    Pico_Pos     pos;
    Pico_Pos     scroll;
    SDL_Texture* tex;
    Pico_Pct     zoom;
} Pico_Ctx;

Pico_Ctx _ctx = {
    0xFF,
    { PICO_DIM_PHY, PICO_DIM_LOG },
    1,
    NULL,
    {0, 0},
    {0, 0},
    NULL,
    {100, 100},
};

static struct {
    struct {
        Pico_Anchor pos;
        Pico_Anchor rotate;
    } anchor;
    int angle;
    Pico_Rect clip;
    struct {
        Pico_Color clear;
        Pico_Color draw;
    } color;
    Pico_Rect crop;
    Pico_Ctx* ctx;
    struct {
        int x;
        Pico_Pos cur;
    } cursor;
    int expert;
    Pico_Flip flip;
    struct {
        void* ttf;
        int h;
    } font;
    int fullscreen;
    int style;
    Pico_Pct scale;
} S = {
    { {PICO_CENTER, PICO_MIDDLE}, {PICO_CENTER, PICO_MIDDLE} },
    0,
    {0, 0, 0, 0},
    { {0x00,0x00,0x00,0xFF}, {0xFF,0xFF,0xFF,0xFF} },
    {0, 0, 0, 0},
    &_ctx,
    {0, {0,0}},
    0,
    {0, 0},
    {NULL, 0},
    0,
    0,
    {100, 100},
};

static int _noclip () {
    return (S.clip.w == PICO_CLIP_RESET.w) ||
           (S.clip.h == PICO_CLIP_RESET.h);
}

static Pico_Dim _zoom (Pico_Ctx* ctx) {
    return (Pico_Dim) {
        ctx->dim.log.x * ctx->zoom.x / 100,
        ctx->dim.log.y * ctx->zoom.y / 100,
    };
}

static void _pico_output_present (int force, Pico_Ctx* ctx);

void pico_output_clear (void) {
    SDL_SetRenderDrawColor (REN,
        S.color.clear.r,
        S.color.clear.g,
        S.color.clear.b,
        S.color.clear.a
    );
    if (_noclip()) {
        SDL_RenderClear(REN);
    } else {
        SDL_Rect r;
        SDL_RenderGetClipRect(REN, &r);
        SDL_RenderFillRect(REN, &r);
    }
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    _pico_output_present(0, S.ctx);
}

static SDL_Texture* _draw_aux (int w, int h) {
    SDL_Texture* aux = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        w, h
    );
    SDL_SetTextureBlendMode(aux, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(REN, aux);
    SDL_SetRenderDrawColor(REN, 0, 0, 0, 0);   // transparent
    SDL_RenderClear(REN);
    SDL_RenderFillRect(REN, NULL);
    SDL_SetRenderDrawColor (REN,
        S.color.draw.r,
        S.color.draw.g,
        S.color.draw.b,
        S.color.draw.a
    );
    return aux;
}

static void _pico_output_present (int force, Pico_Ctx* ctx) {
    if (S.expert && !force) return;

    SDL_Rect clip;
    SDL_RenderGetClipRect(REN, &clip);

    SDL_Texture* up = (ctx == &_ctx) ? NULL : _ctx.tex;

    SDL_SetRenderTarget(REN, up);
    SDL_RenderSetLogicalSize(REN, _ctx.dim.phy.x, _ctx.dim.phy.y);

    if (ctx->name == NULL) {
        SDL_SetRenderDrawColor(REN, 0x77, 0x77, 0x77, 0x77);
        SDL_RenderClear(REN);
        Pico_Color c = S.color.draw;
        SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);
    }

    SDL_Rect dst = { ctx->pos.x, ctx->pos.y, ctx->dim.phy.x, ctx->dim.phy.y };
    SDL_SetTextureAlphaMod(ctx->tex, ctx->alpha);
printf(">>> [%d] %d\n", ctx==&_ctx, ctx->alpha);
    SDL_RenderCopy(REN, ctx->tex, NULL, &dst);
    //SDL_SetTextureAlphaMod(ctx->tex, 0xFF);

    if (ctx->name == NULL) {
        SDL_RenderPresent(REN);
    }

    if (ctx->name != NULL) {
        _pico_output_present(force, &_ctx);
    }

    Pico_Dim Z = _zoom(ctx);
    SDL_RenderSetLogicalSize(REN, Z.x, Z.y);
    SDL_SetRenderTarget(REN, ctx->tex);
    SDL_RenderSetClipRect(REN, &clip);
}

void pico_output_present (void) {
    _pico_output_present(1, &_ctx);
}

void pico_set_alpha (int alpha) {
    S.ctx->alpha = alpha;
    _pico_output_present(0, S.ctx);
}

void pico_set_context (char* name) {
    if (name == NULL) {
        S.ctx = &_ctx;
    } else {
        Pico_Ctx* ctx = (Pico_Ctx*)pico_hash_get(_pico_hash, name);
        if (ctx == NULL) {
            ctx = malloc(sizeof(Pico_Ctx));
            *ctx = (Pico_Ctx) {
                0xFF,
                { {0,0}, {0,0} },
                1,
                name,
                {0, 0},
                {0, 0},
                NULL,
                {100, 100},
            };
            pico_hash_add(_pico_hash, name, ctx);
        }
        pico_assert(ctx != NULL);
        S.ctx = ctx;
    }
}

void pico_set_zoom (Pico_Pct pct) {
    Pico_Dim old = _zoom(S.ctx);
    S.ctx->zoom = pct;
    Pico_Dim new = _zoom(S.ctx);
    
    int dx = new.x - old.x;
    int dy = new.y - old.y;

    SDL_DestroyTexture(S.ctx->tex);
    S.ctx->tex = SDL_CreateTexture (
        REN, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
        new.x, new.y
    );
    pico_assert(S.ctx->tex != NULL);
    SDL_SetTextureBlendMode(S.ctx->tex, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(REN, new.x, new.y);
    SDL_SetRenderTarget(REN, S.ctx->tex);

    // TODO: need to init w/ explicit SetClip to save w/h
    //       do not pass NULL, GetClip would also return w=0,h=0
    SDL_Rect clip = { 0, 0, new.x, new.y };
    SDL_RenderSetClipRect(REN, &clip);
}

int main (void) {
    {
        _pico_hash = pico_hash_create(PICO_HASH);
        pico_assert(0 == SDL_Init(SDL_INIT_VIDEO));
        WIN = SDL_CreateWindow (
            PICO_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            S.ctx->dim.phy.x, S.ctx->dim.phy.y,
            (SDL_WINDOW_SHOWN)
        );
        pico_assert(WIN != NULL);
        SDL_CreateRenderer(WIN, -1, SDL_RENDERER_ACCELERATED);
        pico_assert(REN != NULL);
        SDL_SetRenderDrawBlendMode(REN, SDL_BLENDMODE_BLEND);
        pico_set_zoom(S.ctx->zoom);
        pico_output_clear();
    }

    puts("rect pos=30, dim=50");
    pico_output_clear();
    Pico_Rect r1 = { 192,108,320,180 };

    pico_set_context("rect");

    S.ctx->dim.phy = (Pico_Dim){r1.w,r1.h};
    S.ctx->dim.log = (Pico_Dim){r1.w,r1.h};
    pico_set_zoom(S.ctx->zoom);

    S.ctx->pos = (Pico_Pos) { 32, 18 };
    S.color.clear = (Pico_Color){0xFF, 0xFF, 0xFF, 0xFF};
    pico_output_clear();
    SDL_Delay(500);

    puts("red centered under white");
    Pico_Rect r2 = { 80,45,160,90 };

    SDL_Color c = {0xFF,0x00,0x00,0xFF};
    S.color.draw = c;
    SDL_SetRenderDrawColor(REN, c.r, c.g, c.b, c.a);

    SDL_RenderFillRect(REN, &r2);
    _pico_output_present(0, S.ctx);

    SDL_Delay(500);

    pico_set_context(NULL);
    pico_output_clear();
    pico_set_context("rect");
    pico_set_alpha(0x88);
    SDL_Delay(500);
    pico_set_alpha(0x88);
    SDL_Delay(500);
    pico_set_alpha(0x88);
    SDL_Delay(500);

    return 0;
}
