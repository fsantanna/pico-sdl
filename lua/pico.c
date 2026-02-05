#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <string.h>

#include "../src/pico.h"


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

static char c_mode (lua_State* L, int i, int asr) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    char mode = '!';
    lua_geti(L, i, 1);                          // T | [1]
    if (!lua_isnil(L, -1)) {
        if (!lua_isstring(L, -1)) {
            luaL_error(L, "invalid mode at index 1");
        }
        const char* s = lua_tostring(L, -1);
        mode = s[0];
    } else if (asr) {
        luaL_error(L, "invalid mode at index 1");
    }
    lua_pop(L, 1);                              // T
    if (mode!='!' && mode!='%' && mode!='#') {
        luaL_error(L, "invalid mode '%c': expected '!', '%%', or '#'", mode);
    }
    return mode;
}

static Pico_Anchor c_anchor (lua_State* L, int i) {
    assert(i > 0);
    lua_getfield(L, i, "anc");                      // T | anc
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);                              // T
        return PICO_ANCHOR_C;
    } else {
        int top = lua_gettop(L);
        if (lua_type(L, top) == LUA_TSTRING) {
            lua_pushlightuserdata(L, (void*)&KEY);      // T | anc | K
            lua_gettable(L, LUA_REGISTRYINDEX);         // T | anc | G
            lua_getfield(L, -1, "anchors");             // T | anc | G | ancs
            lua_pushvalue(L, top);                      // T | anc | G | ancs | anc
            lua_gettable(L, -2);                        // T | anc | G | ancs | *anc*
            int ok = lua_islightuserdata(L, -1);
            if (!ok) {
                luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L, top));
            }
            Pico_Anchor* anc = lua_touserdata(L, -1);
            lua_pop(L, 4);                              // T
            return *anc;
        } else if (lua_type(L, top) == LUA_TTABLE) {
            Pico_Anchor anc = (Pico_Anchor) {
                .x = L_checkfieldnum(L, top, "x"),
                .y = L_checkfieldnum(L, top, "y"),
            };
            lua_pop(L, 1);                              // T
            return anc;
        } else {
            luaL_error(L, "invalid anchor");
            __builtin_unreachable();
        }
    }
}

static Pico_Color c_color_t (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { ['!'|'%'], r,g,b }

    char mode = c_mode(L, i, 0);
    if (mode!='!' && mode!='%') {
        luaL_error(L, "invalid mode '%c': expected '!', '%%'", mode);
    }

    float r = L_checkfieldnum(L, i, "r");
    float g = L_checkfieldnum(L, i, "g");
    float b = L_checkfieldnum(L, i, "b");

    if (mode == '%') {
        return (Pico_Color) { r*255, g*255, b*255 };
    } else {
        return (Pico_Color) { r, g, b };
    }
}

static Pico_Color_A c_color_a_t (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { ['!'|'%'], r,g,b,a }

    char mode = c_mode(L, i, 0);
    if (mode!='!' && mode!='%') {
        luaL_error(L, "invalid mode '%c': expected '!' or '%%'", mode);
    }

    float a = (mode == '%') ? 1.0 : 0xFF;
    lua_getfield(L, i, "a");                // T | a
    if (!lua_isnil(L,-1)) {
        a = L_checkfieldnum(L, i, "a");
    }
    lua_pop(L, 1);                          // T

    float r = L_checkfieldnum(L, i, "r");
    float g = L_checkfieldnum(L, i, "g");
    float b = L_checkfieldnum(L, i, "b");

    if (mode == '%') {
        return (Pico_Color_A) { r*255, g*255, b*255, a*255 };
    } else {
        return (Pico_Color_A) { r, g, b, a };
    }
}

static Pico_Color c_color_s (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TSTRING);   // clr = 'red'
    lua_pushlightuserdata(L, (void*)&KEY);  // clr | . | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // clr | . | G
    lua_getfield(L, -1, "colors");          // clr | . | G | clrs
    lua_pushvalue(L, i);                    // clr | . | G | clrs | clr
    lua_gettable(L, -2);                    // clr | . | G | clrs | *clr*
    int ok = lua_islightuserdata(L, -1);
    if (!ok) {
        luaL_error(L, "invalid color \"%s\"", lua_tostring(L,i));
    }
    Pico_Color* clr = lua_touserdata(L, -1);
    lua_pop(L, 3);
    return *clr;
}

static Pico_Color c_color_st (lua_State* L, int i) {
    assert(i > 0);
    if (lua_type(L,i) == LUA_TSTRING) {
        return c_color_s(L, i);
    } else {
        luaL_checktype(L, i, LUA_TTABLE);
        return c_color_t(L, i);
    }
}

static Pico_Abs_Rect c_abs_rect (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    return (Pico_Abs_Rect) {
        (int) L_checkfieldnum(L, i, "x"),
        (int) L_checkfieldnum(L, i, "y"),
        (int) L_checkfieldnum(L, i, "w"),
        (int) L_checkfieldnum(L, i, "h"),
    };
}

static Pico_Abs_Pos c_abs_pos (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    return (Pico_Abs_Pos) {
        (int) L_checkfieldnum(L, i, "x"),
        (int) L_checkfieldnum(L, i, "y"),
    };
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Rel_Rect* c_rel_rect (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    char mode = c_mode(L, i, 1);
    Pico_Anchor anc = c_anchor(L, i);

    lua_getfield(L, i, "up");               // T | up
    Pico_Rel_Rect* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    Pico_Rel_Rect* r = lua_newuserdata(L, sizeof(Pico_Rel_Rect));
    *r = (Pico_Rel_Rect) {                  // T | ud
        .mode = mode,
        .x = L_checkfieldnum(L, i, "x"),
        .y = L_checkfieldnum(L, i, "y"),
        .w = L_checkfieldnum(L, i, "w"),
        .h = L_checkfieldnum(L, i, "h"),
        .anchor = anc,
        .up = up,
    };

    return r;                               // T | *ud*
}

static Pico_Rel_Dim* c_rel_dim (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    char mode = c_mode(L, i, 1);

    lua_getfield(L, i, "up");               // T | up
    Pico_Rel_Rect* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    Pico_Rel_Dim* d = lua_newuserdata(L, sizeof(Pico_Rel_Dim));
    *d = (Pico_Rel_Dim) {                   // T | ud
        .mode = mode,
        .w = L_checkfieldnum(L, i, "w"),
        .h = L_checkfieldnum(L, i, "h"),
        .up = up,
    };

    return d;                               // T | *ud*
}

static Pico_Rel_Pos* c_rel_pos (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    char mode = c_mode(L, i, 1);
    Pico_Anchor anc = c_anchor(L, i);

    lua_getfield(L, i, "up");               // T | up
    Pico_Rel_Rect* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    Pico_Rel_Pos* p = lua_newuserdata(L, sizeof(Pico_Rel_Pos));
    *p = (Pico_Rel_Pos) {                   // T | ud
        .mode = mode,
        .x = L_checkfieldnum(L, i, "x"),
        .y = L_checkfieldnum(L, i, "y"),
        .anchor = anc,
        .up = up,
    };

    return p;                               // T | *ud*
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Abs_Dim c_buffer_dim (lua_State* L, int i) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);

    lua_len(L, i);                              // T | l
    int l = lua_tointeger(L, -1);
    lua_geti(L, i, 1);                          // T | l | T[1]
    if (lua_type(L, -1) != LUA_TTABLE) {
        luaL_error(L, "expected row tables");
    }
    lua_len(L, -1);                             // T | l | T[1] | c
    int c = lua_tointeger(L, -1);
    lua_pop(L, 3);                              // T

    return (Pico_Abs_Dim) { .w=c, .h=l };
}

static void c_buffer_fill (lua_State* L, int i, Pico_Abs_Dim dim,
                           Pico_Color_A* buf) {
    assert(i > 0);
    for (int row=1; row<=dim.h; row++) {
        lua_geti(L, i, row);                    // T | T[row]
        if (lua_type(L, -1) != LUA_TTABLE) {
            luaL_error(L, "expected table at row %d", row);
        }
        for (int col=1; col<=dim.w; col++) {
            lua_geti(L, -1, col);               // T | T[row] | T[col]
            if (lua_type(L, -1) != LUA_TTABLE) {
                luaL_error(L, "expected color at position [%d,%d]", row, col);
            }
            buf[(row-1)*dim.w + (col-1)] = c_color_a_t(L, lua_gettop(L));
            lua_pop(L, 1);                      // T | T[row]
        }
        lua_pop(L, 1);                          // T
    }
}

///////////////////////////////////////////////////////////////////////////////

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

static int l_quit (lua_State* L) {
    pico_quit();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Rel_Pos* _c_tpl_pos (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    char mode = c_mode(L, i, 1);
    Pico_Anchor anc = c_anchor(L, i);

    lua_getfield(L, i, "up");
    Pico_Rel_Rect* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);

    Pico_Rel_Pos* p = lua_newuserdata(L, sizeof(Pico_Rel_Pos));
    *p = (Pico_Rel_Pos) {
        .mode = mode,
        .anchor = anc,
        .up = up,
    };
    return p;
}

static Pico_Rel_Rect* _c_tpl_rect (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);

    char mode = c_mode(L, i, 1);
    Pico_Anchor anc = c_anchor(L, i);

    lua_getfield(L, i, "up");
    Pico_Rel_Rect* up = NULL;
    if (!lua_isnil(L, -1)) {
        up = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);

    Pico_Rel_Rect* r = lua_newuserdata(L, sizeof(Pico_Rel_Rect));
    *r = (Pico_Rel_Rect) {
        .mode = mode,
        .anchor = anc,
        .up = up,
    };
    return r;
}

static int l_cv_pos (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // fr | [to] | [base]

    int has_to = (lua_gettop(L)>=2 && !lua_isnil(L,2));
    if (has_to) {
        luaL_checktype(L, 2, LUA_TTABLE);
    }

    Pico_Abs_Rect* base = NULL;
    Pico_Abs_Rect base_rect;
    if (lua_gettop(L) >= 3 && lua_istable(L, 3)) {
        base_rect = c_abs_rect(L, 3);
        base = &base_rect;
    }

    if (!has_to) {
        // rel_abs: fr → abs
        Pico_Rel_Pos* fr = c_rel_pos(L, 1);
        Pico_Abs_Pos abs = pico_cv_pos_rel_abs(fr, base);
        lua_newtable(L);
        lua_pushinteger(L, abs.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, abs.y);
        lua_setfield(L, -2, "y");
        return 1;
    } else {
        // to present: duck type fr
        lua_geti(L, 1, 1);
        int fr_is_rel = lua_isstring(L, -1);
        lua_pop(L, 1);

        Pico_Rel_Pos* to = _c_tpl_pos(L, 2);
        if (fr_is_rel) {
            Pico_Rel_Pos* fr = c_rel_pos(L, 1);
            pico_cv_pos_rel_rel(fr, to, base);
        } else {
            Pico_Abs_Pos fr = c_abs_pos(L, 1);
            pico_cv_pos_abs_rel(&fr, to, base);
        }

        lua_pushnumber(L, to->x);
        lua_setfield(L, 2, "x");
        lua_pushnumber(L, to->y);
        lua_setfield(L, 2, "y");
        return 0;
    }
}

static int l_cv_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // fr | [to] | [base]

    int has_to = (lua_gettop(L)>=2 && !lua_isnil(L,2));
    if (has_to) {
        luaL_checktype(L, 2, LUA_TTABLE);
    }

    Pico_Abs_Rect* base = NULL;
    Pico_Abs_Rect base_rect;
    if (lua_gettop(L) >= 3 && lua_istable(L, 3)) {
        base_rect = c_abs_rect(L, 3);
        base = &base_rect;
    }

    if (!has_to) {
        // rel_abs: fr → abs
        Pico_Rel_Rect* fr = c_rel_rect(L, 1);
        Pico_Abs_Rect abs = pico_cv_rect_rel_abs(fr, base);
        lua_newtable(L);
        lua_pushinteger(L, abs.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, abs.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, abs.w);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, abs.h);
        lua_setfield(L, -2, "h");
        return 1;
    } else {
        // to present: duck type fr
        lua_geti(L, 1, 1);
        int fr_is_rel = lua_isstring(L, -1);
        lua_pop(L, 1);

        Pico_Rel_Rect* to = _c_tpl_rect(L, 2);
        if (fr_is_rel) {
            Pico_Rel_Rect* fr = c_rel_rect(L, 1);
            pico_cv_rect_rel_rel(fr, to, base);
        } else {
            Pico_Abs_Rect fr = c_abs_rect(L, 1);
            pico_cv_rect_abs_rel(&fr, to, base);
        }

        lua_pushnumber(L, to->x);
        lua_setfield(L, 2, "x");
        lua_pushnumber(L, to->y);
        lua_setfield(L, 2, "y");
        lua_pushnumber(L, to->w);
        lua_setfield(L, 2, "w");
        lua_pushnumber(L, to->h);
        lua_setfield(L, 2, "h");
        return 0;
    }
}

static int l_vs_pos_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rel_Pos*  pos  = c_rel_pos(L, 1);
    Pico_Rel_Rect* rect = c_rel_rect(L, 2);

    int ret = pico_vs_pos_rect(pos, rect);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_vs_rect_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // r1 | r2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rel_Rect* r1 = c_rel_rect(L, 1);
    Pico_Rel_Rect* r2 = c_rel_rect(L, 2);

    int ret = pico_vs_rect_rect(r1, r2);
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
    Pico_Color clr = c_color_st(L, 1);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_darker(clr, pct);
    L_push_color(L, ret);
    return 1;
}

static int l_color_lighter (lua_State* L) {
    Pico_Color clr = c_color_st(L, 1);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_lighter(clr, pct);
    L_push_color(L, ret);
    return 1;
}

static int l_color_mix (lua_State* L) {
    Pico_Color c1 = c_color_st(L, 1);
    Pico_Color c2 = c_color_st(L, 2);
    Pico_Color ret = pico_color_mix(c1, c2);
    L_push_color(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static void L_dim_default_wh (lua_State* L, int i) {
    lua_getfield(L, i, "w");
    if (lua_isnil(L, -1)) {
        lua_pushinteger(L, 0);
        lua_setfield(L, i, "w");
    }
    lua_pop(L, 1);
    lua_getfield(L, i, "h");
    if (lua_isnil(L, -1)) {
        lua_pushinteger(L, 0);
        lua_setfield(L, i, "h");
    }
    lua_pop(L, 1);
}

static void L_image_get_dim (lua_State* L, int i, const char* path) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);               // rel
    L_dim_default_wh(L, i);

    Pico_Rel_Dim* dim = c_rel_dim(L, i);            // rel | ud
    Pico_Abs_Dim abs = pico_get_image(path, dim);
    lua_pop(L, 1);                                  // rel

    lua_pushnumber(L, dim->w);
    lua_setfield(L, i, "w");
    lua_pushnumber(L, dim->h);
    lua_setfield(L, i, "h");

    lua_newtable(L);                                // rel | abs
    lua_pushnumber(L, abs.w);                       // rel | abs | w
    lua_setfield(L, -2, "w");                       // rel | abs
    lua_pushnumber(L, abs.h);                       // rel | abs | h
    lua_setfield(L, -2, "h");                       // rel | *abs*
}

static int l_get_image (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);  // path | [dim]

    if (lua_gettop(L) == 1) { // default dim={'!',w=0,h=0}
        lua_newtable(L);                        // path | dim
        lua_pushstring(L, "!");
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "h");
    }

    L_image_get_dim(L, 2, path);
    return 1;
}

static int l_get_layer (lua_State* L) {
    const char* name = pico_get_layer();
    if (name == NULL) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, name);
    }
    return 1;
}

static int l_get_text (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);  // text | dim
    luaL_checktype(L, 2, LUA_TTABLE);
    L_dim_default_wh(L, 2);

    Pico_Rel_Dim* dim = c_rel_dim(L, 2);            // text | dim | ud
    Pico_Abs_Dim abs = pico_get_text(text, dim);
    lua_pop(L, 1);                                  // text | dim

    lua_pushnumber(L, dim->w);
    lua_setfield(L, 2, "w");
    lua_pushnumber(L, dim->h);
    lua_setfield(L, 2, "h");

    lua_newtable(L);                                // text | dim | abs
    lua_pushnumber(L, abs.w);
    lua_setfield(L, -2, "w");
    lua_pushnumber(L, abs.h);
    lua_setfield(L, -2, "h");                       // text | dim | *abs*
    return 1;
}

static int l_get_ticks (lua_State* L) {
    Uint32 ms = pico_get_ticks();
    lua_pushinteger(L, ms);         // ms
    return 1;                       // [ms]
}

static int l_get_window (lua_State* L) {
    const char* title;
    int fs;
    Pico_Abs_Dim dim;

    pico_get_window(&title, &fs, &dim);

    lua_newtable(L);                    // T

    lua_pushstring(L, title);           // T | title
    lua_setfield(L, -2, "title");       // T

    lua_pushboolean(L, fs);             // T | fs
    lua_setfield(L, -2, "fullscreen");  // T

    lua_newtable(L);                    // T | dim
    lua_pushinteger(L, dim.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, dim.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "dim");         // T

    return 1;
}

static int l_get_view (lua_State* L) {
    int grid;
    //Pico_Abs_Rect dst;
    Pico_Abs_Dim  log;
    //Pico_Abs_Rect src;
    //Pico_Abs_Rect clip;
    Pico_Abs_Dim  tile;

    // TODO: target, source, clip
    pico_get_view(&grid, &log, NULL, NULL, NULL, &tile);

    lua_newtable(L);                    // T

    lua_pushboolean(L, grid);           // T | grid
    lua_setfield(L, -2, "grid");        // T

#if 0
    lua_newtable(L);                    // T | dst
    lua_pushinteger(L, dst.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, dst.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, dst.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, dst.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "target");      // T
#endif

    lua_newtable(L);                    // T | log
    lua_pushinteger(L, log.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, log.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "dim");         // T

    lua_newtable(L);                    // T | tile
    lua_pushinteger(L, tile.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, tile.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "tile");        // T

#if 0
    lua_newtable(L);                    // T | src
    lua_pushinteger(L, src.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, src.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, src.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, src.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "source");      // T
#endif

#if 0
    lua_newtable(L);                    // T | clip
    lua_pushinteger(L, clip.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, clip.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, clip.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, clip.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "clip");        // T
#endif

    return 1;
}

static int l_get_mouse (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);           // pos

    char mode = c_mode(L, 1, 1);
    Pico_Rel_Pos pos = { .mode = mode };

    int button = PICO_MOUSE_BUTTON_NONE;
    if (lua_gettop(L) >= 2 && lua_isinteger(L, 2)) {
        button = lua_tointeger(L, 2);
    }

    int ret = pico_get_mouse(&pos, button);

    lua_pushnumber(L, pos.x);
    lua_setfield(L, 1, "x");
    lua_pushnumber(L, pos.y);
    lua_setfield(L, 1, "y");

    lua_pushboolean(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_alpha (lua_State* L) {
    int a = luaL_checkinteger(L, 1);
    pico_set_alpha(a);
    return 0;
}

static int l_set_crop (lua_State* L) {
    Pico_Abs_Rect r;
    if (lua_gettop(L) == 0) {               // -
        r = (Pico_Abs_Rect) {0,0,0,0};
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);   // r = {x,y,w,h}
        r = c_abs_rect(L, 1);
    }
    pico_set_crop(r);
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_expert(on);
    return 0;
}

static int l_set_color_clear (lua_State* L) {
    Pico_Color clr = c_color_st(L, 1);
    pico_set_color_clear(clr);
    return 0;
}

static int l_set_color_draw (lua_State* L) {
    Pico_Color clr = c_color_st(L, 1);
    pico_set_color_draw(clr);
    return 0;
}

static int l_set_layer (lua_State* L) {
    const char* name = NULL;
    if (lua_gettop(L)>=1 && !lua_isnil(L, 1)) {
        name = luaL_checkstring(L, 1);
    }
    pico_set_layer(name);
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
        luaL_error(L, "invalid style \"%s\"", s);
    }

    pico_set_style(ss);
    return 0;
}

static int l_set_window (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    const char* title = NULL;
    lua_getfield(L, 1, "title");            // T | title
    if (!lua_isnil(L, -1)) {
        title = lua_tostring(L, -1);
    }
    lua_pop(L, 1);                          // T

    int fs = -1;
    lua_getfield(L, 1, "fullscreen");       // T | fs
    if (!lua_isnil(L, -1)) {
        fs = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                          // T

    Pico_Rel_Dim* xdim = NULL;
    lua_getfield(L, 1, "dim");              // T | dim
    if (!lua_isnil(L, -1)) {
        xdim = c_rel_dim(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    pico_set_window(title, fs, xdim);
    return 0;
}

static int l_set_dim (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T
    Pico_Rel_Dim* xdim = c_rel_dim(L, 1);
    pico_set_dim(xdim);
    return 0;
}

static int l_set_view (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    Pico_Rel_Rect* xdst  = NULL;
    Pico_Rel_Dim*  xwld  = NULL;
    Pico_Rel_Rect* xsrc  = NULL;
    Pico_Rel_Rect* xclip = NULL;

    int grid = -1;
    lua_getfield(L, 1, "grid");             // T | grid
    if (!lua_isnil(L, -1)) {
        grid = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "target");           // T | dst
    if (!lua_isnil(L, -1)) {
        xdst = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "dim");              // T | dim
    if (!lua_isnil(L, -1)) {
        xwld = c_rel_dim(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "source");           // T | src
    if (!lua_isnil(L, -1)) {
        xsrc = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "clip");             // T | clip
    if (!lua_isnil(L, -1)) {
        xclip = c_rel_rect(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    Pico_Abs_Dim* xtile = NULL;
    Pico_Abs_Dim tile_dim;
    lua_getfield(L, 1, "tile");             // T | tile
    if (!lua_isnil(L, -1)) {
        tile_dim.w = L_checkfieldnum(L, lua_gettop(L), "w");
        tile_dim.h = L_checkfieldnum(L, lua_gettop(L), "h");
        xtile = &tile_dim;
    }
    lua_pop(L, 1);                          // T

    pico_set_view(grid, xwld, xdst, xsrc, xclip, xtile);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_layer_empty (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);      // name | dim
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Abs_Dim dim = {
        (int) L_checkfieldnum(L, 2, "w"),
        (int) L_checkfieldnum(L, 2, "h"),
    };
    const char* ret = pico_layer_empty(name, dim);
    lua_pushstring(L, ret);                         // name | dim | *name*
    return 1;
}

static int l_layer_image (lua_State* L) {
    const char* name = NULL;
    if (!lua_isnil(L, 1)) {
        name = luaL_checkstring(L, 1);              // name | path
    }
    const char* path = luaL_checkstring(L, 2);
    const char* ret = pico_layer_image(name, path);
    lua_pushstring(L, ret);                         // name | path | *name*
    return 1;
}

static int l_layer_buffer (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);      // name | dim | buffer
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Abs_Dim dim = {
        (int) L_checkfieldnum(L, 2, "w"),
        (int) L_checkfieldnum(L, 2, "h"),
    };

    Pico_Abs_Dim buf_dim = c_buffer_dim(L, 3);
    if (buf_dim.w != dim.w || buf_dim.h != dim.h) {
        return luaL_error(L, "buffer size %dx%d doesn't match dim %dx%d",
                          buf_dim.w, buf_dim.h, dim.w, dim.h);
    }

    Pico_Color_A buf[buf_dim.h][buf_dim.w];
    c_buffer_fill(L, 3, buf_dim, (Pico_Color_A*)buf);

    const char* ret = pico_layer_buffer(name, dim, (Pico_Color_A*)buf);
    lua_pushstring(L, ret);
    return 1;
}

static int l_layer_text (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);      // name | height | text
    int height = luaL_checkinteger(L, 2);
    const char* text = luaL_checkstring(L, 3);
    const char* ret = pico_layer_text(name, height, text);
    lua_pushstring(L, ret);
    return 1;
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
            lua_pushstring(L, "!");                 // . | t | !
            lua_seti(L, -2, 1);                     // . | t
            lua_pushinteger(L, e.motion.x);         // . | t | x
            lua_setfield(L, -2, "x");               // . | t
            lua_pushinteger(L, e.motion.y);         // . | t | y
            lua_setfield(L, -2, "y");               // . | t
            lua_pushstring(L, "NE");                // . | t | NE
            lua_setfield(L, -2, "anc");             // . | t
            break;
        case PICO_MOUSEBUTTONDOWN:
        case PICO_MOUSEBUTTONUP:
            lua_pushstring(L,                       // . | t | tag
                (e.type == PICO_MOUSEBUTTONDOWN ?
                    "mouse.button.dn" : "mouse.button.up"));
            lua_setfield(L, -2, "tag");             // . | t
            lua_pushstring(L, "!");                 // . | t | !
            lua_seti(L, -2, 1);                     // . | t
            lua_pushinteger(L, e.button.x);         // . | t | x
            lua_setfield(L, -2, "x");               // . | t
            lua_pushinteger(L, e.button.y);         // . | t | y
            lua_setfield(L, -2, "y");               // . | t
            lua_pushstring(L, "NE");                // . | t | NE
            lua_setfield(L, -2, "anc");             // . | t
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
    const char* name = luaL_checkstring(L, 1);  // name | buf | rect
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Abs_Dim dim = c_buffer_dim(L, 2);
    Pico_Color_A buf[dim.h][dim.w];
    c_buffer_fill(L, 2, dim, (Pico_Color_A*)buf);

    Pico_Rel_Rect* rect = c_rel_rect(L, 3);
    pico_output_draw_buffer(name, dim, (Pico_Color_A*)buf, rect);
    return 0;
}

static int l_output_draw_image (lua_State* L) {
    luaL_checktype(L, 1, LUA_TSTRING);      // path | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    const char* path = lua_tostring(L, 1);
    L_image_get_dim(L, 2, path);

    Pico_Rel_Rect* rect = c_rel_rect(L, 2);
    pico_output_draw_image(path, rect);
    return 0;
}

static int l_output_draw_layer (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);  // name | [rect]

    Pico_Rel_Rect* rect = NULL;
    if (lua_istable(L, 2)) {
        rect = c_rel_rect(L, 2);
    }

    pico_output_draw_layer(name, rect);
    return 0;
}

static int l_output_draw_line (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1 | p2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rel_Pos* p1 = c_rel_pos(L, 1);
    Pico_Rel_Pos* p2 = c_rel_pos(L, 2);

    pico_output_draw_line(p1, p2);
    return 0;
}

static int l_output_draw_oval (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Rect* rect = c_rel_rect(L, 1);
    pico_output_draw_oval(rect);
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Pos* pos = c_rel_pos(L, 1);
    pico_output_draw_pixel(pos);
    return 0;
}

static int l_output_draw_pixels (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pixels={{...}}
    lua_len(L, 1);                          // pxs | n
    int n = lua_tointeger(L, -1);
    Pico_Rel_Pos pxs[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pxs | n | pxs[i]
        if (lua_type(L, -1) != LUA_TTABLE) {
            return luaL_error(L, "expected position at index %d", i);
        }
        Pico_Rel_Pos* p = c_rel_pos(L, lua_gettop(L));
        pxs[i-1] = *p;
        lua_pop(L, 1);                      // pxs | n
    }
    pico_output_draw_pixels(n, pxs);
    return 0;
}

static int l_output_draw_poly (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pts={{...}}
    lua_len(L, 1);                          // pts | n
    int n = lua_tointeger(L, -1);
    Pico_Rel_Pos poly[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pts | n | {...}
        int k = lua_gettop(L);
        if (lua_type(L, k) != LUA_TTABLE) {
            return luaL_error(L, "expected point at index %d", i);
        }
        Pico_Rel_Pos* p = c_rel_pos(L, k);
        poly[i-1] = *p;
        lua_pop(L, 1);                      // pts | n
    }
    pico_output_draw_poly(n, poly);
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Rect* rect = c_rel_rect(L, 1);
    pico_output_draw_rect(rect);
    return 0;
}

static int l_output_draw_text (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);  // text | rect
    luaL_checktype(L, 2, LUA_TTABLE);
    L_dim_default_wh(L, 2);
    Pico_Rel_Rect* rect = c_rel_rect(L, 2);
    pico_output_draw_text(text, rect);
    return 0;
}

static int l_output_draw_tri (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1 | p2 | p3
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Rel_Pos* p1 = c_rel_pos(L, 1);
    Pico_Rel_Pos* p2 = c_rel_pos(L, 2);
    Pico_Rel_Pos* p3 = c_rel_pos(L, 3);

    pico_output_draw_tri(p1, p2, p3);
    return 0;
}

static int l_output_present (lua_State* L) {
    pico_output_present();
    return 0;
}

static int l_output_screenshot (lua_State* L) {
    const char* path = NULL;
    if (lua_type(L, 1) == LUA_TSTRING) {
        path = lua_tostring(L, 1);
    }

    Pico_Rel_Rect* rect = NULL;
    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
        rect = c_rel_rect(L, 2);
    }

    const char* ret = pico_output_screenshot(path, rect);
    assert(ret != NULL);
    lua_pushstring(L, ret);
    return 1;
}

static int l_output_sound (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);   // path
    pico_output_sound(path);
    return 0;
}


///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_all[] = {
    { "init", l_init },
    { "quit", l_quit },
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
    { "mix",     l_color_mix     },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "image",  l_get_image  },
    { "layer",  l_get_layer  },
    { "mouse",  l_get_mouse  },
    { "text",   l_get_text   },
    { "ticks",  l_get_ticks  },
    { "view",   l_get_view   },
    { "window", l_get_window },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "alpha",  l_set_alpha  },
    { "crop",   l_set_crop   },
    { "dim",    l_set_dim    },
    { "expert", l_set_expert },
    { "layer",  l_set_layer  },
    { "style",  l_set_style  },
    { "view",   l_set_view   },
    { "window", l_set_window },
    { NULL, NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { "draw",  l_set_color_draw  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_layer[] = {
    { "buffer", l_layer_buffer },
    { "empty",  l_layer_empty  },
    { "image",  l_layer_image  },
    { "text",   l_layer_text   },
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
    { "sound",      l_output_sound      },
    { NULL, NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "buffer", l_output_draw_buffer },
    { "image",  l_output_draw_image  },
    { "layer",  l_output_draw_layer  },
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

    luaL_newlib(L, ll_layer);               // pico | layer
    lua_setfield(L, -2, "layer");           // pico

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

