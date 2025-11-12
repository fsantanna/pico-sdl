#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "pico.h"

static int init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

static const luaL_Reg all[] = {
    {"init", init},
    {NULL, NULL}
};

int luaopen_pico (lua_State* L) {
    luaL_newlib(L, all);
    return 1;
}
