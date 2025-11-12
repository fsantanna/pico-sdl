#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../pico.h"

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

static int l_pos (lua_State* L) {
    int ok;
    luaL_checktype(L, 1, LUA_TTABLE);       // pct = { x,y }
    Pico_Pct pct;
    {
        lua_getfield(L, 1, "x");            // pct | pct.x
        pct.x = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "expected field 'x'");
        }
        lua_pop(L, 1);                      // pct
    }
    {
        lua_getfield(L, 1, "y");            // pct | pct.y
        pct.y = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "expected field 'y'");
        }
        lua_pop(L, 1);                      // pct
    }
    {
        Pico_Pos pos = pico_pos(pct);
        lua_newtable(L);                    // pct | pos
        lua_pushinteger(L, pos.x);          // pct | pos | x
        lua_setfield(L, -2, "x");           // pct | pos
        lua_pushinteger(L, pos.x);          // pct | pos | y
        lua_setfield(L, -2, "y");           // pct | pos
    }
    return 1;                               // pct | [pos]
}

static int l_input_event (lua_State* L) {
    Pico_Event e;
    pico_input_event(&e, PICO_ANY);
    return 0;
}

static const luaL_Reg all[] = {
    { "init", l_init },
    { "pos",  l_pos  },
    { NULL,   NULL }
};

static const luaL_Reg input[] = {
    { "event", l_input_event  },
    { NULL,    NULL }
};

int luaopen_pico (lua_State* L) {
    luaL_newlib(L, all);            // pico
    luaL_newlib(L, input);          // pico | input
    lua_setfield(L, -2, "input");   // pico
    return 1;                       // [pico]
}
