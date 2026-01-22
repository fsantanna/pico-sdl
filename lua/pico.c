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

static Pico_Pct_XY c_anchor (lua_State* L, int i) {
    assert(i > 0);

    if (lua_type(L,i) == LUA_TSTRING) {         // anc
        lua_pushlightuserdata(L, (void*)&KEY);  // anc | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // anc | G
        lua_getfield(L, -1, "anchors");         // anc | G | ancs
        lua_pushvalue(L, i);                    // anc | G | ancs | anc
        lua_gettable(L, -2);                    // anc | G | ancs | *anc*
        int ok = lua_islightuserdata(L, -1);
        if (!ok) {
            luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,i));
        }
        Pico_Pct_XY* anc = lua_touserdata(L, -1);
        lua_pop(L, 3);                          // anc
        return *anc;
    } else if (lua_type(L,i) == LUA_TTABLE) {   // {x, y}
        return (Pico_Pct_XY) {
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

static Pico_Color_A c_color_a_t (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { r,g,b,a }
    Pico_Color_A clr = {
        L_checkfieldnum(L, i, "r"),
        L_checkfieldnum(L, i, "g"),
        L_checkfieldnum(L, i, "b"),
        L_checkfieldnum(L, i, "a"),
    };
    return clr;
}

static Pico_Color c_color (lua_State* L) {
    Pico_Color clr;
    if (lua_type(L,1) == LUA_TSTRING) {         // clr = 'red'
        lua_pushlightuserdata(L, (void*)&KEY);  // clr | . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // clr | . | G
        lua_getfield(L, -1, "colors");          // clr | . | G | clrs
        lua_pushvalue(L, 1);                   // clr | . | G | clrs | clr
        lua_gettable(L, -2);                    // clr | . | G | clrs | *clr*
        int ok = lua_islightuserdata(L, -1);
        if (!ok) {
            luaL_error(L, "invalid color \"%s\"", lua_tostring(L,1));
        }
        Pico_Color* clr = lua_touserdata(L, -1);
        lua_pop(L, 3);                          // clr
        return *clr;
    } else if (lua_type(L,1) == LUA_TTABLE) {  // clr = { r,g,b }
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
    Pico_Pct_XY anchor = c_anchor(L, lua_gettop(L));
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
    Pico_Pct_XY anchor = c_anchor(L, lua_gettop(L));
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

static int c_dim_raw_pct (lua_State* L, int i, Pico_Dim* raw, Pico_Pct_WH* pct) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // dim
    if (c_is_raw(L,i)) {
        *raw = c_dim(L, i);
        return 0;
    } else {
        *pct = (Pico_Pct_WH) {
            L_checkfieldnum(L, i, "w"),
            L_checkfieldnum(L, i, "h"),
        };
        return 1;
    }
}

static int c_pos_raw_pct (lua_State* L, int i, Pico_Pos* raw, Pico_Pos_Pct** pct) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // r
    if (c_is_raw(L,i)) {                    // raw
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

static Pico_Pos c_pos_raw_pct_raw (lua_State* L, int i) {
    assert(i > 0);
    Pico_Pos raw;
    Pico_Pos_Pct* pct;
    PICO_RAW_PCT tp = c_pos_raw_pct(L, i, &raw, &pct);
    if (tp == PICO_PCT) {
        raw = pico_cv_pos_pct_raw(pct);
    }
    return raw;
}

static Pico_Rect c_rect_raw_pct_raw (lua_State* L, int i) {
    assert(i > 0);
    Pico_Rect raw;
    Pico_Rect_Pct* pct;
    PICO_RAW_PCT tp = c_rect_raw_pct(L, i, &raw, &pct);
    if (tp == PICO_PCT) {
        raw = pico_cv_rect_pct_raw(pct);
    }
    return raw;
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

    Pico_Pos  pos  = c_pos_raw_pct_raw(L, 1);
    Pico_Rect rect = c_rect_raw_pct_raw(L, 2);

    int ret = pico_vs_pos_rect_raw(pos, rect);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_vs_rect_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // r1 | r2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rect r1 = c_rect_raw_pct_raw(L, 1);
    Pico_Rect r2 = c_rect_raw_pct_raw(L, 2);

    int ret = pico_vs_rect_rect_raw(r1, r2);
    lua_pushboolean(L, ret);
    return 1;
}

static void L_push_color (lua_State* L, Pico_Color clr) {
    lua_newtable(L);
    lua_pushinteger(L, clr.r);
    lua_setfield(L, -2, "r");
    lua_pushinteger(L, clr.g);
    lua_setfield(L, -2, "g");
    lua_pushinteger(L, clr.b);
    lua_setfield(L, -2, "b");
}

static int l_color_darker (lua_State* L) {
    Pico_Color clr = c_color(L);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_darker(clr, pct);
    L_push_color(L, ret);
    return 1;
}

static int l_color_lighter (lua_State* L) {
    Pico_Color clr = c_color(L);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_lighter(clr, pct);
    L_push_color(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static PICO_RAW_PCT L_get_dim_raw_pct (lua_State* L, int i, Pico_Dim* raw, Pico_Pct_WH* pct, Pico_Rect_Pct** ref) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);       // dim | [ref]

    if (c_is_raw(L,i)) {                    // {w,h}
        { // default w/h=0
            lua_getfield(L, i, "w");        // {w,h} | w
            if (lua_isnil(L,-1)) {
                lua_pushinteger(L, 0);      // {w,h} | w | 0
                lua_setfield(L, i, "w");    // {w,h} | w
            }
            lua_pop(L, 1);                  // {w,h}
            lua_getfield(L, i, "h");        // {w,h} | h
            if (lua_isnil(L,-1)) {
                lua_pushinteger(L, 0);      // {w,h} | h | 0
                lua_setfield(L, i, "h");    // {w,h} | h
            }
            lua_pop(L, 1);                  // {w,h}
        }
        *raw = c_dim(L, i);
        return PICO_RAW;
    } else {                                // {%,w,h} | [ref]
        { // default x/y=0
            lua_getfield(L, i, "w");        // {%,w,h} | [ref] | w
            if (lua_isnil(L,-1)) {
                lua_pushinteger(L, 0);      // {%,w,h} | [ref] | w | 0
                lua_setfield(L, i, "w");    // {%,w,h} | [ref] | w
            }
            lua_pop(L, 1);                  // {%,w,h} | [ref]
            lua_getfield(L, i, "h");        // {%,w,h} | [ref] | h
            if (lua_isnil(L,-1)) {
                lua_pushinteger(L, 0);      // {%,w,h} | [ref] | h | 0
                lua_setfield(L, i, "h");    // {%,w,h} | [ref] | h
            }
            lua_pop(L, 1);                  // {%,w,h} | [ref]
        }
        *pct = (Pico_Pct_WH) {
            L_checkfieldnum(L, i, "w"),
            L_checkfieldnum(L, i, "h"),
        };
        *ref = NULL;
        if (lua_gettop(L) >= i+1) {         // {%,w,h} | ref
            *ref = c_rect_pct(L, i+1);
            lua_pop(L, 1);                  // {%,w,h}
        }
    }
}

static void L_image_get_dim_raw_pct (lua_State* L, int i, const char* path) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);       // {w,h} | [ref]
    Pico_Dim raw;
    Pico_Pct_WH pct;
    Pico_Rect_Pct* ref;
    PICO_RAW_PCT tp = L_get_dim_raw_pct(L, i, &raw, &pct, &ref); // removes [ref]
    if (tp == PICO_RAW) {                   // {w,h}
        pico_get_image_raw(path, &raw);
        lua_pushinteger(L, raw.w);          // {w,h} | w
        lua_setfield(L, i, "w");            // {w,h}
        lua_pushinteger(L, raw.h);          // {w,h} | h
        lua_setfield(L, i, "h");            // {w,h}
    } else {
        pico_get_image_pct(path, &pct, ref);
        lua_pushnumber(L, pct.w);           // {%,w,h} | w
        lua_setfield(L, i, "w");            // {%,w,h}
        lua_pushnumber(L, pct.h);           // {%,w,h} | h
        lua_setfield(L, i, "h");            // {%,w,h}
    }
}

static void L_text_get_dim_raw_pct (lua_State* L, int i, const char* text) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);       // {w,h} | [ref]
    Pico_Dim raw;
    Pico_Pct_WH pct;
    Pico_Rect_Pct* ref;
    PICO_RAW_PCT tp = L_get_dim_raw_pct(L, i, &raw, &pct, &ref); // removes [ref]
    if (tp == PICO_RAW) {                   // {w,h}
        pico_get_text_raw(text, &raw);
        lua_pushinteger(L, raw.w);          // {w,h} | w
        lua_setfield(L, i, "w");            // {w,h}
        lua_pushinteger(L, raw.h);          // {w,h} | h
        lua_setfield(L, i, "h");            // {w,h}
    } else {
        pico_get_text_pct(text, &pct, ref);
        lua_pushnumber(L, pct.w);           // {%,w,h} | w
        lua_setfield(L, i, "w");            // {%,w,h}
        lua_pushnumber(L, pct.h);           // {%,w,h} | h
        lua_setfield(L, i, "h");            // {%,w,h}
    }
}

static int l_get_image (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);  // path | [dim | ref]

    // pico.get.image(path) -> {w, h}
    if (lua_gettop(L) == 1) {
        Pico_Dim dim = pico_get_image(path);
        lua_newtable(L);                        // path | dim
        lua_pushinteger(L, dim.w);              // path | dim | w
        lua_setfield(L, -2, "w");               // path | dim
        lua_pushinteger(L, dim.h);              // path | dim | h
        lua_setfield(L, -2, "h");               // path | dim

    // pico.get.image(path, dim, [ref])
    } else {
        L_image_get_dim_raw_pct(L, 2, path);    // path | *dim*
    }
    return 1;
}

static int l_get_text (lua_State* L) {
    // pico.get.text(h, text) -> w
    if (lua_type(L, 1) == LUA_TNUMBER) {
        int h = luaL_checknumber(L, 1);
        const char* text = luaL_checkstring(L, 2);
        int w = pico_get_text(h, text);
        lua_pushinteger(L, w);

    // pico.get.text(text, dim, [ref])
    } else {
        const char* text = luaL_checkstring(L, 1);  // text | dim | [ref]
        L_text_get_dim_raw_pct(L, 2, text);         // text | *dim*
    }
    return 1;
}

static int l_get_ticks (lua_State* L) {
    Uint32 ms = pico_get_ticks();
    lua_pushinteger(L, ms);         // ms
    return 1;                       // [ms]
}

static int l_get_view (lua_State* L) {
    int grid;
    int fs;
    Pico_Dim  phy;
    Pico_Rect dst;
    Pico_Dim  log;
    Pico_Rect src;
    Pico_Rect clip;

    // TODO: dst, src, clip
    pico_get_view(&grid, &fs, &phy, NULL, &log, NULL, NULL);

    lua_newtable(L);                    // T

    lua_pushboolean(L, grid);           // T | grid
    lua_setfield(L, -2, "grid");        // T

    lua_pushboolean(L, fs);             // T | fs
    lua_setfield(L, -2, "fullscreen");  // T

    lua_newtable(L);                    // T | win
    lua_pushinteger(L, phy.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, phy.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "window");      // T

    lua_newtable(L);                    // T | win_tgt
    lua_pushinteger(L, dst.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, dst.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, dst.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, dst.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "target");      // T

    lua_newtable(L);                    // T | wld
    lua_pushinteger(L, log.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, log.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "world");       // T

    lua_newtable(L);                    // T | wld_src
    lua_pushinteger(L, src.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, src.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, src.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, src.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "source");      // T

    lua_newtable(L);                    // T | wld_clip
    lua_pushinteger(L, clip.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, clip.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, clip.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, clip.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "clip");        // T

    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_alpha (lua_State* L) {
    int a = luaL_checkinteger(L, 1);
    pico_set_alpha(a);
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_expert(on);
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

static int l_set_style (lua_State* L) {
    const char* s = luaL_checkstring(L, 1);     // s
    lua_pushlightuserdata(L, (void*)&KEY);      // s | K
    lua_gettable(L, LUA_REGISTRYINDEX);         // s | G
    lua_getfield(L, -1, "styles");              // s | G | styles
    lua_pushvalue(L, 1);                        // s | G | styles | s
    lua_gettable(L, -2);                        // s | G | styles | *s*

    int ok;
    int ss = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        luaL_error(L, "invalid style \"%s\"", lua_tostring(L,1));
    }

    pico_set_style(ss);
    return 0;
}

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);   // title
    pico_set_title(title);
    return 0;
}

static int l_set_view (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // T

    Pico_Dim  xphy,  *xxphy  = NULL;
    Pico_Rect xdst,  *xxdst  = NULL;
    Pico_Dim  xlog,  *xxlog  = NULL;
    Pico_Rect xsrc,  *xxsrc  = NULL;
    Pico_Rect xclip, *xxclip = NULL;

    int grid = -1;
    lua_getfield(L, 1, "grid");         // T | grid
    if (!lua_isnil(L, -1)) {
        grid = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                      // T

    int fs = -1;
    lua_getfield(L, 1, "fullscreen");   // T | fs
    if (!lua_isnil(L, -1)) {
        fs = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                      // T

    lua_getfield(L, 1, "window");       // T | phy
    if (!lua_isnil(L, -1)) {
        Pico_Pct_WH xpct;
        PICO_RAW_PCT tp = c_dim_raw_pct(L, lua_gettop(L), &xphy, &xpct);
        if (tp == PICO_PCT) {
            Pico_Dim cur;
            pico_get_view(NULL, NULL, &cur, NULL, NULL, NULL, NULL);
            xphy = (Pico_Dim) { xpct.w*cur.w , xpct.h*cur.h };
        }
        xxphy = &xphy;
    }
    lua_pop(L, 1);                      // T

    lua_getfield(L, 1, "target");       // T | dst
    if (!lua_isnil(L, -1)) {
        xdst = c_rect_raw_pct_raw(L, lua_gettop(L));
        xxdst = &xdst;
    }
    lua_pop(L, 1);                      // T

    lua_getfield(L, 1, "world");        // T | log
    if (!lua_isnil(L, -1)) {
        Pico_Pct_WH xpct;
        PICO_RAW_PCT tp = c_dim_raw_pct(L, lua_gettop(L), &xlog, &xpct);
        if (tp == PICO_PCT) {
            Pico_Dim cur;
            pico_get_view(NULL, NULL, NULL, NULL, &cur, NULL, NULL);
            xlog = (Pico_Dim) { xpct.w*cur.w , xpct.h*cur.h };
        }
        xxlog = &xlog;
    }
    lua_pop(L, 1);                      // T

    lua_getfield(L, 1, "source");       // T | src
    if (!lua_isnil(L, -1)) {
        xsrc = c_rect_raw_pct_raw(L, lua_gettop(L));
        xxsrc = &xsrc;
    }
    lua_pop(L, 1);                      // T

    lua_getfield(L, 1, "clip");         // T | clip
    if (!lua_isnil(L, -1)) {
        xclip = c_rect_raw_pct_raw(L, lua_gettop(L));
        xxclip = &xclip;
    }
    lua_pop(L, 1);                      // T

    pico_set_view_raw(grid, fs, xxphy, xxdst, xxlog, xxsrc, xxclip);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checknumber(L, 1);       // ms
    pico_input_delay(ms);
    return 0;
}

static void L_mouse_button (lua_State* L, int but) {
    lua_pushlightuserdata(L, (void*)&KEY);      // . | K
    lua_gettable(L, LUA_REGISTRYINDEX);         // . | G
    lua_getfield(L, -1, "buttons");             // . | G | buttons
    lua_geti(L, -1, but);                       // . | G | buttons | but
    lua_replace(L, -3);                         // . | but | buttons
    lua_pop(L, 1);                              // . | but
}

static int l_input_event (lua_State* L) {
    // pico.input.event()
    // pico.input.event(1000)
    // pico.input.event('key.dn')
    // pico.input.event('key.dn', 1000)

    int ms = -1;
    if (lua_isinteger(L,-1)) {
        ms = lua_tointeger(L, 1);
    }

    int evt = PICO_ANY;
    if (lua_type(L,1) == LUA_TSTRING) {
        lua_pushlightuserdata(L, (void*)&KEY);  // "e" | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // "e" | G
        lua_getfield(L, -1, "events");          // "e" | G | events
        lua_pushvalue(L, 1);                    // "e" | G | events | "e"
        lua_gettable(L, -2);                    // "e" | G | events | e
        int ok;
        evt = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "invalid event \"%s\"", lua_tostring(L,1));
        }
    }

    Pico_Event e;
    int ise = 1;
    if (ms == -1) {
        pico_input_event(&e, evt);
    } else {
        ise = pico_input_event_timeout(&e, PICO_ANY, ms);
    }

    if (!ise) {
        return 0;
    }

    lua_newtable(L);    // . | t

    switch (e.type) {
        case PICO_QUIT:
            lua_pushstring(L, "quit");              // . | t | tag
            lua_setfield(L, -2, "tag");             // . | t
            break;
        case PICO_MOUSEMOTION:
            lua_pushstring(L, "mouse.motion");      // . | t | tag
            lua_setfield(L, -2, "tag");             // . | t
            lua_pushinteger(L, e.motion.x);         // . | t | x
            lua_setfield(L, -2, "x");               // . | t
            lua_pushinteger(L, e.motion.y);         // . | t | y
            lua_setfield(L, -2, "y");               // . | t
            break;
        case PICO_MOUSEBUTTONDOWN:
        case PICO_MOUSEBUTTONUP:
            lua_pushstring(L,                       // . | t | tag
                (e.type == PICO_MOUSEBUTTONDOWN ?
                    "mouse.button.dn" : "mouse.button.up"));
            lua_setfield(L, -2, "tag");             // . | t
            lua_pushinteger(L, e.button.x);         // . | t | x
            lua_setfield(L, -2, "x");               // . | t
            lua_pushinteger(L, e.button.y);         // . | t | y
            lua_setfield(L, -2, "y");               // . | t
            L_mouse_button(L, e.button.button);     // . | t | but
            lua_setfield(L, -2, "but");             // . | t
            break;
        case PICO_KEYDOWN:
        case PICO_KEYUP: {
            lua_pushstring(L,                       // . | t | tag
                (e.type == PICO_KEYDOWN ?
                    "key.dn" : "key.up"));
            lua_setfield(L, -2, "tag");             // . | t
            const char* key = SDL_GetKeyName(e.key.keysym.sym);
            lua_pushstring(L, key);                 // . | t | key
            lua_setfield(L, -2, "key");             // . | t
            break;
        }
        default:
            //assert(0 && "TODO: e.type");
    }

    return 1;           // . | [t]
}

///////////////////////////////////////////////////////////////////////////////

static int l_output_clear (lua_State* L) {
    pico_output_clear();
    return 0;
}

static int l_output_draw_buffer (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T | rect={x,y}
    luaL_checktype(L, 2, LUA_TTABLE);       // T | rect

    lua_len(L, 1);                          // T | rect | l
    int l = lua_tointeger(L, -1);
    lua_geti(L, 1, 1);                      // T | rect | l | T[1]
    if (lua_type(L,-1) != LUA_TTABLE) {
        return luaL_error(L, "expected column tables");
    }
    lua_len(L, -1);                         // T | rect | l | T[1] | c
    int c = lua_tointeger(L, -1);
    lua_pop(L, 3);                          // T | rect

    Pico_Color_A buf[l][c];
    for (int i=1; i<=l; i++) {
        lua_geti(L, 1, i);                  // T | rect | T[i]
        if (lua_type(L,-1) != LUA_TTABLE) {
            return luaL_error(L, "expected table at column %d", i);
        }
        for (int j=1; j<=c; j++) {
            lua_geti(L, -1, j);             // T | rect | T[i] | T[j]
            if (lua_type(L,-1) != LUA_TTABLE) {
                return luaL_error(L, "expected color at position [%d,%d]", i, j);
            }
            Pico_Color_A clr = c_color_a_t(L, lua_gettop(L));
            buf[i-1][j-1] = clr;
            lua_pop(L, 1);                  // T | rect | T[i]
        }
        lua_pop(L, 1);                      // T | rect
    }

    Pico_Rect raw;
    Pico_Rect_Pct* pct;
    PICO_RAW_PCT tp = c_rect_raw_pct(L, 2, &raw, &pct);

    if (tp == PICO_RAW) {
        pico_output_draw_buffer_raw((Pico_Dim){c,l}, (Pico_Color_A*)buf, raw);
    } else {
        pico_output_draw_buffer_pct((Pico_Dim){c,l}, (Pico_Color_A*)buf, pct);
    }
    return 0;
}

static int l_output_draw_image (lua_State* L) {
    luaL_checktype(L, 1, LUA_TSTRING);  // path | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    const char* path = lua_tostring(L, 1);

    Pico_Rect raw;
    Pico_Rect_Pct* pct;
    PICO_RAW_PCT tp = c_rect_raw_pct(L, 2, &raw, &pct);

    if (tp == PICO_RAW) {
        pico_output_draw_image_raw(path, raw);
    } else {
        pico_output_draw_image_pct(path, pct);
    }

    return 0;
}

static int l_output_draw_line (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // p1 | p2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Pos p1 = c_pos_raw_pct_raw(L, 1);
    Pico_Pos p2 = c_pos_raw_pct_raw(L, 2);

    pico_output_draw_line_raw(p1, p2);
    return 0;
}

static int l_output_draw_oval (lua_State* L) {
    if (lua_type(L,1) == LUA_TSTRING) {             // 'C' | x | y | w | h
        Pico_Pct_XY anc = c_anchor(L, 1);
        Pico_Rect_Pct pct = {
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4),
            luaL_checknumber(L, 5),
            anc,
            NULL
        };
        pico_output_draw_oval_pct(&pct);
    } else if (lua_type(L,1) == LUA_TNUMBER) {      // x | y | w | h
        Pico_Rect raw = {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4)
        };
        pico_output_draw_oval_raw(raw);
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);           // { x, y, w, h }
        Pico_Rect raw;
        Pico_Rect_Pct* pct;
        PICO_RAW_PCT tp = c_rect_raw_pct(L, 1, &raw, &pct);
        if (tp == PICO_RAW) {
            pico_output_draw_oval_raw(raw);
        } else {
            pico_output_draw_oval_pct(pct);
        }
    }
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    if (lua_type(L,1) == LUA_TSTRING) {             // 'C' | x | y
        Pico_Pct_XY anc = c_anchor(L, 1);
        Pico_Pos_Pct pct = {
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            anc,
            NULL
        };
        pico_output_draw_pixel_pct(&pct);
    } else if (lua_type(L,1) == LUA_TNUMBER) {      // x | y
        Pico_Pos raw = {
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

static int l_output_draw_pixels (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pixels={{x,y}}
    lua_len(L, 1);                          // pxs | n
    int n = lua_tointeger(L, -1);
    Pico_Pos pxs[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pxs | n | pxs[i]
        if (lua_type(L,3) != LUA_TTABLE) {
            return luaL_error(L, "expected position at index %d", i);
        }
        pxs[i-1] = c_pos_raw_pct_raw(L,3);
        lua_pop(L, 1);                      // pxs | n
    }
    pico_output_draw_pixels_raw(n, pxs);
    return 0;
}

static int l_output_draw_poly (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pts={{x,y}}
    lua_len(L, 1);                          // pts | n
    int n = lua_tointeger(L, -1);
    Pico_Pos poly[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pts | n | {x,y}
        int k = lua_gettop(L);
        if (lua_type(L,k) != LUA_TTABLE) {
            return luaL_error(L, "expected point at index %d", i);
        }
        poly[i-1] = c_pos_raw_pct_raw(L, k);
        lua_pop(L, 1);                      // pts | n
    }
    pico_output_draw_poly_raw(n, poly);
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    if (lua_type(L,1) == LUA_TSTRING) {             // 'C' | x | y | w | h
        Pico_Pct_XY anc = c_anchor(L, 1);
        Pico_Rect_Pct pct = {
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4),
            luaL_checknumber(L, 5),
            anc,
            NULL
        };
        pico_output_draw_rect_pct(&pct);
    } else if (lua_type(L,1) == LUA_TNUMBER) {      // x | y | w | h
        Pico_Rect raw = {
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

static int l_output_draw_tri (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);   // p1 | p2 | p3
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Pos p1 = c_pos_raw_pct_raw(L, 1);
    Pico_Pos p2 = c_pos_raw_pct_raw(L, 2);
    Pico_Pos p3 = c_pos_raw_pct_raw(L, 3);

    pico_output_draw_tri_raw(p1, p2, p3);
    return 0;
}

static int l_output_present (lua_State* L) {
    pico_output_present();
    return 0;
}

static int l_output_screenshot (lua_State* L) {
    char* path = NULL;
    if (lua_type(L,1) == LUA_TSTRING) {         // path
        path = (char*) lua_tostring(L, 1);
    }

    char* ret = NULL;

    int n = lua_gettop(L);
    if (n <= 1) {
        ret = (char*) pico_output_screenshot(path);
        goto _RET_;
    } else {                                    // path | rect
        Pico_Rect raw;
        Pico_Rect_Pct* pct;
        PICO_RAW_PCT tp = c_rect_raw_pct(L, 2, &raw, &pct);
        if (tp == PICO_RAW) {
            ret = (char*) pico_output_screenshot_raw(path, raw);
        } else {
            ret = (char*) pico_output_screenshot_pct(path, pct);
        }
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

static const luaL_Reg ll_color[] = {
    { "darker",  l_color_darker  },
    { "lighter", l_color_lighter },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "image", l_get_image },
    { "text",  l_get_text  },
    { "ticks", l_get_ticks },
    { "view",  l_get_view  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "alpha",  l_set_alpha  },
    { "expert", l_set_expert },
    { "style",  l_set_style  },
    { "title",  l_set_title  },
    { "view",   l_set_view   },
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
    { "event", l_input_event },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_output[] = {
    { "clear",      l_output_clear      },
    { "present",    l_output_present    },
    { "screenshot", l_output_screenshot },
    { NULL, NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "buffer", l_output_draw_buffer },
    { "image",  l_output_draw_image  },
    { "line",   l_output_draw_line   },
    { "oval",   l_output_draw_oval   },
    { "pixel",  l_output_draw_pixel  },
    { "pixels", l_output_draw_pixels },
    { "poly",   l_output_draw_poly   },
    { "rect",   l_output_draw_rect   },
    { "text",   l_output_draw_text   },
    { "tri",    l_output_draw_tri    },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico_native (lua_State* L) {
    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_cv);                  // pico | cv
    lua_setfield(L, -2, "cv");              // pico

    luaL_newlib(L, ll_vs);                  // pico | vs
    lua_setfield(L, -2, "vs");              // pico

    luaL_newlib(L, ll_color);               // pico | color
    lua_setfield(L, -2, "color");           // pico

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
        lua_setfield(L, -2, "anchors");                   // pico | G
        lua_pop(L, 1);                                    // pico
    }                                                     // pico

    // colors
    {
        lua_pushlightuserdata(L, (void*)&KEY);                  // pico | K
        lua_gettable(L, LUA_REGISTRYINDEX);                     // pico | G
        lua_newtable(L);                                        // pico | G | clrs
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_BLACK);     // pico | G | clrs | black
        lua_setfield(L, -2, "black");                           // pico | G | clrs
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_WHITE);     // . | white
        lua_setfield(L, -2, "white");                           // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_GRAY);      // . | gray
        lua_setfield(L, -2, "gray");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_SILVER);    // . | silver
        lua_setfield(L, -2, "silver");                          // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_RED);       // . | red
        lua_setfield(L, -2, "red");                             // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_GREEN);     // . | green
        lua_setfield(L, -2, "green");                           // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_BLUE);      // . | blue
        lua_setfield(L, -2, "blue");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_YELLOW);    // . | yellow
        lua_setfield(L, -2, "yellow");                          // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_CYAN);      // . | cyan
        lua_setfield(L, -2, "cyan");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_MAGENTA);   // . | magenta
        lua_setfield(L, -2, "magenta");                         // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_ORANGE);    // . | orange
        lua_setfield(L, -2, "orange");                         // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_PURPLE);    // . | purple
        lua_setfield(L, -2, "purple");                          // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_PINK);      // . | pink
        lua_setfield(L, -2, "pink");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_BROWN);     // . | brown
        lua_setfield(L, -2, "brown");                           // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_LIME);      // . | lime
        lua_setfield(L, -2, "lime");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_TEAL);      // . | teal
        lua_setfield(L, -2, "teal");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_NAVY);      // . | navy
        lua_setfield(L, -2, "navy");                            // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_MAROON);    // . | maroon
        lua_setfield(L, -2, "maroon");                          // .
        lua_pushlightuserdata(L, (void*)&PICO_COLOR_OLIVE);     // . | olive
        lua_setfield(L, -2, "olive");                           // pico | G | clrs
        lua_setfield(L, -2, "colors");                          // pico | G
        lua_pop(L, 1);                                          // pico
    }

    // events
    {                                               // pico
        lua_pushlightuserdata(L, (void*)&KEY);      // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);         // . | G
        lua_newtable(L);                            // . | G | events
        lua_pushinteger(L, PICO_QUIT);              // . | G | events | QT
        lua_setfield(L, -2, "quit");                // . | G | events
        lua_pushinteger(L, PICO_KEYDOWN);           // . | G | events | DN
        lua_setfield(L, -2, "key.dn");              // . | G | events
        lua_pushinteger(L, PICO_KEYUP);             // . | G | events | UP
        lua_setfield(L, -2, "key.up");              // . | G | events
        lua_pushinteger(L, PICO_MOUSEBUTTONDOWN);   // . | G | events | DN
        lua_setfield(L, -2, "mouse.button.dn");     // . | G | events
        lua_setfield(L, -2, "events");              // . | G
        lua_pop(L, 1);                              // pico

        // mouse buttons
        {                                           // pico
            lua_pushlightuserdata(L, (void*)&KEY);  // . | K
            lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
            lua_newtable(L);                        // . | G | buttons

            lua_pushinteger(L, PICO_MOUSE_BUTTON_LEFT);
            lua_pushstring(L, "left");              // . | G | buttons | L | "l"
            lua_settable(L, -3);                    // . | G | buttons

            lua_pushinteger(L, PICO_MOUSE_BUTTON_MIDDLE);
            lua_pushstring(L, "middle");            // . | G | buttons | M | "m"
            lua_settable(L, -3);                    // . | G | buttons

            lua_pushinteger(L, PICO_MOUSE_BUTTON_RIGHT);
            lua_pushstring(L, "right");             // . | G | buttons | R | "r"
            lua_settable(L, -3);                    // . | G | buttons

            lua_setfield(L, -2, "buttons");         // . | G
            lua_pop(L, 1);                          // pico
        }
    }

    // styles
    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | styles
        lua_pushinteger(L, PICO_STYLE_FILL);    // . | G | styles | fill
        lua_setfield(L, -2, "fill");            // . | G | styles
        lua_pushinteger(L, PICO_STYLE_STROKE);  // . | G | styles | stroke
        lua_setfield(L, -2, "stroke");          // . | G | styles
        lua_setfield(L, -2, "styles");          // . | G
        lua_pop(L, 1);                          // .
    }

    return 1;   // [pico]
}

