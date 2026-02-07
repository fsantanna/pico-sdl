# Issue #68: Color in %, when first arg

## Status: Done

- Issue: https://github.com/fsantanna/pico-sdl/issues/68
- PR: https://github.com/fsantanna/pico-sdl/pull/78
- Branch: `fix-issue-68-color-pct`

## Objetivo

Suportar cores em porcentagem usando tabelas com `'%'` na posição 1.

## Formato

```lua
-- Absoluto (existente)
pico.set.color.draw({r=128, g=128, b=128})

-- Absoluto explícito (novo)
pico.set.color.draw({'!', r=128, g=128, b=128})

-- Porcentagem (novo)
pico.set.color.draw({'%', r=0.5, g=0.5, b=0.5})

-- Com alpha em porcentagem
pico.set.color.draw({'%', r=1, g=0, b=0, a=0.5})
```

## Alterações

| Arquivo | Linha | Local | Descricao |
|---------|-------|-------|-----------|
| `lua/pico.c` | 60-83 | `c_color_t()` | Suporte a `'%'` e `'!'` opcional |
| `lua/pico.c` | 85-115 | `c_color_a_t()` | Suporte a `'%'` e `'!'` com alpha |
| `lua/tst/colors.lua` | 109-136 | test "colors-06" | Testes Lua para formato porcentagem |
| `tst/colors.c` | 131-173 | test "colors-06" | Testes C (referencia para imagem) |

## Notas

- O teste C produz a imagem de referencia usando `Pico_Color` direto
- O teste Lua verifica que o parsing de `'%'` e `'!'` produz o mesmo resultado
- `0.5 * 255 = 127.5` trunca para `127` (Uint8)

## Pendente

- [x] Commit e push
- [x] Criar PR
- [x] Adicionar testes Lua
- [x] Adicionar testes C
- [x] Resolver conflito com colors-05 (mix) -> renomeado para colors-06
- [x] Gerar imagem de referencia `tst/asr/colors-06.png`
- [x] Testar manualmente
- [x] Merge
