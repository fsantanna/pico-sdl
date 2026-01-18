Assumes this directory structure:

```
.
└── lua/
    ├── pico/
    └── tst/    <-- we are here
```

```bash
LUA_CPATH="../pico/?.so;" LUA_PATH="../?/init.lua;../?.lua;" lua5.4 init.lua
```
