#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <string.h>

#include "../src/pico.h"

static const char KEY;

static void L_reg_get (lua_State* L, const char* t, int i) {
    assert(i > 0);
    lua_pushlightuserdata(L, (void*)&KEY);  // ... | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // ... | G
    lua_getfield(L, -1, t);                 // ... | G | T
    lua_pushvalue(L, i);                    // ... | G | T | k
    lua_gettable(L, -2);                    // ... | G | T | *v*
    lua_replace(L, -3);                     // ... | *v* | T
    lua_pop(L, 1);                          // ... | *v*
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static float C_asrfieldnum (lua_State* L, int i, const char* k) {
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

static char C_mode_t (lua_State* L, int i, int asr) {
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
    if (mode!='w' && mode!='!' && mode!='%' && mode!='#') {
        luaL_error(L, "invalid mode '%c': expected '!', '%%', or '#'", mode);
    }
    return mode;
}

// Optional mode-string at arg i.
// - returns mode char if ('!', '%', '#')
// - error or '\0', otherwise
static char C_mode_s_opt (lua_State* L, int asr, int i) {
    if (!lua_isstring(L, i)) {
        return '\0';
    }
    size_t len;
    const char* s = lua_tolstring(L, i, &len);
    if (len!=1 || (s[0]!='!' && s[0]!='%' && s[0]!='#')) {
        if (asr) {
            luaL_error(L, "invalid mode '%s': expected '!', '%%', or '#'", s);
        } else {
            return '\0';
        }
    }
    return s[0];
}

// L expects optional layer at index 1, otherwise insert nil there.
// Need to distinguish layer from mode.
static void L_layer_opt_mode (lua_State* L) {
    int t = lua_type(L, 1);
    int has = (t == LUA_TNIL) || (t==LUA_TSTRING && C_mode_s_opt(L,0,1)=='\0');
    if (!has) {
        lua_pushnil(L);
        lua_insert(L, 1);
    }
}

// Detect optional realm mode as first arg.
// Returns mode char or '\0' when no mode is set.
static int C_realm_opt (lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return '\0';
    }
    const char* ms = lua_tostring(L, 1);
    if (strlen(ms) > 1) {
        return '\0';
    }
    char m = ms[0];
    if (m!='!' && m!='=' && m!='~') {
        return '\0';
    }
    return ms[0];
}

static Pico_Color C_color_s (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TSTRING);   // clr = 'red'
    L_reg_get(L, "colors", i);              // clr | . | *clr*
    int ok = lua_islightuserdata(L, -1);
    if (!ok) {
        luaL_error(L, "invalid color \"%s\"", lua_tostring(L,i));
    }
    Pico_Color* clr = lua_touserdata(L, -1);
    lua_pop(L, 1);
    return *clr;
}

static Pico_Color C_color_t (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { ['!'|'%'], r,g,b[,a] }

    char mode = C_mode_t(L, i, 0);
    if (mode!='!' && mode!='%') {
        luaL_error(L, "invalid mode '%c': expected '!', '%%'", mode);
    }

    float a = (mode == '%') ? 1.0 : 0xFF;
    lua_getfield(L, i, "a");                // T | a
    if (!lua_isnil(L,-1)) {
        a = C_asrfieldnum(L, i, "a");
    }
    lua_pop(L, 1);                          // T

    float r = C_asrfieldnum(L, i, "r");
    float g = C_asrfieldnum(L, i, "g");
    float b = C_asrfieldnum(L, i, "b");

    if (mode == '%') {
        return (Pico_Color) { r*255, g*255, b*255, a*255 };
    } else {
        return (Pico_Color) { r, g, b, a };
    }
}

static Pico_Color C_color_tis (lua_State* L, int i) {
    assert(i > 0);
    if (lua_type(L,i) == LUA_TSTRING) {
        return C_color_s(L, i);
    } else if (lua_isinteger(L, i)) {
        return pico_color_hex((uint32_t)lua_tointeger(L, i));
    } else {
        luaL_checktype(L, i, LUA_TTABLE);
        return C_color_t(L, i);
    }
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Anchor C_anchor (lua_State* L, int i) {
    assert(i > 0);
    lua_getfield(L, i, "anchor");                   // T | anchor
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);                              // T
        return PICO_ANCHOR_C;
    } else {
        int top = lua_gettop(L);
        if (lua_type(L, top) == LUA_TSTRING) {
            L_reg_get(L, "anchors", top);               // T | anc | *anc*
            int ok = lua_islightuserdata(L, -1);
            if (!ok) {
                luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L, top));
            }
            Pico_Anchor* anc = lua_touserdata(L, -1);
            lua_pop(L, 2);                              // T
            return *anc;
        } else if (lua_type(L, top) == LUA_TTABLE) {
            Pico_Anchor anc = (Pico_Anchor) {
                .x = C_asrfieldnum(L, top, "x"),
                .y = C_asrfieldnum(L, top, "y"),
            };
            lua_pop(L, 1);                              // T
            return anc;
        } else {
            luaL_error(L, "invalid anchor");
            __builtin_unreachable();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

static Pico_Abs_Dim C_pixmap_dim (lua_State* L, int i) {
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

static void C_pixmap_fill (lua_State* L, int i, Pico_Abs_Dim dim,
                           Pico_Color* buf) {
    assert(i > 0);
    for (int row=1; row<=dim.h; row++) {
        lua_geti(L, i, row);                    // T | T[row]
        if (lua_type(L, -1) != LUA_TTABLE) {
            luaL_error(L, "expected table at row %d", row);
        }
        int top = lua_gettop(L) + 1;
        for (int col=1; col<=dim.w; col++) {
            lua_geti(L, -1, col);               // T | T[row] | T[col]
            buf[(row-1)*dim.w + (col-1)] = C_color_tis(L, top);
            lua_pop(L, 1);                      // T | T[row]
        }
        lua_pop(L, 1);                          // T
    }
}

///////////////////////////////////////////////////////////////////////////////

// Reads a numeric field with default 0 if missing. Used by w/h to
// support the "infer from source" convention (w=0 or h=0).
static float C_optfieldnum (lua_State* L, int i, const char* k) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);        // t
    lua_getfield(L, i, k);                   // t | t[k]
    float v;
    if (lua_isnil(L, -1)) {
        v = 0;
    } else {
        int ok;
        v = lua_tonumberx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "expected numeric field '%s'", k);
        }
    }
    lua_pop(L, 1);
    return v;
}

static Pico_Rel_Rect C_rel_rect (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    return (Pico_Rel_Rect) {
        .mode = C_mode_t(L, i, 1),
        .x = C_asrfieldnum(L, i, "x"),
        .y = C_asrfieldnum(L, i, "y"),
        .w = C_optfieldnum(L, i, "w"),
        .h = C_optfieldnum(L, i, "h"),
        .anchor = C_anchor(L, i),
    };
}

static Pico_Rel_Dim C_rel_dim (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    return (Pico_Rel_Dim) {
        .mode = C_mode_t(L, i, 1),
        .w = C_optfieldnum(L, i, "w"),
        .h = C_optfieldnum(L, i, "h"),
    };
}

static Pico_Rel_Pos C_rel_pos (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_type(L,i) == LUA_TTABLE);
    return (Pico_Rel_Pos) {
        .mode = C_mode_t(L, i, 1),
        .x = C_asrfieldnum(L, i, "x"),
        .y = C_asrfieldnum(L, i, "y"),
        .anchor = C_anchor(L, i),
    };
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void L_image_get_dim (lua_State* L, int i, const char* path) {
    assert(i > 0);
    luaL_checktype(L, i, LUA_TTABLE);               // rel

    Pico_Rel_Dim dim = C_rel_dim(L, i);
    Pico_Abs_Dim abs = pico_get_image(path, &dim);

    lua_pushnumber(L, dim.w);
    lua_setfield(L, i, "w");
    lua_pushnumber(L, dim.h);
    lua_setfield(L, i, "h");

    lua_newtable(L);                                // rel | abs
    lua_pushnumber(L, abs.w);                       // rel | abs | w
    lua_setfield(L, -2, "w");                       // rel | abs
    lua_pushnumber(L, abs.h);                       // rel | abs | h
    lua_setfield(L, -2, "h");                       // rel | *abs*
}

static void L_push_color (lua_State* L, Pico_Color clr) {
    lua_newtable(L);
    int i = lua_gettop(L);
    {
        lua_pushinteger(L, clr.r);
        lua_setfield(L, i, "r");
        lua_pushinteger(L, clr.g);
        lua_setfield(L, i, "g");
        lua_pushinteger(L, clr.b);
        lua_setfield(L, i, "b");
        lua_pushinteger(L, clr.a);
        lua_setfield(L, i, "a");
    }
}

static void L_push_rel_rect (lua_State* L, Pico_Rel_Rect* r) {
    lua_newtable(L);
    int i = lua_gettop(L);
    {
        char mode[2] = { r->mode, '\0' };
        lua_pushstring(L, mode);
        lua_rawseti(L, i, 1);
    }
    {
        lua_pushnumber(L, r->x);
        lua_setfield(L, i, "x");
        lua_pushnumber(L, r->y);
        lua_setfield(L, i, "y");
        lua_pushnumber(L, r->w);
        lua_setfield(L, i, "w");
        lua_pushnumber(L, r->h);
        lua_setfield(L, i, "h");
    }
    {
        lua_newtable(L);
        int j = lua_gettop(L);
        lua_pushnumber(L, r->anchor.x);
        lua_setfield(L, j, "x");
        lua_pushnumber(L, r->anchor.y);
        lua_setfield(L, j, "y");
        lua_setfield(L, i, "anchor");
    }
}

static void L_push_rel_pos (lua_State* L, Pico_Rel_Pos* p) {
    lua_newtable(L);
    int i = lua_gettop(L);
    {
        char mode[2] = { p->mode, '\0' };
        lua_pushstring(L, mode);
        lua_rawseti(L, i, 1);
    }
    {
        lua_pushnumber(L, p->x);
        lua_setfield(L, i, "x");
        lua_pushnumber(L, p->y);
        lua_setfield(L, i, "y");
    }
    {
        lua_newtable(L);
        int j = lua_gettop(L);
        lua_pushnumber(L, p->anchor.x);
        lua_setfield(L, j, "x");
        lua_pushnumber(L, p->anchor.y);
        lua_setfield(L, j, "y");
        lua_setfield(L, i, "anchor");
    }
}

static void L_push_rel_dim (lua_State* L, Pico_Rel_Dim* d) {
    lua_newtable(L);
    int i = lua_gettop(L);
    {
        char mode[2] = { d->mode, '\0' };
        lua_pushstring(L, mode);
        lua_rawseti(L, i, 1);
    }
    {
        lua_pushnumber(L, d->w);
        lua_setfield(L, i, "w");
        lua_pushnumber(L, d->h);
        lua_setfield(L, i, "h");
    }
}

static void L_set_keyboard (lua_State* L, int idx, Pico_Keyboard* k) {
    const char* name = SDL_GetKeyName(k->key);
    lua_pushstring(L, name);                    // T | key
    lua_setfield(L, idx, "key");                // T
    lua_pushboolean(L, k->ctrl);                // T | ctrl
    lua_setfield(L, idx, "ctrl");               // T
    lua_pushboolean(L, k->shift);               // T | shift
    lua_setfield(L, idx, "shift");              // T
    lua_pushboolean(L, k->alt);                 // T | alt
    lua_setfield(L, idx, "alt");                // T
}

static void L_set_mouse (lua_State* L, int idx, Pico_Mouse* m) {
    lua_pushstring(L, (char[]){m->mode, 0});    // T | mode
    lua_seti(L, idx, 1);                        // T
    lua_pushnumber(L, m->x);                    // T | x
    lua_setfield(L, idx, "x");                  // T
    lua_pushnumber(L, m->y);                    // T | y
    lua_setfield(L, idx, "y");                  // T
    {
        lua_newtable(L);                        // T | anc
        int j = lua_gettop(L);
        lua_pushnumber(L, m->anchor.x);         // T | anc | ax
        lua_setfield(L, j, "x");                // T | anc
        lua_pushnumber(L, m->anchor.y);         // T | anc | ay
        lua_setfield(L, j, "y");                // T | anc
        lua_setfield(L, idx, "anchor");         // T
    }
    lua_pushboolean(L, m->left);                // T | left
    lua_setfield(L, idx, "left");               // T
    lua_pushboolean(L, m->right);               // T | right
    lua_setfield(L, idx, "right");              // T
    lua_pushboolean(L, m->middle);              // T | middle
    lua_setfield(L, idx, "middle");             // T
}

///////////////////////////////////////////////////////////////////////////////
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
// CV: unified pos/rect/dim — (L_to, to_or_mode, L_fr, fr)
///////////////////////////////////////////////////////////////////////////////

// L expects optional layer at index i, otherwise insert nil there.
static void L_layer_opt (lua_State* L, int i) {
    assert(i > 0);
    assert(lua_gettop(L) >= i-1);
    int t = lua_type(L, i);
    if (t!=LUA_TNIL && t!=LUA_TSTRING) {
        lua_pushnil(L);
        lua_insert(L, i);
    }
}

static int l_cv_dim (lua_State* L) {    // [L] | to | [L] | fr
    lua_settop(L, 4);
    L_layer_opt_mode(L);                // L | to | [L] | fr
    L_layer_opt(L, 3);                  // L | to | L | fr

    const char* L_to = lua_tostring(L, 1);
    const char* L_fr = lua_tostring(L, 3);
    Pico_Rel_Dim fr = C_rel_dim(L, 4);

    char m = C_mode_s_opt(L, 1, 2);
    Pico_Rel_Dim to = {
        .mode = (m != '\0') ? m : C_mode_t(L, 2, 1)
    };
    pico_cv_dim(L_to, &to, L_fr, &fr);

    if (m != '\0') {
        L_push_rel_dim(L, &to);
    } else {
        lua_pushnumber(L, to.w);
        lua_setfield(L, 2, "w");
        lua_pushnumber(L, to.h);
        lua_setfield(L, 2, "h");
        lua_pushvalue(L, 2);
    }
    return 1;
}

static int l_cv_pos (lua_State* L) {    // [L] | to | [L] | fr
    lua_settop(L, 4);
    L_layer_opt_mode(L);                // L | to | [L] | fr
    L_layer_opt(L, 3);                  // L | to | L | fr

    const char* L_to = lua_tostring(L, 1);
    const char* L_fr = lua_tostring(L, 3);
    Pico_Rel_Pos fr = C_rel_pos(L, 4);

    char m = C_mode_s_opt(L, 1, 2);
    Pico_Rel_Pos to;
    if (m != '\0') {
        to = (Pico_Rel_Pos) {
            .mode   = m,
            .anchor = PICO_ANCHOR_C,
        };
    } else {
        to = (Pico_Rel_Pos) {
            .mode   = C_mode_t(L, 2, 1),
            .anchor = C_anchor(L, 2),
        };
    }
    pico_cv_pos(L_to, &to, L_fr, &fr);

    if (m != '\0') {
        L_push_rel_pos(L, &to);
    } else {
        lua_pushnumber(L, to.x);
        lua_setfield(L, 2, "x");
        lua_pushnumber(L, to.y);
        lua_setfield(L, 2, "y");
        lua_pushvalue(L, 2);
    }
    return 1;
}

static int l_cv_rect (lua_State* L) {   // [L] | to | [L] | fr
    lua_settop(L, 4);
    L_layer_opt_mode(L);                // L | to | [L] | fr
    L_layer_opt(L, 3);                  // L | to | L | fr

    const char* L_to = lua_tostring(L, 1);
    const char* L_fr = lua_tostring(L, 3);
    Pico_Rel_Rect fr = C_rel_rect(L, 4);

    char m = C_mode_s_opt(L, 1, 2);
    Pico_Rel_Rect to;
    if (m != '\0') {
        to = (Pico_Rel_Rect) {
            .mode   = m,
            .anchor = PICO_ANCHOR_C,
        };
    } else {
        to = (Pico_Rel_Rect) {
            .mode   = C_mode_t(L, 2, 1),
            .anchor = C_anchor(L, 2),
        };
    }
    pico_cv_rect(L_to, &to, L_fr, &fr);

    if (m != '\0') {
        L_push_rel_rect(L, &to);
    } else {
        lua_pushnumber(L, to.x);
        lua_setfield(L, 2, "x");
        lua_pushnumber(L, to.y);
        lua_setfield(L, 2, "y");
        lua_pushnumber(L, to.w);
        lua_setfield(L, 2, "w");
        lua_pushnumber(L, to.h);
        lua_setfield(L, 2, "h");
        lua_pushvalue(L, 2);
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_in_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // out | in
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Rel_Rect out = C_rel_rect(L, 1);
    Pico_Rel_Rect in  = C_rel_rect(L, 2);
    Pico_Rel_Rect ret = pico_in_rect(out, in);
    L_push_rel_rect(L, &ret);
    return 1;
}

static int l_in_pos (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // out | in
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Rel_Rect out = C_rel_rect(L, 1);
    Pico_Rel_Pos  in  = C_rel_pos(L, 2);
    Pico_Rel_Pos ret = pico_in_pos(out, in);
    L_push_rel_pos(L, &ret);
    return 1;
}

static int l_in_dim (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // out | in
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Rel_Rect out = C_rel_rect(L, 1);
    Pico_Rel_Dim  in  = C_rel_dim(L, 2);
    Pico_Rel_Dim ret = pico_in_dim(out, in);
    L_push_rel_dim(L, &ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_vs_pos_pos (lua_State* L) {    // [L1] | p1 | [L2] | p2
    lua_settop(L, 4);
    L_layer_opt(L, 1);                      // L1 | p1 | [L2] | p2
    L_layer_opt(L, 3);                      // L1 | p1 | L2 | p2

    const char* L1 = lua_tostring(L, 1);
    const char* L2 = lua_tostring(L, 3);

    Pico_Rel_Pos p1 = C_rel_pos(L, 2);
    Pico_Rel_Pos p2 = C_rel_pos(L, 4);

    int ret = pico_vs_pos_pos(L1, &p1, L2, &p2);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_vs_pos_rect (lua_State* L) {   // [L1] | p1 | [L2] | [r2]
    lua_settop(L, 4);
    L_layer_opt(L, 1);                      // L1 | p1 | [L2] | [r2]
    L_layer_opt(L, 3);                      // L1 | p1 | L2 | [r2]

    const char* L1 = lua_tostring(L, 1);
    const char* L2 = lua_tostring(L, 3);

    Pico_Rel_Pos p1 = C_rel_pos(L, 2);

    Pico_Rel_Rect r2, *xr2=NULL;
    if (!lua_isnil(L,4)) {
        r2 = C_rel_rect(L, 4);
        xr2 = &r2;
    }

    int ret = pico_vs_pos_rect(L1, &p1, L2, xr2);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_vs_rect_pos (lua_State* L) {   // [L1] | [r1] | [L2] | p2
    lua_settop(L, 4);
    L_layer_opt(L, 1);                      // L1 | [r1] | [L2] | p2
    if (lua_type(L,2) == LUA_TSTRING) {     // L1 | L2 | p2
        lua_pushnil(L);
        lua_insert(L, 2);                   // L1 | nil | L2 | p2
    } else {
        L_layer_opt(L, 3);                  // L1 | nil | [L2] | p2
    }

    const char* L1 = lua_tostring(L, 1);
    const char* L2 = lua_tostring(L, 3);

    Pico_Rel_Rect r1;
    Pico_Rel_Rect* xr1 = NULL;
    if (!lua_isnil(L, 2)) {
        r1 = C_rel_rect(L, 2);
        xr1 = &r1;
    }

    Pico_Rel_Pos p2 = C_rel_pos(L, 4);

    int ret = pico_vs_rect_pos(L1, xr1, L2, &p2);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_vs_rect_rect (lua_State* L) {  // [L1] | [r1] | [L2] | [r2]
    lua_settop(L, 4);
    L_layer_opt(L, 1);                      // L1 | [r1] | [L2] | [r2]
    if (lua_type(L,2) == LUA_TSTRING) {     // L1 | L2 | [r2]
        lua_pushnil(L);
        lua_insert(L, 2);                   // L1 | nil | L2 | [r2]
    } else {
        L_layer_opt(L, 3);                  // L1 | nil | [L2] | [r2]
    }

    const char* L1 = lua_tostring(L, 1);
    const char* L2 = lua_tostring(L, 3);

    Pico_Rel_Rect r1;
    Pico_Rel_Rect* xr1 = NULL;
    if (!lua_isnil(L, 2)) {
        r1 = C_rel_rect(L, 2);
        xr1 = &r1;
    }

    Pico_Rel_Rect r2;
    Pico_Rel_Rect* xr2 = NULL;
    if (!lua_isnil(L, 4)) {
        r2 = C_rel_rect(L, 4);
        xr2 = &r2;
    }

    int ret = pico_vs_rect_rect(L1, xr1, L2, xr2);
    lua_pushboolean(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_color_darker (lua_State* L) {
    Pico_Color clr = C_color_tis(L, 1);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_darker(clr, pct);
    L_push_color(L, ret);
    return 1;
}

static int l_color_lighter (lua_State* L) {
    Pico_Color clr = C_color_tis(L, 1);
    float pct = luaL_checknumber(L, 2);
    Pico_Color ret = pico_color_lighter(clr, pct);
    L_push_color(L, ret);
    return 1;
}

static int l_color_mix (lua_State* L) {
    Pico_Color c1 = C_color_tis(L, 1);
    Pico_Color c2 = C_color_tis(L, 2);
    Pico_Color ret = pico_color_mix(c1, c2);
    L_push_color(L, ret);
    return 1;
}

static int l_color_alpha (lua_State* L) {
    Pico_Color clr = C_color_tis(L, 1);
    int a = luaL_checkinteger(L, 2);
    Pico_Color ret = pico_color_alpha(clr, a);
    L_push_color(L, ret);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_get_pencil (lua_State* L) {
    Pico_Layer_Pencil draw = pico_get_pencil();

    lua_newtable(L);                    // T

    L_push_color(L, draw.color);        // T | color
    lua_setfield(L, -2, "color");       // T

    if (draw.font == NULL) {
        lua_pushnil(L);                 // T | nil
    } else {
        lua_pushstring(L, draw.font);   // T | font
    }
    lua_setfield(L, -2, "font");        // T

    lua_pushinteger(L, draw.style);         // T | s
    L_reg_get(L, "styles", lua_gettop(L));  // T | s | *str*
    lua_remove(L, -2);                      // T | *str*
    lua_setfield(L, -2, "style");           // T

    return 1;
}

static int l_get_image (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);  // path | [dim]

    if (lua_gettop(L) == 1) { // default dim={'!',w=0,h=0}
        lua_newtable(L);                        // path | dim
        lua_pushliteral(L, "!");
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "h");
    }

    L_image_get_dim(L, 2, path);
    return 1;
}

static int l_get_keyboard (lua_State* L) {
    Pico_Keyboard k = pico_get_keyboard();
    lua_newtable(L);                            // t
    L_set_keyboard(L, lua_gettop(L), &k);
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

static int l_get_mouse (lua_State* L) {     // [lay] | (mode|pos)
    L_layer_opt_mode(L);
    const char* layer = lua_tostring(L, 1);
    char m = C_mode_s_opt(L, 1, 2);
    Pico_Rel_Pos pos;
    if (m != '\0') {
        pos = (Pico_Rel_Pos) { .mode=m, .anchor=PICO_ANCHOR_C };
    } else {
        pos = (Pico_Rel_Pos) {
            .mode   = C_mode_t(L, 2, 1),
            .anchor = C_anchor(L, 2),
        };
    }
    Pico_Mouse mouse = pico_get_mouse(layer, &pos);
    lua_newtable(L);                            // ... | mouse
    L_set_mouse(L, lua_gettop(L), &mouse);
    return 1;                                   // ... | *mouse*
}

static int l_get_now (lua_State* L) {
    Uint32 ms = pico_get_now();
    lua_pushinteger(L, ms);         // ms
    return 1;                       // [ms]
}

static int l_get_effect (lua_State* L) {
    Pico_Layer_Effect show = pico_get_effect();

    lua_newtable(L);                    // T

    lua_pushinteger(L, show.alpha);     // T | alpha
    lua_setfield(L, -2, "alpha");       // T

    L_push_color(L, show.color);        // T | color
    lua_setfield(L, -2, "color");       // T

    lua_pushinteger(L, show.flip);          // T | f
    L_reg_get(L, "flips", lua_gettop(L));   // T | f | *str*
    lua_remove(L, -2);                      // T | *str*
    lua_setfield(L, -2, "flip");            // T

    lua_pushboolean(L, show.grid);      // T | grid
    lua_setfield(L, -2, "grid");        // T

    lua_newtable(L);                    // T | rot
    lua_pushinteger(L, show.rotate.angle);
    lua_setfield(L, -2, "angle");
    lua_newtable(L);                    // T | rot | anc
    lua_pushnumber(L, show.rotate.anchor.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, show.rotate.anchor.y);
    lua_setfield(L, -2, "y");
    lua_setfield(L, -2, "anchor");      // T | rot
    lua_setfield(L, -2, "rotate");      // T

    return 1;
}

static int l_get_text (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);  // text | dim
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rel_Dim dim = C_rel_dim(L, 2);
    Pico_Abs_Dim abs = pico_get_text(text, &dim);

    lua_pushnumber(L, dim.w);
    lua_setfield(L, 2, "w");
    lua_pushnumber(L, dim.h);
    lua_setfield(L, 2, "h");

    lua_newtable(L);                                // text | dim | abs
    lua_pushnumber(L, abs.w);
    lua_setfield(L, -2, "w");
    lua_pushnumber(L, abs.h);
    lua_setfield(L, -2, "h");                       // text | dim | *abs*
    return 1;
}

static int l_get_video (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);  // path | [rect]

    Pico_Rel_Rect rect, *xrect=NULL;
    if (lua_gettop(L)>=2 && !lua_isnil(L,2)) {
        rect = C_rel_rect(L, 2);
        xrect = &rect;
    }

    Pico_Video vid = pico_get_video(path, xrect);

    lua_newtable(L);                        // T

    lua_newtable(L);                        // T | dim
    lua_pushliteral(L, "!");
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, vid.dim.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, vid.dim.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "dim");             // T

    lua_pushinteger(L, vid.fps);
    lua_setfield(L, -2, "fps");
    lua_pushinteger(L, vid.frame);
    lua_setfield(L, -2, "frame");
    lua_pushboolean(L, vid.done);
    lua_setfield(L, -2, "done");

    return 1;
}

static int l_get_scene (lua_State* L) {
    Pico_Layer_Scene view;
    pico_get_scene(&view);

    lua_newtable(L);                    // T

    lua_newtable(L);                    // T | dim
    lua_pushliteral(L, "!");
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, view.dim.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, view.dim.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "dim");         // T

    lua_newtable(L);                    // T | tile
    lua_pushinteger(L, view.tile.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, view.tile.h);
    lua_setfield(L, -2, "h");
    lua_setfield(L, -2, "tile");        // T

    L_push_rel_rect(L, &view.dst);      // T | dst
    lua_setfield(L, -2, "target");      // T

    L_push_rel_rect(L, &view.src);      // T | src
    lua_setfield(L, -2, "source");      // T

    L_push_rel_rect(L, &view.clip);     // T | clip
    lua_setfield(L, -2, "clip");        // T

    lua_pushboolean(L, view.clear);     // T | clear
    lua_setfield(L, -2, "clear");       // T

    return 1;
}

static int l_get_window (lua_State* L) {
    Pico_Window win = pico_get_window();

    lua_newtable(L);                    // T

    lua_pushboolean(L, win.fs);         // T | fs
    lua_setfield(L, -2, "fullscreen");  // T

    lua_pushboolean(L, win.show);       // T | show
    lua_setfield(L, -2, "show");        // T

    lua_pushstring(L, win.title);       // T | title
    lua_setfield(L, -2, "title");       // T

    return 1;
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_dim (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T
    Pico_Rel_Dim xdim = C_rel_dim(L, 1);
    pico_set_dim(xdim);
    return 0;
}

static int l_set_pencil (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    Pico_Layer_Pencil draw = pico_get_pencil();

    lua_getfield(L, 1, "color");            // T | color
    if (!lua_isnil(L, -1)) {
        draw.color = C_color_tis(L, lua_gettop(L));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "font");             // T | font
    if (!lua_isnil(L, -1)) {
        draw.font = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "style");            // T | style
    if (!lua_isnil(L, -1)) {
        const char* s = luaL_checkstring(L, -1);
        int si = lua_gettop(L);
        L_reg_get(L, "styles", si);             // T | style | *val*
        int ok;
        draw.style = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "invalid style \"%s\"", s);
        }
        lua_pop(L, 1);                          // T | style
    }
    lua_pop(L, 1);                          // T

    pico_set_pencil(draw);
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);     // ok | [fps]
    int on = lua_toboolean(L, 1);
    int fps = 0;
    if (lua_gettop(L) == 1) {               // ok
        fps = 0;
    } else if (lua_isboolean(L, 2)) {       // ok | bool
        fps = lua_toboolean(L,2) ? -1 : 0;
    } else {                                // ok | N
        fps = luaL_checkinteger(L, 2);
    }
    int ms = pico_set_expert(on, fps);
    lua_pushinteger(L, ms);
    return 1;
}

static int l_set_layer (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const char* old = pico_set_layer(name);
    lua_pushstring(L, old);
    return 1;
}

static int l_set_mouse (lua_State* L) {
    const char* layer = NULL;
    int pos_idx = 1;
    if (lua_isstring(L, 1)) {
        layer = lua_tostring(L, 1);
        pos_idx = 2;
    } else if (lua_isnil(L, 1)) {
        pos_idx = 2;
    }
    luaL_checktype(L, pos_idx, LUA_TTABLE);    // pos
    Pico_Rel_Pos pos = C_rel_pos(L, pos_idx);
    pico_set_mouse(layer, pos);
    return 0;
}

static int l_set_effect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    lua_getfield(L, 1, "alpha");            // T | alpha
    if (!lua_isnil(L, -1)) {
        pico_set_effect_alpha(
            (unsigned char) luaL_checkinteger(L, -1));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "color");            // T | color
    if (!lua_isnil(L, -1)) {
        Pico_Color c = C_color_tis(L, lua_gettop(L));
        pico_set_effect_color(c);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "flip");             // T | flip
    if (!lua_isnil(L, -1)) {
        const char* s = luaL_checkstring(L, -1);
        int fi = lua_gettop(L);
        L_reg_get(L, "flips", fi);              // T | flip | *val*
        int ok;
        PICO_FLIP flip = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "invalid flip \"%s\"", s);
        }
        lua_pop(L, 1);                          // T | flip
        pico_set_effect_flip(flip);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "grid");             // T | grid
    if (!lua_isnil(L, -1)) {
        pico_set_effect_grid(lua_toboolean(L, -1));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "rotate");           // T | rot
    if (!lua_isnil(L, -1)) {
        Pico_Rot rot;
        rot.angle  = C_asrfieldnum(L, lua_gettop(L), "angle");
        rot.anchor = C_anchor(L, lua_gettop(L));
        pico_set_effect_rotate(rot);
    }
    lua_pop(L, 1);                          // T

    return 0;
}

static int l_set_video (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);  // name | frame
    int frame = luaL_checkinteger(L, 2);
    int ok = pico_set_video(name, frame);
    lua_pushboolean(L, ok);
    return 1;
}

static int l_set_scene (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    Pico_Rel_Dim  dim,  *xdim=NULL;
    Pico_Abs_Dim  tile, *xtile=NULL;
    Pico_Rel_Rect dst,  *xdst=NULL;
    Pico_Rel_Rect src,  *xsrc=NULL;
    Pico_Rel_Rect clip, *xclip=NULL;

    lua_getfield(L, 1, "clear");            // T | clear
    if (!lua_isnil(L, -1)) {
        pico_set_scene_clear(lua_toboolean(L, -1));
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "dim");              // T | dim
    if (!lua_isnil(L, -1)) {
        dim = C_rel_dim(L, lua_gettop(L));
        xdim = &dim;
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "tile");             // T | tile
    if (!lua_isnil(L, -1)) {
        tile.w = C_asrfieldnum(L, lua_gettop(L), "w");
        tile.h = C_asrfieldnum(L, lua_gettop(L), "h");
        xtile = &tile;
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "clip");             // T | clip
    if (!lua_isnil(L, -1)) {
        clip = C_rel_rect(L, lua_gettop(L));
        xclip = &clip;
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "target");           // T | dst
    if (!lua_isnil(L, -1)) {
        dst = C_rel_rect(L, lua_gettop(L));
        xdst = &dst;
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "source");           // T | src
    if (!lua_isnil(L, -1)) {
        src = C_rel_rect(L, lua_gettop(L));
        xsrc = &src;
    }
    lua_pop(L, 1);                          // T

    if (xtile != NULL) { pico_set_scene_tile(*xtile); }
    if (xdim  != NULL) { pico_set_scene_dim(*xdim);  }
    if (xclip != NULL) { pico_set_scene_clip(*xclip); }
    if (xdst  != NULL) { pico_set_scene_dst(*xdst);  }
    if (xsrc  != NULL) { pico_set_scene_src(*xsrc);  }
    return 0;
}

static int l_set_window (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // T

    int         xfs    = -1;
    int         xshow  = -1;
    const char* xtitle = NULL;

    lua_getfield(L, 1, "fullscreen");       // T | fs
    if (!lua_isnil(L, -1)) {
        xfs = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "show");             // T | show
    if (!lua_isnil(L, -1)) {
        xshow = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);                          // T

    lua_getfield(L, 1, "title");            // T | title
    if (!lua_isnil(L, -1)) {
        xtitle = lua_tostring(L, -1);
    }
    lua_pop(L, 1);                          // T

    if (xfs    != -1)   { pico_set_window_fs   (xfs);     }
    if (xshow  != -1)   { pico_set_window_show (xshow);   }
    if (xtitle != NULL) { pico_set_window_title(xtitle);  }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

// If slot `i` holds a Rect (table with `x` field), set layer `key`'s
// scene.target to it. No-op otherwise. Preserves the current layer.
static void L_opt_target (lua_State* L, int i, const char* key) {
    if (!lua_istable(L, i)) {       // . | t?
        return;
    }

    lua_getfield(L, i, "x");        // . | t | t.x
    int no = lua_isnil(L, -1);
    lua_pop(L, 1);                  // . | t
    if (no) {
        return;
    }

    Pico_Rel_Rect tgt = C_rel_rect(L, i);
    const char* old = pico_set_layer(key);
    pico_set_scene_dst(tgt);
    pico_set_layer(old);
}

// True if table at idx i has a non-nil `x` field (Rect-shaped).
static int L_is_rect (lua_State* L, int i) {
    lua_getfield(L, i, "x");
    int yes = !lua_isnil(L, -1);
    lua_pop(L, 1);
    return yes;
}

static int l_layer_empty (lua_State* L) {
    int m = C_realm_opt(L);  // [m] | up | key | clear | (Dim|Rect) | (Tile|Rect)? | (Tile|Rect)?
    int i = m ? 2 : 1;
    if (!m) m = '!';
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key = luaL_checkstring(L, i+1);

    luaL_checktype(L, i+2, LUA_TBOOLEAN);
    int clear = lua_toboolean(L, i+2);

    // arg 4: Dim or Rect (Rect also sets scene.target via its full shape).
    luaL_checktype(L, i+3, LUA_TTABLE);
    Pico_Rel_Dim dim = C_rel_dim(L, i+3);
    int target_idx = L_is_rect(L, i+3) ? i+3 : 0;

    // args 5, 6: each Tile (no x) or Rect (scene.target). Order-free.
    Pico_Abs_Dim tile;
    Pico_Abs_Dim* tile_ptr = NULL;
    for (int j = i+4; j <= i+5; j++) {
        if (lua_isnoneornil(L, j)) {
            continue;
        }
        luaL_checktype(L, j, LUA_TTABLE);
        if (L_is_rect(L, j)) {
            if (target_idx != 0) {
                return luaL_error(L, "layer.empty: target specified twice");
            }
            target_idx = j;
        } else {
            if (tile_ptr != NULL) {
                return luaL_error(L, "layer.empty: tile specified twice");
            }
            tile.w = (int) C_asrfieldnum(L, j, "w");
            tile.h = (int) C_asrfieldnum(L, j, "h");
            tile_ptr = &tile;
        }
    }

    pico_layer_empty_mode(m, up, key, clear, dim, tile_ptr);
    if (target_idx != 0) {
        L_opt_target(L, target_idx, key);
    }
    return 0;
}

static int l_layer_image (lua_State* L) {
    int m = C_realm_opt(L);
    int i = m ? 2 : 1;
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key;
    const char* path;
    if (lua_isstring(L, i+2)) {
        key  = luaL_checkstring(L, i+1);
        path = luaL_checkstring(L, i+2);
        if (!m) m = '!';
    } else {
        key  = NULL;
        path = luaL_checkstring(L, i+1);
        if (!m) m = '=';
    }
    pico_layer_image_mode(m, up, key, path);
    L_opt_target(L, i+3, key);
    return 0;
}

static int l_layer_pixmap (lua_State* L) {
    int m = C_realm_opt(L);
    int i = m ? 2 : 1;
    if (!m) m = '!';
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key = luaL_checkstring(L, i+1);
    luaL_checktype(L, i+2, LUA_TTABLE);

    Pico_Abs_Dim dim = C_pixmap_dim(L, i+2);
    Pico_Color buf[dim.h][dim.w];
    C_pixmap_fill(L, i+2, dim, (Pico_Color*)buf);

    pico_layer_pixmap_mode(m, up, key, dim, (Pico_Color*)buf);
    L_opt_target(L, i+3, key);
    return 0;
}

static int l_layer_text (lua_State* L) {
    int m = C_realm_opt(L);
    int i = m ? 2 : 1;
    if (!m) m = '!';
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key = luaL_checkstring(L, i+1);
    int height = luaL_checkinteger(L, i+2);
    const char* text = luaL_checkstring(L, i+3);
    pico_layer_text_mode(m, up, key, height, text);
    L_opt_target(L, i+4, key);
    return 0;
}

static int l_layer_video (lua_State* L) {
    int m = C_realm_opt(L);
    int i = m ? 2 : 1;
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key;
    const char* path;
    if (lua_isstring(L, i+2)) {
        key  = luaL_checkstring(L, i+1);
        path = luaL_checkstring(L, i+2);
        if (!m) m = '!';
    } else {
        key  = NULL;
        path = luaL_checkstring(L, i+1);
        if (!m) m = '=';
    }
    pico_layer_video_mode(m, up, key, path);
    L_opt_target(L, i+3, key);
    return 0;
}

static int l_layer_sub (lua_State* L) {
    int m = C_realm_opt(L);
    int i = m ? 2 : 1;
    if (!m) m = '!';
    const char* up = lua_isnil(L, i) ? NULL : luaL_checkstring(L, i);
    const char* key    = luaL_checkstring(L, i+1);
    const char* parent = luaL_checkstring(L, i+2);
    luaL_checktype(L, i+3, LUA_TTABLE);
    Pico_Rel_Rect crop = C_rel_rect(L, i+3);
    pico_layer_sub_mode(m, up, key, parent, &crop);
    L_opt_target(L, i+4, key);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checknumber(L, 1);       // ms
    int dt = pico_input_delay(ms);
    lua_pushinteger(L, dt);                // ms | dt
    return 1;
}

static int l_input_event (lua_State* L) {
    // pico.input.event()
    // pico.input.event(1000)
    // pico.input.event('key.dn')
    // pico.input.event('key.dn', 1000)

    int ms = -1;
    if (lua_isinteger(L,-1)) {
        ms = lua_tointeger(L, -1);
    }

    int id = PICO_EVENT_ANY;
    if (lua_type(L,1) == LUA_TSTRING) {
        L_reg_get(L, "events", 1);              // "e" | *e*
        int ok;
        id = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "invalid event \"%s\"", lua_tostring(L,1));
        }
        lua_pop(L, 1);                          // "e"
    }

    Pico_Event e;
    int dt;
    if (ms == -1) {
        dt = pico_input_event(&e, id);
    } else {
        dt = pico_input_event_timeout(&e, id, ms);
    }

    if (e.type == PICO_EVENT_NONE) {
        lua_pushnil(L);                             // nil
        lua_pushinteger(L, dt);                     // nil | dt
        return 2;
    }

    lua_newtable(L);                                // t
    int T = lua_gettop(L);

    switch (e.type)
    {
        case PICO_EVENT_QUIT:
            lua_pushliteral(L, "quit");             // t | tag
            lua_setfield(L, T, "tag");              // t
            break;

        case PICO_EVENT_MOUSE_MOTION:
            lua_pushliteral(L, "mouse.motion");     // t | tag
            lua_setfield(L, T, "tag");              // t
            L_set_mouse(L, T, &e.mouse);            // t
            break;
        case PICO_EVENT_MOUSE_BUTTON_DN:
            lua_pushliteral(L, "mouse.button.dn");  // t | tag
            lua_setfield(L, T, "tag");              // t
            L_set_mouse(L, T, &e.mouse);            // t
            break;
        case PICO_EVENT_MOUSE_BUTTON_UP:
            lua_pushliteral(L, "mouse.button.up");  // t | tag
            lua_setfield(L, T, "tag");              // t
            L_set_mouse(L, T, &e.mouse);            // t
            break;

        case PICO_EVENT_KEY_DN:
            lua_pushliteral(L, "key.dn");           // t | tag
            lua_setfield(L, T, "tag");              // t
            L_set_keyboard(L, T, &e.keyboard);      // t
            break;
        case PICO_EVENT_KEY_UP:
            lua_pushliteral(L, "key.up");           // t | tag
            lua_setfield(L, T, "tag");              // t
            L_set_keyboard(L, T, &e.keyboard);      // t
            break;

        case PICO_EVENT_WINDOW_RESIZE:
            lua_pushliteral(L, "window.resize");    // t | tag
            lua_setfield(L, T, "tag");              // t
            lua_pushinteger(L, e.window.w);         // t | w
            lua_setfield(L, T, "w");                // t
            lua_pushinteger(L, e.window.h);         // t | h
            lua_setfield(L, T, "h");                // t
            break;

        default:
            assert(0 && "unhandled event type");
    }

    lua_pushinteger(L, dt);                         // t | dt
    return 2;                                       // *t | dt*
}

static int l_input_loop (lua_State* L) {
    pico_input_loop();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_output_clear (lua_State* L) {
    pico_output_clear();
    return 0;
}

static int l_output_draw_pixmap (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);  // name | buf | rect
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Abs_Dim dim = C_pixmap_dim(L, 2);
    Pico_Color buf[dim.h][dim.w];
    C_pixmap_fill(L, 2, dim, (Pico_Color*)buf);

    Pico_Rel_Rect rect = C_rel_rect(L, 3);
    pico_output_draw_pixmap(name, dim, (Pico_Color*)buf, rect);
    return 0;
}

static int l_output_draw_image (lua_State* L) {
    luaL_checktype(L, 1, LUA_TSTRING);      // path | rect
    luaL_checktype(L, 2, LUA_TTABLE);

    const char* path = lua_tostring(L, 1);
    L_image_get_dim(L, 2, path);

    Pico_Rel_Rect rect = C_rel_rect(L, 2);
    pico_output_draw_image(path, rect);
    return 0;
}

static int l_output_draw_layer (lua_State* L) {
    const char* name = luaL_checkstring(L, 1);  // name | [rect]

    Pico_Rel_Rect rect, *xrect=NULL;
    if (lua_istable(L, 2)) {
        rect = C_rel_rect(L, 2);
        xrect = &rect;
    }

    pico_output_draw_layer(name, xrect);
    return 0;
}

static int l_output_draw_layers (lua_State* L) {
    pico_output_draw_layers();
    return 0;
}

static int l_output_draw_line (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1 | p2
    luaL_checktype(L, 2, LUA_TTABLE);

    Pico_Rel_Pos p1 = C_rel_pos(L, 1);
    Pico_Rel_Pos p2 = C_rel_pos(L, 2);

    pico_output_draw_line(p1, p2);
    return 0;
}

static int l_output_draw_oval (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Rect rect = C_rel_rect(L, 1);
    pico_output_draw_oval(rect);
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Pos pos = C_rel_pos(L, 1);
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
        Pico_Rel_Pos p = C_rel_pos(L, lua_gettop(L));
        pxs[i-1] = p;
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
        Pico_Rel_Pos p = C_rel_pos(L, k);
        poly[i-1] = p;
        lua_pop(L, 1);                      // pts | n
    }
    pico_output_draw_poly(n, poly);
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    Pico_Rel_Rect rect = C_rel_rect(L, 1);
    pico_output_draw_rect(rect);
    return 0;
}

static int l_output_draw_text (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);  // text | rect
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Rel_Rect rect = C_rel_rect(L, 2);
    pico_output_draw_text(text, rect);
    return 0;
}

static int l_output_draw_tri (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1 | p2 | p3
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    Pico_Rel_Pos p1 = C_rel_pos(L, 1);
    Pico_Rel_Pos p2 = C_rel_pos(L, 2);
    Pico_Rel_Pos p3 = C_rel_pos(L, 3);

    pico_output_draw_tri(p1, p2, p3);
    return 0;
}

static int l_output_draw_video (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);  // path | rect
    luaL_checktype(L, 2, LUA_TTABLE);
    Pico_Rel_Rect rect = C_rel_rect(L, 2);
    int ok = pico_output_draw_video(path, rect);
    lua_pushboolean(L, ok);
    return 1;
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

    Pico_Rel_Rect rect, *xrect=NULL;
    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
        rect = C_rel_rect(L, 2);
        xrect = &rect;
    }

    const char* ret = pico_output_screenshot(path, xrect);
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
    { "dim",  l_cv_dim  },
    { NULL, NULL }
};

static const luaL_Reg ll_in[] = {
    { "dim",  l_in_dim  },
    { "pos",  l_in_pos  },
    { "rect", l_in_rect },
    { NULL, NULL }
};

static const luaL_Reg ll_vs_pos[] = {
    { "pos",  l_vs_pos_pos  },
    { "rect", l_vs_pos_rect },
    { NULL, NULL }
};

static const luaL_Reg ll_vs_rect[] = {
    { "pos",  l_vs_rect_pos  },
    { "rect", l_vs_rect_rect },
    { NULL, NULL }
};

static const luaL_Reg ll_color[] = {
    { "alpha",   l_color_alpha   },
    { "darker",  l_color_darker  },
    { "lighter", l_color_lighter },
    { "mix",     l_color_mix     },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "effect",   l_get_effect   },
    { "image",    l_get_image    },
    { "keyboard", l_get_keyboard },
    { "layer",    l_get_layer    },
    { "mouse",    l_get_mouse    },
    { "now",      l_get_now      },
    { "pencil",   l_get_pencil   },
    { "scene",    l_get_scene    },
    { "text",     l_get_text     },
    { "video",    l_get_video    },
    { "window",   l_get_window   },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "dim",    l_set_dim    },
    { "effect", l_set_effect },
    { "expert", l_set_expert },
    { "layer",  l_set_layer  },
    { "mouse",  l_set_mouse  },
    { "pencil", l_set_pencil },
    { "scene",  l_set_scene  },
    { "video",  l_set_video  },
    { "window", l_set_window },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_layer[] = {
    { "empty",  l_layer_empty  },
    { "image",  l_layer_image  },
    { "pixmap", l_layer_pixmap },
    { "sub",    l_layer_sub    },
    { "text",   l_layer_text   },
    { "video",  l_layer_video  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_input[] = {
    { "delay", l_input_delay },
    { "event", l_input_event },
    { "loop",  l_input_loop  },
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
    { "image",  l_output_draw_image  },
    { "layer",  l_output_draw_layer  },
    { "layers", l_output_draw_layers },
    { "line",   l_output_draw_line   },
    { "oval",   l_output_draw_oval   },
    { "pixel",  l_output_draw_pixel  },
    { "pixels", l_output_draw_pixels },
    { "pixmap", l_output_draw_pixmap },
    { "poly",   l_output_draw_poly   },
    { "rect",   l_output_draw_rect   },
    { "text",   l_output_draw_text   },
    { "tri",    l_output_draw_tri    },
    { "video",  l_output_draw_video  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico_native (lua_State* L) {
    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_cv);                  // pico | cv
    lua_setfield(L, -2, "cv");              // pico

    luaL_newlib(L, ll_in);                  // pico | xin
    lua_setfield(L, -2, "xin");             // pico

    lua_newtable(L);                        // pico | vs
    luaL_newlib(L, ll_vs_pos);              // pico | vs | pos
    lua_setfield(L, -2, "pos");             // pico | vs
    luaL_newlib(L, ll_vs_rect);             // pico | vs | rect
    lua_setfield(L, -2, "rect");            // pico | vs
    lua_setfield(L, -2, "vs");              // pico

    luaL_newlib(L, ll_color);               // pico | color
    lua_setfield(L, -2, "color");           // pico

    luaL_newlib(L, ll_get);                 // pico | get
    lua_setfield(L, -2, "get");             // pico

    luaL_newlib(L, ll_set);                 // pico | set
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
    {                                                       // pico
        lua_pushlightuserdata(L, (void*)&KEY);              // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);                 // . | G
        lua_newtable(L);                                    // . | G | events
        lua_pushinteger(L, PICO_EVENT_QUIT);                // . | G | events | QT
        lua_setfield(L, -2, "quit");                        // . | G | events
        lua_pushinteger(L, PICO_EVENT_WINDOW_RESIZE);          // . | G | events | WN
        lua_setfield(L, -2, "window.resize");                  // . | G | events
        lua_pushinteger(L, PICO_EVENT_KEY_DN);              // . | G | events | DN
        lua_setfield(L, -2, "key.dn");                      // . | G | events
        lua_pushinteger(L, PICO_EVENT_KEY_UP);              // . | G | events | UP
        lua_setfield(L, -2, "key.up");                      // . | G | events
        lua_pushinteger(L, PICO_EVENT_MOUSE_MOTION);        // . | G | events | MO
        lua_setfield(L, -2, "mouse.motion");                // . | G | events
        lua_pushinteger(L, PICO_EVENT_MOUSE_BUTTON_DN);     // . | G | events | DN
        lua_setfield(L, -2, "mouse.button.dn");             // . | G | events
        lua_pushinteger(L, PICO_EVENT_MOUSE_BUTTON_UP);     // . | G | events | UP
        lua_setfield(L, -2, "mouse.button.up");             // . | G | events
        lua_setfield(L, -2, "events");                      // . | G
        lua_pop(L, 1);                                      // pico


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
        lua_pushliteral(L, "fill");             // . | G | styles | "fill"
        lua_rawseti(L, -2, PICO_STYLE_FILL);    // . | G | styles
        lua_pushliteral(L, "stroke");           // . | G | styles | "stroke"
        lua_rawseti(L, -2, PICO_STYLE_STROKE);  // . | G | styles
        lua_setfield(L, -2, "styles");          // . | G
        lua_pop(L, 1);                          // .
    }

    // flip
    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | flip
        lua_pushinteger(L, PICO_FLIP_NONE);
        lua_setfield(L, -2, "none");
        lua_pushinteger(L, PICO_FLIP_HORIZONTAL);
        lua_setfield(L, -2, "horizontal");
        lua_pushinteger(L, PICO_FLIP_VERTICAL);
        lua_setfield(L, -2, "vertical");
        lua_pushinteger(L, PICO_FLIP_BOTH);
        lua_setfield(L, -2, "both");
        lua_pushliteral(L, "none");
        lua_rawseti(L, -2, PICO_FLIP_NONE);
        lua_pushliteral(L, "horizontal");
        lua_rawseti(L, -2, PICO_FLIP_HORIZONTAL);
        lua_pushliteral(L, "vertical");
        lua_rawseti(L, -2, PICO_FLIP_VERTICAL);
        lua_pushliteral(L, "both");
        lua_rawseti(L, -2, PICO_FLIP_BOTH);
        lua_setfield(L, -2, "flips");           // . | G
        lua_pop(L, 1);                          // .
    }

    return 1;   // [pico]
}

