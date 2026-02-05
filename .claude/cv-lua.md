# Plano: Duck-typed Lua bindings para conversoes inversas

## Status: Concluido

- Branch: `claude/review-pico-sdl-OAOZ7`

## Motivacao

As funcoes C de conversao inversa (`abs_rel`, `rel_rel`) para pos e rect
precisavam de bindings Lua. Em vez de expor 6 funcoes separadas, usamos
duck typing para despachar automaticamente a partir de 2 funcoes:

- `pico.cv.pos(fr, [to], [base])`
- `pico.cv.rect(fr, [to], [base])`

## Logica de Despacho

```
to == nil           → rel_abs(fr, base)      -- retorna tabela abs
to ~= nil, fr rel   → rel_rel(fr, to, base)  -- preenche to, void
to ~= nil, fr abs   → abs_rel(fr, to, base)  -- preenche to, void
```

Deteccao: `fr` e rel se `fr[1]` e string (modo). Caso contrario, e abs.

O parametro `to` e um template: modo, ancora e up ja definidos.
A funcao preenche apenas x, y (e w, h para rect) no template.

## Breaking Change

A posicao do parametro `base` mudou de arg 2 para arg 3:

```lua
-- Antes
pico.cv.pos(fr, base)
pico.cv.rect(fr, base)

-- Depois
pico.cv.pos(fr, nil, base)     -- rel_abs com base
pico.cv.rect(fr, nil, base)    -- rel_abs com base
```

## Arquivos Alterados

| Arquivo | Linha(s) | Local | Descricao |
|---------|----------|-------|-----------|
| `lua/pico.c` | 148-155 | `c_abs_pos()` | Novo helper: le {x,y} sem modo |
| `lua/pico.c` | 237-254 | `L_optfieldnum()` | Novo helper: campo opcional |
| `lua/pico.c` | 256-279 | `c_rel_pos_tpl()` | Template: x,y default 0 |
| `lua/pico.c` | 281-306 | `c_rel_rect_tpl()` | Template: x,y,w,h default 0 |
| `lua/pico.c` | 363-407 | `l_cv_pos()` | Duck typing pos |
| `lua/pico.c` | 409-461 | `l_cv_rect()` | Duck typing rect |
| `lua/tst/cv.lua` | 10,16,22 | rel_abs base | `pos(fr,up)` → `pos(fr,nil,up)` |
| `lua/tst/cv.lua` | 36,42,48 | rel_abs base | `rect(fr,up)` → `rect(fr,nil,up)` |
| `lua/tst/cv.lua` | 102-128 | abs_rel pos | Novos testes |
| `lua/tst/cv.lua` | 131-148 | abs_rel rect | Novos testes |
| `lua/tst/cv.lua` | 150-178 | rel_rel pos | Forma canonica |
| `lua/tst/cv.lua` | 180-211 | rel_rel rect | Forma canonica |
| `lua/tst/cv.lua` | 213-231 | abs_rel base | Com base explicito |
| `lua/tst/cv.lua` | 233-244 | rel_rel base | Com base explicito |

## API Lua

```lua
-- rel_abs: retorna tabela abs {x=N, y=N}
local abs = pico.cv.pos(rel_pos)
local abs = pico.cv.pos(rel_pos, nil, base)

-- abs_rel: preenche template, retorna void
local to = {'%', anc='C'}
pico.cv.pos({x=50, y=50}, to)
-- to.x e to.y agora preenchidos

-- rel_rel: preenche template, retorna void
local to = {'!', anc='NW'}
pico.cv.pos({'%', x=0.5, y=0.5, anc='C'}, to)
-- to.x e to.y agora preenchidos

-- Mesmo padrao para pico.cv.rect com w, h adicionais
```
