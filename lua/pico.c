#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <string.h>

#include "../src/pico.h"

static const char KEY;

static float L_checkfieldnum (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    float v = lua_tonumberx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected numeric field '%s'", k);
    }
    lua_pop(L, 1);                      // T
    return v;
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Pct c_anchor (lua_State* L, int i) {
    Pico_Pct anc;

    if (lua_type(L,i) == LUA_TSTRING) {         // anc
        lua_pushlightuserdata(L, (void*)&KEY);  // anc | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // anc | G
        lua_getfield(L, -1, "ancs");            // anc | G | ancs
        lua_pushvalue(L, -3);                   // anc | G | ancs | anc
        lua_gettable(L, -2);                    // anc | G | ancs | *anc*
        int ok = lua_islightuserdata(L, -1);
        if (!ok) {
            luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,i));
        }
        Pico_Pct* anc = lua_touserdata(L, -1);
        lua_pop(L, 3);                          // anc
        return *anc;
    } else {                                // x | y | G | ancs
        assert(0 && "TODO: non-string anchor");
    }
}

static Pico_Pos_Pct c_pos_pct (lua_State* L, int i) {
    assert(lua_type(L,i) == LUA_TTABLE);    // pct = { 'C', x, y }
    lua_geti(L, i, 1);                      // pct | anc
    Pico_Pct anchor = c_anchor(L, -1);
    lua_getfield(L, i, "up");               // pct | anc | up
    assert(lua_isnil(L,-1) && "TODO: up");
    lua_pop(L, 2);                          // pct
    return (Pico_Pos_Pct) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
        anchor,
        NULL,
    };
}

static Pico_Rect c_rect (lua_State* L, int i) {
    assert(lua_type(L,i) == LUA_TTABLE);    // r = { x, y, w, h }
    return (Pico_Rect) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
        L_checkfieldnum(L, i, "w"),
        L_checkfieldnum(L, i, "h"),
    };
}

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
    luaL_checktype(L, 1, LUA_TTABLE);   // pos | [up]

    lua_geti(L, 1, 1);                  // pos | [up] | anc
    int anc = !lua_isnil(L, -1);
    lua_pop(L, 1);                      // pos | [up]

    // pct -> raw
    if (anc) {                          // pct | [up]
        Pico_Pos_Pct pct = c_pos_pct(L, 1);

        Pico_Pos raw;
        if (lua_istable(L,2)) {         // pct | up
            Pico_Rect up = c_rect(L, 2);
            raw = pico_cv_pos_pct_raw_ext(&pct, up);
        } else {                        // pct
            raw = pico_cv_pos_pct_raw(&pct);
        }

        lua_newtable(L);                // pct | [up] | raw
        lua_pushinteger(L, raw.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, raw.y);
        lua_setfield(L, -2, "y");

    // raw -> pct
    } else {
        assert(0 && "TODO");
    }
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

    lua_pushlightuserdata(L, (void*)&KEY);  // pico | K
    lua_newtable(L);                        // pico | K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // pico

    // anchors
    {                                               // pico
        lua_pushlightuserdata(L, (void*)&KEY);      // pico | K
        lua_gettable(L, LUA_REGISTRYINDEX);         // pico | G
        lua_newtable(L);                            // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_C);   // pico | G | ancs | C
        lua_setfield(L, -2, "C");                   // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_NW);  // pico | G | ancs | NW
        lua_setfield(L, -2, "NW");                  // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_N);   // pico | G | ancs | N
        lua_setfield(L, -2, "N");                   // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_NE);  // pico | G | ancs | NE
        lua_setfield(L, -2, "NE");                  // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_E);   // pico | G | ancs | E
        lua_setfield(L, -2, "E");                   // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_SE);  // pico | G | ancs | SE
        lua_setfield(L, -2, "SE");                  // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_S);   // pico | G | ancs | S
        lua_setfield(L, -2, "S");                   // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_SW);  // pico | G | ancs | SW
        lua_setfield(L, -2, "SW");                  // pico | G | ancs
        lua_pushlightuserdata(L, &PICO_ANCHOR_W);   // pico | G | ancs | W
        lua_setfield(L, -2, "W");                   // pico | G | ancs
        lua_setfield(L, -2, "ancs");                // pico | G
        lua_pop(L, 1);                              // pico
    }                                               // pico

    return 1;                               // [pico]
}

