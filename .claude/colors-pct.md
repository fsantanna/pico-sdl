# Issue #68: Color in %, when first arg

## Status: Implementado

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

## Pendente

- [ ] Commit e push
- [ ] Testar manualmente
