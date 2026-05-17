#ifndef PICO_AUX_H
#define PICO_AUX_H

#include "pico.h"

///////////////////////////////////////////////////////////////////////////////
// _raw_*: rel -> float (logical coords)
///////////////////////////////////////////////////////////////////////////////

SDL_FDim   _pico_raw_dim  (Pico_Rel_Dim* dim, const Pico_Abs_Rect* base,
                      const Pico_Abs_Dim* ratio);
SDL_FPoint _pico_raw_pos  (Pico_Rel_Pos pos, const Pico_Abs_Rect* base);
SDL_FRect  _pico_raw_rect (Pico_Rel_Rect rect, const Pico_Abs_Rect* base,
                      const Pico_Abs_Dim* ratio);

///////////////////////////////////////////////////////////////////////////////
// _rel_*: float (logical coords) -> rel
///////////////////////////////////////////////////////////////////////////////

void _pico_rel_dim  (SDL_FDim   flt, Pico_Rel_Dim*  to, const Pico_Abs_Rect* base);
void _pico_rel_pos  (SDL_FPoint flt, Pico_Rel_Pos*  to, const Pico_Abs_Rect* base);
void _pico_rel_rect (SDL_FRect  flt, Pico_Rel_Rect* to, const Pico_Abs_Rect* base);

///////////////////////////////////////////////////////////////////////////////
// _rnd_*: float -> abs (rounding)
///////////////////////////////////////////////////////////////////////////////

Pico_Abs_Dim  _pico_rnd_dim  (SDL_FDim   f);
Pico_Abs_Pos  _pico_rnd_pos  (SDL_FPoint f);
Pico_Abs_Rect _pico_rnd_rect (SDL_FRect  f);

#endif
