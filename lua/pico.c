#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../src/pico.h"

///////////////////////////////////////////////////////////////////////////////

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

static const luaL_Reg ll_all[] = {
    { "init", l_init },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static int l_cv_pos (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // pos (raw/pct?)
    lua_geti(L, 1, 1);                  // pos | anc
    if (lua_isnil(L,-1)) {
        // raw -> pct
        assert(0 && "TODO");
    } else {
        // pct -> raw
        // check if up is given as 2nd arg
        // call ext or not depending
    }
    // cleanup
    return 1;
}

static const luaL_Reg ll_cv[] = {
    { "pos",  l_cv_pos },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico_native (lua_State* L) {
    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_cv);                  // pico | cv
    lua_setfield(L, -2, "cv");              // pico

    return 1;                               // [pico]
}

