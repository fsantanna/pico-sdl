#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <string.h>

#include "../src/pico.h"

typedef enum {
    PICO_RAW = 0,
    PICO_PCT = 1,
} PICO_RAW_PCT;

static const char KEY;

static float L_checkfieldnum (lua_State* L, int i, const char* k) {
    assert(i > 0);
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
    assert(i > 0);

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
    } else if (lua_type(L,i) == LUA_TTABLE) {   // {x, y}
        return (Pico_Pct) {
            L_checkfieldnum(L, i, "x"),
            L_checkfieldnum(L, i, "y"),
        };
    } else {
        luaL_error(L, "invalid anchor");
    }
}

static Pico_Color c_color_t (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { r,g,b }
    Pico_Color clr = {
        L_checkfieldnum(L, i, "r"),
        L_checkfieldnum(L, i, "g"),
        L_checkfieldnum(L, i, "b"),
    };
    return clr;
}

static Pico_Color c_color (lua_State* L) {
    Pico_Color clr;
    if (lua_type(L,1) == LUA_TTABLE) {  // clr = { r,g,b }
        clr = c_color_t(L, 1);
    } else {                            // r | g | b
        clr = (Pico_Color) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
        };
    }
    return clr;
}

static Pico_Dim c_dim (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // dim = { w, h }
    return (Pico_Dim) {
        L_checkfieldnum(L, i, "w"),
        L_checkfieldnum(L, i, "h"),
    };
}

static Pico_Pos c_pos_raw (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // dim = { w, h }
    return (Pico_Pos) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
    };
}

static Pico_Rect c_rect_raw (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // r = { x, y, w, h }
    return (Pico_Rect) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
        L_checkfieldnum(L, i, "w"),
        L_checkfieldnum(L, i, "h"),
    };
}

///////////////////////////////////////////////////////////////////////////////

#define PCT_STACK_MAX 8
static Pico_Rect_Pct _pct_stack[PCT_STACK_MAX];
static int _pct_depth = 0;

static Pico_Rect_Pct* c_rect_pct (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    assert(_pct_depth < PCT_STACK_MAX);

    int cur = _pct_depth++;
    lua_geti(L, i, 1);                      // pct | anc
    Pico_Pct anchor = c_anchor(L, lua_gettop(L));
    lua_pop(L, 1);                          // pct

    lua_getfield(L, i, "up");               // pct | up
    Pico_Rect_Pct* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rect_pct(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // pct

    _pct_stack[cur] = (Pico_Rect_Pct) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
        L_checkfieldnum(L, i, "w"),
        L_checkfieldnum(L, i, "h"),
        anchor,
        up,
    };

    if (cur == 0) {
        _pct_depth = 0;           // reset on root return
    }
    return &_pct_stack[cur];
}

static Pico_Pos_Pct* c_pos_pct (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    lua_geti(L, i, 1);                      // pct | anc
    Pico_Pct anchor = c_anchor(L, lua_gettop(L));
    lua_pop(L, 1);                          // pct

    lua_getfield(L, i, "up");               // pct | up
    Pico_Rect_Pct* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rect_pct(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // pct

    static Pico_Pos_Pct pos;
    pos = (Pico_Pos_Pct) {
        L_checkfieldnum(L, i, "x"),
        L_checkfieldnum(L, i, "y"),
        anchor,
        up,
    };

    return &pos;
}

///////////////////////////////////////////////////////////////////////////////

static int c_is_raw (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    lua_geti(L, i, 1);                      // . | anc
    int raw = lua_isnil(L, -1);
    lua_pop(L, 1);
    return raw;
}

static int c_pos_raw_pct (lua_State* L, int i, Pico_Pos* raw, Pico_Pos_Pct** pct) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // r
    if (c_is_raw(L,1)) {                    // raw
        *raw = c_pos_raw(L, i);
        return 0;
    } else {                                // pct
        *pct = c_pos_pct(L, i);
        return 1;
    }
}

static PICO_RAW_PCT c_rect_raw_pct (lua_State* L, int i, Pico_Rect* raw, Pico_Rect_Pct** pct) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // r
    if (c_is_raw(L,i)) {                    // raw
        *raw = c_rect_raw(L, i);
        return PICO_RAW;
    } else {                                // pct
        *pct = c_rect_pct(L, i);
        return PICO_PCT;
    }
}

///////////////////////////////////////////////////////////////////////////////

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_cv_pos (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // pos | [up]

    Pico_Pos raw;
    Pico_Pos_Pct* pct;
    PICO_RAW_PCT tp = c_pos_raw_pct(L, 1, &raw, &pct);

    // raw -> pct
    if (tp == PICO_RAW) {               // raw | [up]
        assert(0 && "TODO");

    // pct -> raw
    } else {                            // pct | [up]
        Pico_Pos raw;
        if (lua_istable(L,2)) {         // pct | up
            Pico_Rect up = c_rect_raw(L, 2);
            raw = pico_cv_pos_pct_raw_ext(pct, up);
        } else {                        // pct
            raw = pico_cv_pos_pct_raw(pct);
        }

        lua_newtable(L);                // pct | [up] | raw
        lua_pushinteger(L, raw.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, raw.y);
        lua_setfield(L, -2, "y");
    }
    return 1;
}

static int l_cv_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // rect | [up]
    Pico_Rect raw;
    Pico_Rect_Pct* pct;
    PICO_RAW_PCT tp = c_rect_raw_pct(L, 1, &raw, &pct);

    // raw -> pct
    if (tp = PICO_RAW) {                // raw | [up]
        assert(0 && "TODO");

    // pct -> raw
    } else {
        Pico_Rect raw;
        if (lua_istable(L,2)) {         // pct | up
            Pico_Rect up = c_rect_raw(L, 2);
            raw = pico_cv_rect_pct_raw_ext(pct, up);
        } else {                        // pct
            raw = pico_cv_rect_pct_raw(pct);
        }

        lua_newtable(L);                // pct | [up] | raw
        lua_pushinteger(L, raw.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, raw.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, raw.w);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, raw.h);
        lua_setfield(L, -2, "h");
    }
    return 1;
}

static int l_vs_pos_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // pos | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Pos pos;
    Pico_Pos_Pct* pct1;
    PICO_RAW_PCT tp1 = c_pos_raw_pct(L, 1, &pos, &pct1);

    if (tp1 == PICO_PCT) {
        pos = pico_cv_pos_pct_raw(pct1);
    }

    Pico_Rect rect;
    Pico_Rect_Pct* pct2;
    PICO_RAW_PCT tp2 = c_rect_raw_pct(L, 2, &rect, &pct2);

    if (tp2 == PICO_PCT) {
        rect = pico_cv_rect_pct_raw(pct2);
    }

    lua_pushboolean(L, pico_vs_pos_rect_raw(pos, rect));
    return 1;
}

static int l_vs_rect_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // r1 | r2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rect raw1;
    Pico_Rect_Pct* pct1;
    PICO_RAW_PCT tp1 = c_rect_raw_pct(L, 1, &raw1, &pct1);

    if (tp1 == PICO_PCT) {
        raw1 = pico_cv_rect_pct_raw(pct1);
    }

    Pico_Rect raw2;
    Pico_Rect_Pct* pct2;
    PICO_RAW_PCT tp2 = c_rect_raw_pct(L, 2, &raw2, &pct2);

    if (tp2 == PICO_PCT) {
        raw2 = pico_cv_rect_pct_raw(pct2);
    }

    lua_pushboolean(L, pico_vs_rect_rect_raw(raw1, raw2));
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_get_text (lua_State* L) {
    int h = luaL_checknumber(L, 1);
    const char* path = luaL_checkstring(L, 2);
    int w = pico_get_text(h, path);
    lua_pushinteger(L, w);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_alpha (lua_State* L) {
    int a = luaL_checkinteger(L, 1);
    pico_set_alpha(a);
    return 0;
}

static int l_set_color_clear (lua_State* L) {
    Pico_Color clr = c_color(L);
    pico_set_color_clear(clr);
    return 0;
}

static int l_set_color_draw (lua_State* L) {
    Pico_Color clr = c_color(L);
    pico_set_color_draw(clr);
    return 0;
}

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);   // title
    pico_set_title(title);
    return 0;
}

static int l_set_view (lua_State* L) {
    Pico_Dim  xphy,  *xxphy  = NULL;
    Pico_Rect xdst,  *xxdst  = NULL;
    Pico_Dim  xlog,  *xxlog  = NULL;
    Pico_Rect xsrc,  *xxsrc  = NULL;
    Pico_Rect xclip, *xxclip = NULL;

    int fs = luaL_checkinteger(L, 1);   // fs | phy | dst | log | src | clip

    if (lua_istable(L, 2)) {
        xphy = c_dim(L, 2);
        xxphy = &xphy;
    }
    if (lua_istable(L, 3)) {
        xdst = c_rect_raw(L, 3);
        xxdst = &xdst;
    }
    if (lua_istable(L, 4)) {
        xlog = c_dim(L, 4);
        xxlog = &xlog;
    }
    if (lua_istable(L, 5)) {
        xsrc = c_rect_raw(L, 5);
        xxsrc = &xsrc;
    }
    if (lua_istable(L, 6)) {
        xclip = c_rect_raw(L, 6);
        xxclip = &xclip;
    }

    pico_set_view_raw(fs, xxphy, xxdst, xxlog, xxsrc, xxclip);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checknumber(L, 1);       // ms
    pico_input_delay(ms);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_output_clear (lua_State* L) {
    pico_output_clear();
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    if (lua_type(L,1) == LUA_TSTRING) {             // 'C' | x | y
        assert(0 && "TODO");
    } else if (lua_type(L,1) == LUA_TNUMBER) {      // x | y
        Pico_Pos raw = (Pico_Pos) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2)
        };
        pico_output_draw_pixel_raw(raw);
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);           // { x, y }
        Pico_Pos raw;
        Pico_Pos_Pct* pct;
        PICO_RAW_PCT tp = c_pos_raw_pct(L, 1, &raw, &pct);
        if (tp == PICO_RAW) {
            pico_output_draw_pixel_raw(raw);
        } else {
            pico_output_draw_pixel_pct(pct);
        }
    }
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    if (lua_type(L,1) == LUA_TSTRING) {             // 'C' | x | y | w | h
        assert(0 && "TODO");
    } else if (lua_type(L,1) == LUA_TNUMBER) {      // x | y | w | h
        Pico_Rect raw = (Pico_Rect) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4)
        };
        pico_output_draw_rect_raw(raw);
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);           // { x, y, w, h }
        Pico_Rect raw;
        Pico_Rect_Pct* pct;
        PICO_RAW_PCT tp = c_rect_raw_pct(L, 1, &raw, &pct);
        if (tp == PICO_RAW) {
            pico_output_draw_rect_raw(raw);
        } else {
            pico_output_draw_rect_pct(pct);
        }
    }
    return 0;
}

static int l_output_draw_text (lua_State* L) {
    luaL_checktype(L, 1, LUA_TSTRING);  // text | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    const char* text = lua_tostring(L, 1);

    Pico_Rect raw;
    Pico_Rect_Pct* pct;
    PICO_RAW_PCT tp = c_rect_raw_pct(L, 2, &raw, &pct);

    if (tp == PICO_RAW) {
        pico_output_draw_text_raw(text, raw);
    } else {
        pico_output_draw_text_pct(text, pct);
    }

    return 0;
}

static int l_output_screenshot (lua_State* L) {
    char* path = NULL;
    if (lua_type(L,1) == LUA_TSTRING) {         // path
        path = (char*) lua_tostring(L, 1);
    }

    char* ret = NULL;

    int n = lua_gettop(L);
    if (n == 1) {
        ret = (char*) pico_output_screenshot(path);
        goto _RET_;
    } else {
        assert(0 && "TODO");
    }

    _RET_:
    assert(ret != NULL);
    lua_pushstring(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_all[] = {
    { "init", l_init },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_cv[] = {
    { "pos",  l_cv_pos  },
    { "rect", l_cv_rect },
    { NULL, NULL }
};

static const luaL_Reg ll_vs[] = {
    { "pos_rect",  l_vs_pos_rect  },
    { "rect_rect", l_vs_rect_rect },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "text", l_get_text },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "alpha", l_set_alpha },
    { "title", l_set_title },
    { "view",  l_set_view  },
    { NULL, NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { "draw",  l_set_color_draw  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_input[] = {
    { "delay", l_input_delay },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_output[] = {
    { "clear",      l_output_clear      },
    { "screenshot", l_output_screenshot },
    { NULL, NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "pixel",  l_output_draw_pixel },
    { "rect",   l_output_draw_rect  },
    { "text",   l_output_draw_text  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico_native (lua_State* L) {
    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_cv);                  // pico | cv
    lua_setfield(L, -2, "cv");              // pico

    luaL_newlib(L, ll_vs);                  // pico | vs
    lua_setfield(L, -2, "vs");              // pico

    luaL_newlib(L, ll_get);                 // pico | get
    lua_setfield(L, -2, "get");             // pico

    luaL_newlib(L, ll_set);                 // pico | set
    luaL_newlib(L, ll_set_color);           // pico | set | color
    lua_setfield(L, -2, "color");           // pico | set
    lua_setfield(L, -2, "set");             // pico

    luaL_newlib(L, ll_input);               // pico | input
    lua_setfield(L, -2, "input");           // pico

    luaL_newlib(L, ll_output);              // pico | output
    luaL_newlib(L, ll_output_draw);         // pico | output | draw
    lua_setfield(L, -2, "draw");            // pico | output
    lua_setfield(L, -2, "output");          // pico

    lua_pushlightuserdata(L, (void*)&KEY);  // pico | K
    lua_newtable(L);                        // pico | K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // pico

    // anchors
    {                                                     // pico
        lua_pushlightuserdata(L, (void*)&KEY);            // pico | K
        lua_gettable(L, LUA_REGISTRYINDEX);               // pico | G
        lua_newtable(L);                                  // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_C);  // pico | G | ancs | C
        lua_setfield(L, -2, "C");                         // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_NW); // pico | G | ancs | NW
        lua_setfield(L, -2, "NW");                        // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_N);  // pico | G | ancs | N
        lua_setfield(L, -2, "N");                         // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_NE); // pico | G | ancs | NE
        lua_setfield(L, -2, "NE");                        // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_E);  // pico | G | ancs | E
        lua_setfield(L, -2, "E");                         // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_SE); // pico | G | ancs | SE
        lua_setfield(L, -2, "SE");                        // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_S);  // pico | G | ancs | S
        lua_setfield(L, -2, "S");                         // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_SW); // pico | G | ancs | SW
        lua_setfield(L, -2, "SW");                        // pico | G | ancs
        lua_pushlightuserdata(L, (void*)&PICO_ANCHOR_W);  // pico | G | ancs | W
        lua_setfield(L, -2, "W");                         // pico | G | ancs
        lua_setfield(L, -2, "ancs");                      // pico | G
        lua_pop(L, 1);                                    // pico
    }                                                     // pico

    return 1;   // [pico]
}

