// gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../hash.c ../dir.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../pico.h"

static const char KEY;

///////////////////////////////////////////////////////////////////////////////

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
            return luaL_error(L, "expected field 'x'");
        }
        lua_pop(L, 1);                      // pct
    }
    {
        lua_getfield(L, 1, "y");            // pct | pct.y
        pct.y = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "expected field 'y'");
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

///////////////////////////////////////////////////////////////////////////////

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);   // title
    pico_set_title(title);
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checkinteger(L, 1);       // ms
    pico_input_delay(ms);
}

static int l_input_event (lua_State* L) {
    lua_pushlightuserdata(L, (void*)&KEY);  // evt | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // evt | G
    lua_getfield(L, -1, "evts");            // evt | G | evts
    lua_pushvalue(L, 1);                    // evt | G | evts | id
    lua_gettable(L, -2);                    // evt | G | evts | num
    int ok;
    int evt = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "invalid event");
    }
    Pico_Event e;
    pico_input_event(&e, evt);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_all[] = {
    { "init", l_init },
    { "pos",  l_pos  },
    { NULL,   NULL }
};

static const luaL_Reg ll_set[] = {
    { "title", l_set_title },
    { NULL,    NULL }
};

static const luaL_Reg ll_input[] = {
    { "delay", l_input_delay },
    { "event", l_input_event },
    { NULL,    NULL }
};

int luaopen_pico (lua_State* L) {
    lua_pushlightuserdata(L, (void*)&KEY);  // K
    lua_newtable(L);                        // K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // -

    luaL_newlib(L, ll_all);                 // pico
    luaL_newlib(L, ll_set);                 // pico | set
    lua_setfield(L, -2, "set");             // pico
    luaL_newlib(L, ll_input);               // pico | input
    lua_setfield(L, -2, "input");           // pico

    {                                       // pico
        lua_pushlightuserdata(L, (void*)&KEY); // ... | K
        lua_gettable(L, LUA_REGISTRYINDEX); // ... | G
        lua_newtable(L);                    // ... | G | evts
        lua_pushinteger(L, PICO_KEYUP);     // ... | G | evts | UP
        lua_setfield(L, -2, "pico.key.up"); // ... | G | evts
        lua_setfield(L, -2, "evts");        // ... | G
        lua_pop(L, 1);                      // ...
    }                                       // pico

    return 1;                               // [pico]
}
