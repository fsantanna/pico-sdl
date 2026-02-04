# Plano: Renomear `ref` para `base`

## Status: Concluido

- Branch: `claude/review-pico-sdl-NumZH`
- Commit: `8c45d95`

## Motivacao

O parametro `ref` nas funcoes de conversao de coordenadas foi renomeado para
`base` para melhor transmitir seu proposito como retangulo base para calculos
de coordenadas relativas.

- `ref` era ambiguo (referencia a que?)
- `base` e mais claro: "base rectangle for conversion"
- Campo `up` nas structs foi mantido (hierarquia pai)

## Arquivos Alterados

| Arquivo | Linha(s) | Local | Descricao |
|---------|----------|-------|-----------|
| `src/pico.h` | 420, 423, 427, 430 | `pico_cv_*` | Parametros e docs |
| `src/pico.c` | 163, 167, 170 | `_f1()` | Variavel interna |
| `src/pico.c` | 177, 181, 188 | `_sdl_dim()` | Parametro |
| `src/pico.c` | 216, 220, 227 | `_sdl_pos()` | Parametro |
| `src/pico.c` | 257, 262, 269 | `_sdl_rect()` | Parametro |
| `src/pico.c` | 331, 332, 336, 337 | `pico_cv_*` | API publica |
| `lua/pico.c` | 284-296 | `l_cv_pos()` | Bindings Lua |
| `lua/pico.c` | 308-320 | `l_cv_rect()` | Bindings Lua |
| `lua/doc/api.md` | 29, 31 | docs | Documentacao API |
| `tst/cv.c` | multiplas | testes | Variaveis e comentarios |
| `tst/font.c` | 13, 19-22 | teste | Variavel e comentarios |
| `tst/image_pct.c` | 9, 30-42 | teste | Variavel e comentarios |
| `lua/tst/cv.lua` | 5, 27 | teste | Comentarios |
| `lua/tst/font.lua` | 15-22 | teste | Variavel |
| `lua/tst/image_pct.lua` | 29-36 | teste | Variavel (comentado) |

## API Afetada

```c
// Antes
Pico_Abs_Pos pico_cv_pos_rel_abs(const Pico_Rel_Pos* pos, Pico_Abs_Rect* ref);
Pico_Abs_Rect pico_cv_rect_rel_abs(const Pico_Rel_Rect* rect, Pico_Abs_Rect* ref);

// Depois
Pico_Abs_Pos pico_cv_pos_rel_abs(const Pico_Rel_Pos* pos, Pico_Abs_Rect* base);
Pico_Abs_Rect pico_cv_rect_rel_abs(const Pico_Rel_Rect* rect, Pico_Abs_Rect* base);
```

```lua
-- Antes
pico.cv.pos(pos [,ref])
pico.cv.rect(rect [,ref])

-- Depois
pico.cv.pos(pos [,base])
pico.cv.rect(rect [,base])
```
