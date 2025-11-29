// gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../src/hash.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../src/pico.h"

static const char KEY;

static int L_checkfieldnum (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    int v = lua_tonumberx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected numeric field '%s'", k);
    }
    lua_pop(L, 1);                      // T
    return v;
}

static Pico_Anchor _anchor (lua_State* L, int n) {
    Pico_Anchor anc;

    lua_pushlightuserdata(L, (void*)&KEY);  // . | K
    lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
    lua_getfield(L, -1, "ancs");            // . | G | ancs

    if (lua_type(L,n) == LUA_TTABLE) {      // T | G | ancs
        lua_getfield(L, n, "x");            // T | G | ancs | x
        lua_getfield(L, n, "y");            // T | G | ancs | x | y
    } else {                                // x | y | G | ancs
        lua_pushvalue(L, n);                // x | y | G | ancs | x
        lua_pushvalue(L, n+1);              // x | y | G | ancs | x | y
    }

    if (lua_isinteger(L,-2)) {
        anc.x = lua_tointeger(L, -2);
    } else {
        lua_pushvalue(L, -2);               // . | G | ancs | x | y | x
        lua_gettable(L, -4);                // . | G | ancs | x | y | *x*
        int ok;
        anc.x = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,-3));
        }
        lua_pop(L, 1);                      // . | G | ancs | x | y
    }

    if (lua_isinteger(L,-1)) {
        anc.y = lua_tointeger(L, -1);
    } else {
        lua_pushvalue(L, -1);               // . | G | ancs | x | y | y
        lua_gettable(L, -4);                // . | G | ancs | x | y | *y*
        int ok;
        anc.y = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,-2));
        }
        lua_pop(L, 1);                      // . | G | ancs | x | y
    }

    lua_pop(L, 4);                          // .
    return anc;
}

static Pico_Color _color_t (lua_State* L, int i) {
    assert(lua_type(L,i) == LUA_TTABLE);    // clr = { r,g,b[,a] }
    Pico_Color clr = {
        L_checkfieldnum(L, i, "r"),
        L_checkfieldnum(L, i, "g"),
        L_checkfieldnum(L, i, "b"),
    };
    lua_getfield(L, i, "a");                // clr | a
    int ok;
    clr.a = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        clr.a = 0xFF;
    }
    lua_pop(L, 1);                          // clr
    return clr;
}

static Pico_Color _color (lua_State* L) {
    Pico_Color clr;
    if (lua_type(L,1) == LUA_TTABLE) {  // clr = { r,g,b[,a] }
        clr = _color_t(L, 1);
    } else {                            // r | g | b [| a]
        clr = (Pico_Color) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
        };
        if (lua_gettop(L) >= 4) {
            clr.a = luaL_checknumber(L, 4);
        } else {
            clr.a = 0xFF;
        }
    }
    return clr;
}

static void _mouse_button (lua_State* L, int but) {
    lua_pushlightuserdata(L, (void*)&KEY);      // . | K
    lua_gettable(L, LUA_REGISTRYINDEX);         // . | G
    lua_getfield(L, -1, "buttons");             // . | G | buttons
    lua_geti(L, -1, but);                       // . | G | buttons | but
    lua_replace(L, -3);                         // . | but | buttons
    lua_pop(L, 1);                              // . | but
}

///////////////////////////////////////////////////////////////////////////////

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

static int l_dim (lua_State* L) {
    int ext;    // extra outer dim arg
    Pico_Pct pct;
    if (lua_type(L,1) == LUA_TTABLE) {      // pct = { x,y }
        pct.x = L_checkfieldnum(L, 1, "x");
        pct.y = L_checkfieldnum(L, 1, "y");
        ext = (lua_gettop(L) > 1);
    } else {                                // x | y
        pct.x = luaL_checknumber(L, 1);
        pct.y = luaL_checknumber(L, 2);
        ext = (lua_gettop(L) > 2);
    }

    Pico_Dim dim;
    if (ext) {                              // . | {x,y}
        if (lua_type(L,-1) != LUA_TTABLE) {
            return luaL_error(L, "expected outer dimension table");
        }
        Pico_Dim out = {
            L_checkfieldnum(L, -1, "x"),
            L_checkfieldnum(L, -1, "y"),
        };
        dim = pico_dim_ext(pct, out);
    } else {
        dim = pico_dim(pct);
    }

    lua_newtable(L);                        // . | dim
    lua_pushinteger(L, dim.x);              // . | dim | x
    lua_setfield(L, -2, "x");               // . | dim
    lua_pushinteger(L, dim.y);              // . | dim | y
    lua_setfield(L, -2, "y");               // . | dim

    return 1;                               // . | [dim]
}

static int l_pos (lua_State* L) {
    int ext = 0;    // extra outer dim arg
    Pico_Pct pct;
    if (lua_type(L,1) == LUA_TTABLE) {      // pct = { x,y }
        pct.x = L_checkfieldnum(L, 1, "x");
        pct.y = L_checkfieldnum(L, 1, "y");
        ext = (lua_gettop(L) > 1);
    } else {                                // x | y
        pct.x = luaL_checknumber(L, 1);
        pct.y = luaL_checknumber(L, 2);
    }

    Pico_Pos pos;
    if (ext) {                              // . | {x,y,w,h}
        luaL_checktype(L, 2, LUA_TTABLE);
        Pico_Rect out = {
            L_checkfieldnum(L, 2, "x"),
            L_checkfieldnum(L, 2, "y"),
            L_checkfieldnum(L, 2, "w"),
            L_checkfieldnum(L, 2, "h"),
        };
        Pico_Anchor anc;
        if (lua_gettop(L) >= 3) {
            luaL_checktype(L, 3, LUA_TTABLE);
            anc = _anchor(L, 3);
        } else {
            anc = pico_get_anchor_pos();
        }
        pos = pico_pos_ext(pct, out, anc);
    } else {
        pos = pico_pos(pct);
    }

    lua_newtable(L);                        // . | pos
    lua_pushinteger(L, pos.x);              // . | pos | x
    lua_setfield(L, -2, "x");               // . | pos
    lua_pushinteger(L, pos.y);              // . | pos | y
    lua_setfield(L, -2, "y");               // . | pos

    return 1;                               // . | [pos]
}

///////////////////////////////////////////////////////////////////////////////

static int l_vs_pos_rect (lua_State* L) {
    Pico_Pos pos = {                    // pos | rect
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
    };
    Pico_Rect rect = {                  // pos | rect
        L_checkfieldnum(L, 2, "x"),
        L_checkfieldnum(L, 2, "y"),
        L_checkfieldnum(L, 2, "w"),
        L_checkfieldnum(L, 2, "h"),
    };

    int x;
    if (lua_gettop(L) <= 2) {
        x = pico_pos_vs_rect(pos, rect);
    } else {
        Pico_Anchor anc1 = _anchor(L, 3);
        Pico_Anchor anc2 = _anchor(L, 4);
        x = pico_pos_vs_rect_ext(pos, rect, anc1, anc2);
    }

    lua_pushboolean(L, x);              // pos | rect | x
    return 1;                           // pos | rect | [x]
}

static int l_vs_rect_rect (lua_State* L) {
    Pico_Rect r1 = {                    // r1 | r2
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
        L_checkfieldnum(L, 1, "w"),
        L_checkfieldnum(L, 1, "h"),
    };
    Pico_Rect r2 = {                    // r1 | r2
        L_checkfieldnum(L, 2, "x"),
        L_checkfieldnum(L, 2, "y"),
        L_checkfieldnum(L, 2, "w"),
        L_checkfieldnum(L, 2, "h"),
    };

    int x;
    if (lua_gettop(L) <= 2) {
        x = pico_rect_vs_rect(r1, r2);
    } else {
        Pico_Anchor anc1 = _anchor(L, 3);
        Pico_Anchor anc2 = _anchor(L, 4);
        x = pico_rect_vs_rect_ext(r1, r2, anc1, anc2);
    }

    lua_pushboolean(L, x);              // pos | rect | x
    return 1;                           // pos | rect | [x]
}

///////////////////////////////////////////////////////////////////////////////

static void _get_xy (lua_State* L, int x, int y) {
    lua_newtable(L);            // . | t
    lua_pushinteger(L, x);      // . | t | x
    lua_setfield(L, -2, "x");   // . | t
    lua_pushinteger(L, y);      // . | t | y
    lua_setfield(L, -2, "y");   // . | [t]
}

static int l_get_anchor_pos (lua_State* L) {
    Pico_Dim anc = pico_get_anchor_pos();
    _get_xy(L, anc.x, anc.y);   // . | anc
    return 1;                   // . | [anc]
}

static int l_get_anchor_rotate (lua_State* L) {
    Pico_Dim anc = pico_get_anchor_rotate();
    _get_xy(L, anc.x, anc.y);   // . | anc
    return 1;                   // . | [anc]
}

static int l_get_dim_image (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    Pico_Dim dim = pico_get_dim_image(path);
    _get_xy(L, dim.x, dim.y);   // . | dim
    return 1;                   // . | [dim]
}

static int l_get_dim_window (lua_State* L) {
    Pico_Dim dim = pico_get_dim_window();
    _get_xy(L, dim.x, dim.y);   // . | dim
    return 1;                   // . | [dim]
}

static int l_get_dim_world (lua_State* L) {
    Pico_Dim dim = pico_get_dim_world();
    _get_xy(L, dim.x, dim.y);   // . | dim
    return 1;                   // . | [dim]
}

static int l_get_mouse (lua_State* L) {
    if (lua_gettop(L) > 0) {
        return luaL_error(L, "TODO: pico.mouse.get(button)");
    }
    Pico_Pos pos;
    pico_get_mouse(&pos, PICO_MOUSE_BUTTON_NONE);
    _get_xy(L, pos.x, pos.y);   // . | pos
    return 1;                   // . | [pos]
}

static int l_get_rotate (lua_State* L) {
    int ang = pico_get_rotate();
    lua_pushinteger(L, ang);        // ang
    return 1;                       // [ang]
}

static int l_get_ticks (lua_State* L) {
    Uint32 ms = pico_get_ticks();
    lua_pushinteger(L, ms);         // ms
    return 1;                       // [ms]
}

///////////////////////////////////////////////////////////////////////////////

static int l_set_anchor_pos (lua_State* L) {
    Pico_Anchor anc = _anchor(L, 1);
    pico_set_anchor_pos(anc);
    return 0;
}

static int l_set_anchor_rotate (lua_State* L) {
    Pico_Anchor anc = _anchor(L, 1);
    pico_set_anchor_rotate(anc);
    return 0;
}

static int l_set_color_clear (lua_State* L) {
    Pico_Color clr = _color(L);
    pico_set_color_clear(clr);
    return 0;
}

static int l_set_color_draw (lua_State* L) {
    Pico_Color clr = _color(L);
    pico_set_color_draw(clr);
    return 0;
}

static int l_set_crop (lua_State* L) {
    Pico_Rect r;
    if (lua_gettop(L) == 0) {               // -
        r = (Pico_Rect) {0,0,0,0};
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);   // r = {x,y,w,h}
        r = (Pico_Rect) {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y"),
            L_checkfieldnum(L, 1, "w"),
            L_checkfieldnum(L, 1, "h"),
        };
    }
    pico_set_crop(r);
    return 0;
}

static int l_set_cursor (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos = { x,y }
    Pico_Pos pos = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
    };
    pico_set_cursor(pos);
    return 0;
}

static int l_set_dim_window (lua_State* L) {
    Pico_Dim dim;

    // w | h
    if (lua_type(L,1) == LUA_TNUMBER) {
        dim = (Pico_Dim) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
        };

    // dim
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);   // dim
        dim = (Pico_Dim) {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y"),
        };
    }

    pico_set_dim_window(dim);
    return 0;
}

static int l_set_dim_world (lua_State* L) {
    Pico_Dim dim;

    // w | h
    if (lua_type(L,1) == LUA_TNUMBER) {
        dim = (Pico_Dim) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
        };

    // dim
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);   // dim
        dim = (Pico_Dim) {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y"),
        };
    }

    pico_set_dim_world(dim);
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_expert(on);
    return 0;
}

static int l_set_font (lua_State* L) {
    int h = luaL_checknumber(L, 2);    // path | h
    char* f = NULL;
    if (!lua_isnil(L, 1)) {
        f = (char*) luaL_checklstring(L, 1, NULL);
    }
    pico_set_font(f, h);
    return 0;
}

static int l_set_grid (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_grid(on);
    return 0;
}

static int l_set_rotate (lua_State* L) {
    int ang = luaL_checknumber(L, 1);      // ang
    pico_set_rotate(ang);
    return 0;
}

static int l_set_scroll (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos = { x,y }
    Pico_Pos pos = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
    };
    pico_set_scroll(pos);
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

static int l_set_show (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_show(on);
    return 0;
}

static int l_set_scale (lua_State* L) {
    Pico_Pct pct;
    if (lua_type(L,1) == LUA_TTABLE) {      // pct = { x,y }
        pct.x = L_checkfieldnum(L, 1, "x");
        pct.y = L_checkfieldnum(L, 1, "y");
    } else {                                // x | y
        pct.x = luaL_checknumber(L, 1);
        pct.y = luaL_checknumber(L, 2);
    }
    pico_set_scale(pct);
    return 0;
}

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);   // title
    pico_set_title(title);
    return 0;
}

static int l_set_zoom (lua_State* L) {
    Pico_Pct pct;
    if (lua_type(L,1) == LUA_TTABLE) {      // pct = { x,y }
        pct.x = L_checkfieldnum(L, 1, "x");
        pct.y = L_checkfieldnum(L, 1, "y");
    } else {                                // x | y
        pct.x = luaL_checknumber(L, 1);
        pct.y = luaL_checknumber(L, 2);
    }
    pico_set_zoom(pct);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checknumber(L, 1);       // ms
    pico_input_delay(ms);
    return 0;
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
        lua_getfield(L, -1, "evts");            // "e" | G | evts
        lua_pushvalue(L, 1);                    // "e" | G | evts | "e"
        lua_gettable(L, -2);                    // "e" | G | evts | e
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
            _mouse_button(L, e.button.button);      // . | t | but
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
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y} | T
    luaL_checktype(L, 2, LUA_TTABLE);       // pos | T

    Pico_Pos pos = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y")
    };

    lua_len(L, 2);                          // pos | T | l
    int l = lua_tointeger(L, -1);
    lua_geti(L, 2, 1);                      // pos | T | l | T[1]
    if (lua_type(L,-1) != LUA_TTABLE) {
        return luaL_error(L, "expected column tables");
    }
    lua_len(L, -1);                         // pos | T | l | T[1] | c
    int c = lua_tointeger(L, -1);
    lua_pop(L, 3);                          // pos | T

    Pico_Color buf[l][c];
    for (int i=1; i<=l; i++) {
        lua_geti(L, 2, i);                  // pos | T | T[i]
        if (lua_type(L,-1) != LUA_TTABLE) {
            return luaL_error(L, "expected table at column %d", i);
        }
        for (int j=1; j<=c; j++) {
            lua_geti(L, -1, j);             // pos | T | T[i] | T[j]
            if (lua_type(L,-1) != LUA_TTABLE) {
                return luaL_error(L, "expected color at position [%d,%d]", i, j);
            }
            SDL_Color clr = _color_t(L, lua_gettop(L));
            buf[i-1][j-1] = clr;
            lua_pop(L, 1);                  // pos | T | T[i]
        }
        lua_pop(L, 1);                      // pos | T
    }
    pico_output_draw_buffer(pos, (Pico_Color*)buf, (Pico_Dim){c,l});
    return 0;
}

static int l_output_draw_image (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y}
    luaL_checktype(L, 2, LUA_TSTRING);      // pos={x,y} | path
    Pico_Pos pos = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y")
    };
    if (lua_gettop(L) < 3) {
        pico_output_draw_image(pos, lua_tostring(L,2));
    } else {
        if (lua_type(L,1) != LUA_TTABLE) {
            return luaL_error(L, "expected dimension table");
        }
        Pico_Dim dim = {
            L_checkfieldnum(L, -1, "x"),
            L_checkfieldnum(L, -1, "y")
        };
        pico_output_draw_image_ext(pos, lua_tostring(L,2), dim);
    }
    return 0;
}

static int l_output_draw_line (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1={x,y}
    luaL_checktype(L, 2, LUA_TTABLE);       // p1={x,y} | p2={x,y}
    Pico_Pos p1 = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y")
    };
    Pico_Pos p2 = {
        L_checkfieldnum(L, 2, "x"),
        L_checkfieldnum(L, 2, "y")
    };
    pico_output_draw_line(p1, p2);
    return 0;
}

static int l_output_draw_oval (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // rect={x,y,w,h}
    Pico_Rect rect = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
        L_checkfieldnum(L, 1, "w"),
        L_checkfieldnum(L, 1, "h")
    };
    pico_output_draw_oval(rect);
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    Pico_Pos pos;
    if (lua_type(L,1) == LUA_TTABLE) {      // pos={x,y}
        pos = (Pico_Pos) {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y")
        };
    } else {                                // x | y
        pos = (Pico_Pos) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2)
        };
    }
    pico_output_draw_pixel(pos);
    return 0;
}

static int l_output_draw_pixels (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pixels={{x,y}}
    lua_len(L, 1);                          // pxs | n
    int n = lua_tointeger(L, -1);
    Pico_Pos pxs[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pxs | n | pxs[i]
        if (lua_type(L,-1) != LUA_TTABLE) {
            return luaL_error(L, "expected position at index %d", i);
        }
        Pico_Pos px = {
            L_checkfieldnum(L, -1, "x"),
            L_checkfieldnum(L, -1, "y")
        };
        pxs[i-1] = px;
        lua_pop(L, 1);                      // pxs | n
    }
    pico_output_draw_pixels(pxs, n);
    return 0;
}

static int l_output_draw_poly (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pts={{x,y,w,h}}
    lua_len(L, 1);                          // pts | n
    int n = lua_tointeger(L, -1);
    Pico_Pos poly[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pts | n | {x,y}
        if (lua_type(L,-1) != LUA_TTABLE) {
            return luaL_error(L, "expected point at index %d", i);
        }
        poly[i-1] = (Pico_Pos) {
            L_checkfieldnum(L, -1, "x"),
            L_checkfieldnum(L, -1, "y")
        };
        lua_pop(L, 1);                      // pts | n
    }
    pico_output_draw_poly(poly, n);
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    Pico_Rect rect;
    if (lua_type(L,1) == LUA_TTABLE) {      // rect={x,y,w,h}
        rect = (Pico_Rect) {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y"),
            L_checkfieldnum(L, 1, "w"),
            L_checkfieldnum(L, 1, "h")
        };
    } else {                                // x | y | w | h
        rect = (Pico_Rect) {
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4)
        };
    }

    pico_output_draw_rect(rect);
    return 0;
}

static int l_output_draw_text (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y}
    lua_tostring(L, 2);
    luaL_checktype(L, 2, LUA_TSTRING);      // pos={x,y} | text
    Pico_Pos pos = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y")
    };
    pico_output_draw_text(pos, lua_tostring(L,2));
    return 0;
}

static int l_output_draw_tri (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // rect={x,y,w,h}
    Pico_Rect rect = {
        L_checkfieldnum(L, 1, "x"),
        L_checkfieldnum(L, 1, "y"),
        L_checkfieldnum(L, 1, "w"),
        L_checkfieldnum(L, 1, "h")
    };
    pico_output_draw_tri(rect);
    return 0;
}

static int l_output_present (lua_State* L) {
    pico_output_present();
    return 0;
}

static int l_output_screenshot (lua_State* L) {
    char* path = NULL;
    if (lua_type(L,1) == LUA_TSTRING) {     // path
        path = (char*) lua_tostring(L, 1);
    }

    char* ret;
    int n = lua_gettop(L);
    if (n>0 && lua_type(L,-1)!=LUA_TSTRING) {    // . | rect
        luaL_checktype(L, n, LUA_TTABLE);
        Pico_Rect rect = {
            L_checkfieldnum(L, 1, "x"),
            L_checkfieldnum(L, 1, "y"),
            L_checkfieldnum(L, 1, "w"),
            L_checkfieldnum(L, 1, "h")
        };
        ret = (char*) pico_output_screenshot_ext(path, rect);
    } else {
        ret = (char*) pico_output_screenshot(path);
    }

    if (ret == NULL) {
        return 0;
    } else {
        lua_pushstring(L, ret);
        return 1;
    }
}

static int l_output_sound (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);   // path
    pico_output_sound(path);
    return 0;
}

static int l_output_write (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);   // text
    pico_output_write(text);
    return 0;
}

static int l_output_writeln (lua_State* L) {
    const char* text = luaL_checkstring(L, 1);   // text
    pico_output_writeln(text);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_all[] = {
    { "dim",  l_dim  },
    { "init", l_init },
    { "pos",  l_pos  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_vs[] = {
    { "pos_rect",  l_vs_pos_rect  },
    { "rect_rect", l_vs_rect_rect },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "mouse",  l_get_mouse  },
    { "rotate", l_get_rotate },
    { "ticks",  l_get_ticks  },
    { NULL, NULL }
};

static const luaL_Reg ll_get_anchor[] = {
    { "pos",    l_get_anchor_pos    },
    { "rotate", l_get_anchor_rotate },
    { NULL, NULL }
};

static const luaL_Reg ll_get_dim[] = {
    { "image",  l_get_dim_image  },
    { "window", l_get_dim_window },
    { "world",  l_get_dim_world  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "crop",   l_set_crop   },
    { "cursor", l_set_cursor },
    { "expert", l_set_expert },
    { "font",   l_set_font   },
    { "grid",   l_set_grid   },
    { "rotate", l_set_rotate },
    { "scroll", l_set_scroll },
    { "show",   l_set_show   },
    { "scale",  l_set_scale  },
    { "style",  l_set_style  },
    { "title",  l_set_title  },
    { "zoom",   l_set_zoom   },
    { NULL, NULL }
};

static const luaL_Reg ll_set_anchor[] = {
    { "pos",    l_set_anchor_pos    },
    { "rotate", l_set_anchor_rotate },
    { NULL, NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { "draw",  l_set_color_draw  },
    { NULL, NULL }
};

static const luaL_Reg ll_set_dim[] = {
    { "window", l_set_dim_window },
    { "world",  l_set_dim_world  },
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
    { "write",      l_output_write      },
    { "writeln",    l_output_writeln    },
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
    lua_pushlightuserdata(L, (void*)&KEY);  // K
    lua_newtable(L);                        // K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // -

    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_vs);                  // pico | vs
    lua_setfield(L, -2, "vs");              // pico

    luaL_newlib(L, ll_get);                 // pico | get
    luaL_newlib(L, ll_get_anchor);          // pico | get | anchor
    lua_setfield(L, -2, "anchor");          // pico | get
    luaL_newlib(L, ll_get_dim);             // pico | get | anchor
    lua_setfield(L, -2, "dim");             // pico | get
    lua_setfield(L, -2, "get");             // pico

    luaL_newlib(L, ll_set);                 // pico | set
    luaL_newlib(L, ll_set_anchor);          // pico | set | anchor
    lua_setfield(L, -2, "anchor");          // pico | set
    luaL_newlib(L, ll_set_color);           // pico | set | color
    lua_setfield(L, -2, "color");           // pico | set
    luaL_newlib(L, ll_set_dim);             // pico | set | dim
    lua_setfield(L, -2, "dim");             // pico | set
    lua_setfield(L, -2, "set");             // pico

    luaL_newlib(L, ll_input);               // pico | input
    lua_setfield(L, -2, "input");           // pico

    luaL_newlib(L, ll_output);              // pico | output
    luaL_newlib(L, ll_output_draw);         // pico | output | draw
    lua_setfield(L, -2, "draw");            // pico | output
    lua_setfield(L, -2, "output");          // pico

    // events
    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | evts
        lua_pushinteger(L, PICO_QUIT);          // . | G | evts | QT
        lua_setfield(L, -2, "quit");            // . | G | evts
        lua_pushinteger(L, PICO_KEYDOWN);       // . | G | evts | DN
        lua_setfield(L, -2, "key.dn");          // . | G | evts
        lua_pushinteger(L, PICO_KEYUP);         // . | G | evts | UP
        lua_setfield(L, -2, "key.up");          // . | G | evts
        lua_pushinteger(L, PICO_MOUSEBUTTONDOWN); // . | G | evts | DN
        lua_setfield(L, -2, "mouse.button.dn"); // . | G | evts
        lua_setfield(L, -2, "evts");            // . | G
        lua_pop(L, 1);                          // .
    }                                           // pico

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
        lua_pop(L, 1);                          // .
    }

    // anchors
    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | ancs
        lua_pushinteger(L, PICO_LEFT);          // . | G | ancs | L
        lua_setfield(L, -2, "left");            // . | G | ancs
        lua_pushinteger(L, PICO_CENTER);        // . | G | ancs | C
        lua_setfield(L, -2, "center");          // . | G | ancs
        lua_pushinteger(L, PICO_RIGHT);         // . | G | ancs | R
        lua_setfield(L, -2, "right");           // . | G | ancs
        lua_pushinteger(L, PICO_TOP);           // . | G | ancs | T
        lua_setfield(L, -2, "top");             // . | G | ancs
        lua_pushinteger(L, PICO_MIDDLE);        // . | G | ancs | M
        lua_setfield(L, -2, "middle");          // . | G | ancs
        lua_pushinteger(L, PICO_BOTTOM);        // . | G | ancs | B
        lua_setfield(L, -2, "bottom");          // . | G | ancs
        lua_setfield(L, -2, "ancs");            // . | G
        lua_pop(L, 1);                          // .
    }                                           // pico

    // styles
    {                                           // pico
        lua_pushlightuserdata(L, (void*)&KEY);  // . | K
        lua_gettable(L, LUA_REGISTRYINDEX);     // . | G
        lua_newtable(L);                        // . | G | styles
        lua_pushinteger(L, PICO_FILL);          // . | G | styles | fill
        lua_setfield(L, -2, "fill");            // . | G | styles
        lua_pushinteger(L, PICO_STROKE);        // . | G | styles | stroke
        lua_setfield(L, -2, "stroke");          // . | G | styles
        lua_setfield(L, -2, "styles");          // . | G
        lua_pop(L, 1);                          // .
    }

    return 1;                               // [pico]
}
