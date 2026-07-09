# Layer coordinate space: `image` vs `empty`

## Context

File:  `screens/telas_menu/personalizar.atm`
Func:  `cria_layer_quadro`
Goal:  a tiled `:quadro` board holding a grid of color swatches
       as child layers (`up = :quadro`).

## Problem / Question

A child positioned with a `%` target lands in different places
depending on the parent layer type.

- With `pico.layer.image` as `:quadro`,
  the color swatches spread relative to the **world**.
- With `pico.layer.empty` as `:quadro`,
  the swatches land correctly relative to **quadro**.

Question: why?
Is this a bug in pico-sdl?

## Investigation

Key rule found in pico-sdl:

> A child's `%` target resolves against the parent layer's
> `scene.dim` (the parent texture size), then is baked onto that
> texture.

`scene.dim` has a different source per layer type:

| layer type      | source of `scene.dim`          | you control it? |
| --------------- | ------------------------------ | --------------- |
| `image`         | PNG native pixels              | no              |
| `empty`         | your `dim` / `target`          | yes             |
| `pixmap`/`text` | pixel / text content           | no              |

Source references:

- `mem.c:_pico_mem_alloc_layer_image` : `SDL_QueryTexture`
- `mem.c:_pico_mem_alloc_layer_empty` : `_pico_abs_dim(target)`
- `layer.c:_pico_draw_all_pos`         : sets `G.layer = parent`
- `layer.c:_pico_layer_output`         : resolves `%` vs
                                         `G.layer->scene.dim`

Concrete numbers in this project:

```
world / window          = 1280 x 720   (main.atm:7)
quadro.png native        = 1360 x 768   (≈ whole world)
empty target 0.4 x 0.5   =  512 x 360   (the box you want)
```

- empty -> `scene.dim = 512x360`, the box you asked for.
- image -> `scene.dim = 1360x768`, ≈ world, so the grid spans a
  near-world-sized coordinate space.

## Answer / Verdict

Not a bug: surprising-but-consistent design.

The rule `% is always a fraction of the parent texture` is
uniform across `image`, `pixmap`, and `text`.
An `image` layer's texture is fixed by its file, so it cannot act
as a container with a coordinate box independent of its artwork.

Legitimate design wart (worth reporting upstream):
`layer.image` has no `dim` / `target`-as-dim override to decouple
coordinate space from file resolution, unlike `layer.empty`.

On-screen the two are positionally proportional (both scale into
the same `0.4 x 0.5` target).
The visible difference is aspect distortion plus the artwork,
not misplacement:

```
empty  dim 512x360   aspect 1.42  == target 1.42  -> clean
image  dim 1360x768  aspect 1.77  != target 1.42  -> squished
```

## Idiomatic pattern

Separate concerns: `empty` owns the box, image rides on top.

```atm
pico.layer.empty [ up=:world, key=:quadro,
    dim    = ['!', w=7, h=5],
    tile   = [ w=600/7, h=400/5 ],
    target = ['%', x=0.78, y=0.5, w=0.4, h=0.5 ] ]

pico.layer.image [ up=:quadro, key=:quadro_bg,
    path="../../../assets/imgs/personalizar/quadro.png",
    target=['%', x=0.5, y=0.5, w=1, h=1 ] ]
```

## Pending

- [ ] Optional: confirm on-screen behavior by running the app.
- [ ] Optional: file upstream request for `layer.image` dim
      override.
