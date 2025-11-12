// gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../hash.c ../dir.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../pico.h"

static const char KEY;

static int L_checkfieldint (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    int v = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected field '%s'", k);
    }
    lua_pop(L, 1);                      // T
    return v;
}

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
    Pico_Pct pct = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    {
        Pico_Pos pos = pico_pos(pct);
        lua_newtable(L);                    // pct | pos
        lua_pushinteger(L, pos.x);          // pct | pos | x
        lua_setfield(L, -2, "x");           // pct | pos
        lua_pushinteger(L, pos.y);          // pct | pos | y
        lua_setfield(L, -2, "y");           // pct | pos
    }
    return 1;                               // pct | [pos]
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_anchor_draw (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T
    lua_getfield(L, 1, "x");                // T | x
    lua_getfield(L, 1, "y");                // T | x | y

    Pico_Anchor anc;

    lua_pushlightuserdata(L, (void*)&KEY);  // T | x | y | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // T | x | y | G
    lua_getfield(L, -1, "ancs");            // T | x | y | G | ancs

    int ok;
    {
        lua_pushvalue(L, 2);                    // . | ancs | x
        lua_gettable(L, -2);                    // . | ancs | x
        anc.x = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,2));
        }
        lua_pop(L, 1);                          // . | ancs
    }
    {
        lua_pushvalue(L, 3);                    // . | ancs | y
        lua_gettable(L, -2);                    // . | ancs | y
        anc.y = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,3));
        }
        lua_pop(L, 1);                          // . | ancs
    }

    pico_set_anchor_draw(anc);
    return 0;
}

static int l_set_color_clear (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // clr = { r,g,b,a }
    Pico_Color clr = {
        L_checkfieldint(L, 1, "r"),
        L_checkfieldint(L, 1, "g"),
        L_checkfieldint(L, 1, "b"),
        L_checkfieldint(L, 1, "a")
    };
    pico_set_color_clear(clr);
    return 0;
}

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);   // title
    pico_set_title(title);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checkinteger(L, 1);       // ms
    pico_input_delay(ms);
    return 0;
}

static int l_input_event (lua_State* L) {
    luaL_checktype(L, 1, LUA_TSTRING);      // evt
    lua_pushlightuserdata(L, (void*)&KEY);  // evt | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // evt | G
    lua_getfield(L, -1, "evts");            // evt | G | evts
    lua_pushvalue(L, 1);                    // evt | G | evts | id
    lua_gettable(L, -2);                    // evt | G | evts | num
    int ok;
    int evt = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "invalid event \"%s\"", lua_tostring(L,1));
    }
    Pico_Event e;
    pico_input_event(&e, evt);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_output_clear (lua_State* L) {
    pico_output_clear();
    return 0;
}

static int l_output_draw_image (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y}
    luaL_checktype(L, 2, LUA_TSTRING);      // pos={x,y} | path
    Pico_Pos pos = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    pico_output_draw_image(pos, lua_tostring(L,2));
    return 0;
}

static int l_output_sound (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);   // path
    pico_output_sound(path);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_all[] = {
    { "init", l_init },
    { "pos",  l_pos  },
    { NULL,   NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "title", l_set_title },
    { NULL,    NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { NULL,    NULL }
};

static const luaL_Reg ll_set_anchor[] = {
    { "draw", l_set_anchor_draw },
    { NULL,   NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_input[] = {
    { "delay", l_input_delay },
    { "event", l_input_event },
    { NULL,    NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_output[] = {
    { "clear", l_output_clear },
    { "sound", l_output_sound },
    { NULL,    NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "image", l_output_draw_image },
    { NULL,    NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico (lua_State* L) {
    lua_pushlightuserdata(L, (void*)&KEY);  // K
    lua_newtable(L);                        // K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // -

    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_set);                 // pico | set
    luaL_newlib(L, ll_set_color);           // pico | set | color
    lua_setfield(L, -2, "color");           // pico | set
    luaL_newlib(L, ll_set_anchor);          // pico | set | anchor
    lua_setfield(L, -2, "anchor");          // pico | set
    lua_setfield(L, -2, "set");             // pico

    luaL_newlib(L, ll_input);               // pico | input
    lua_setfield(L, -2, "input");           // pico

    luaL_newlib(L, ll_output);              // pico | output
    luaL_newlib(L, ll_output_draw);         // pico | output | draw
    lua_setfield(L, -2, "draw");            // pico | output
    lua_setfield(L, -2, "output");          // pico

    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | evts
        lua_pushinteger(L, PICO_KEYUP);         // . | G | evts | UP
        lua_setfield(L, -2, "key.up");          // . | G | evts
        lua_setfield(L, -2, "evts");            // . | G
        lua_pop(L, 1);                          // .
    }                                           // pico

    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | ancs
        lua_pushinteger(L, PICO_CENTER);        // . | G | ancs | C
        lua_setfield(L, -2, "center");          // . | G | ancs
        lua_pushinteger(L, PICO_MIDDLE);        // . | G | ancs | M
        lua_setfield(L, -2, "middle");          // . | G | ancs
        lua_setfield(L, -2, "ancs");            // . | G
        lua_pop(L, 1);                          // .
    }                                           // pico

    return 1;                               // [pico]
}
