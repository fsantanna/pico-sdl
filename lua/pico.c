// gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../src/hash.c ../src/dir.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../src/pico.h"

static const char KEY;

static int L_checkfieldint (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    int v = lua_tointegerx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected integer field '%s'", k);
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
    Pico_Pct pct;
    if (lua_type(L,1) == LUA_TTABLE) {  // pct = { x,y }
        pct.x = L_checkfieldint(L, 1, "x");
        pct.y = L_checkfieldint(L, 1, "y");
    } else {
        pct.x = luaL_checkinteger(L, 1);
        pct.y = luaL_checkinteger(L, 2);
    }

    Pico_Pos pos = pico_pos(pct);
    lua_newtable(L);                    // pct | pos
    lua_pushinteger(L, pos.x);          // pct | pos | x
    lua_setfield(L, -2, "x");           // pct | pos
    lua_pushinteger(L, pos.y);          // pct | pos | y
    lua_setfield(L, -2, "y");           // pct | pos

    return 1;                               // pct | [pos]
}

///////////////////////////////////////////////////////////////////////////////

static int l_get_size (lua_State* L) {
    Pico_Size siz = pico_get_size();
    lua_newtable(L);                // siz

    lua_newtable(L);                // siz | phy
    lua_pushinteger(L, siz.phy.x);  // siz | phy | x
    lua_setfield(L, 2, "x");        // siz | phy
    lua_pushinteger(L, siz.phy.y);  // siz | phy | y
    lua_setfield(L, 2, "y");        // siz | phy
    lua_setfield(L, 1, "phy");      // siz

    lua_newtable(L);                // siz | log
    lua_pushinteger(L, siz.log.x);  // siz | log | x
    lua_setfield(L, 2, "x");        // siz | log
    lua_pushinteger(L, siz.log.y);  // siz | log | y
    lua_setfield(L, 2, "y");        // siz | log
    lua_setfield(L, 1, "log");      // siz

    return 1;                       // [siz]
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
    if (lua_isinteger(L,2)) {
        anc.x = lua_tointeger(L, 2);
    } else {
        lua_pushvalue(L, 2);                    // . | ancs | x
        lua_gettable(L, -2);                    // . | ancs | x
        anc.x = lua_tointegerx(L, -1, &ok);
        if (!ok) {
            return luaL_error(L, "invalid anchor \"%s\"", lua_tostring(L,2));
        }
        lua_pop(L, 1);                          // . | ancs
    }

    if (lua_isinteger(L,3)) {
        anc.y = lua_tointeger(L, 3);
    } else {
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

static int l_set_color_draw (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // clr = { r,g,b,a }
    Pico_Color clr = {
        L_checkfieldint(L, 1, "r"),
        L_checkfieldint(L, 1, "g"),
        L_checkfieldint(L, 1, "b"),
        L_checkfieldint(L, 1, "a")
    };
    pico_set_color_draw(clr);
    return 0;
}

static int l_set_cursor (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos = { x,y }
    Pico_Pos pos = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    pico_set_cursor(pos);
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_expert(on);
    return 0;
}

static int l_set_grid (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_set_grid(on);
    return 0;
}

static int l_set_scroll (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos = { x,y }
    Pico_Pos pos = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y"),
    };
    pico_set_scroll(pos);
    return 0;
}

static int l_set_size (lua_State* L) {
    // phy | log
    Pico_Dim phy, log;
    if (lua_isnil(L,1)) {
        phy = PICO_SIZE_KEEP;               // nil | log
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);   // phy | log
        phy = (Pico_Dim) {
            L_checkfieldint(L, 1, "x"),
            L_checkfieldint(L, 1, "y"),
        };
    }
    if (lua_isnil(L,2)) {
        log = PICO_SIZE_KEEP;               // phy | nil
    } else {
        luaL_checktype(L, 2, LUA_TTABLE);   // phy | log
        log = (Pico_Dim) {
            L_checkfieldint(L, 2, "x"),
            L_checkfieldint(L, 2, "y"),
        };
    }
    pico_set_size(phy, log);
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
        case PICO_MOUSEBUTTONDOWN:
            lua_pushinteger(L, e.button.x); // . | t | x
            lua_setfield(L, -2, "x");       // . | t
            lua_pushinteger(L, e.button.y); // . | t | y
            lua_setfield(L, -2, "y");       // . | t
            break;
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

static int l_output_draw_line (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // p1={x,y}
    luaL_checktype(L, 2, LUA_TTABLE);       // p1={x,y} | p2={x,y}
    Pico_Pos p1 = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    Pico_Pos p2 = {
        L_checkfieldint(L, 2, "x"),
        L_checkfieldint(L, 2, "y")
    };
    pico_output_draw_line(p1, p2);
    return 0;
}

static int l_output_draw_oval (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // rect={x,y,w,h}
    Pico_Rect rect = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y"),
        L_checkfieldint(L, 1, "w"),
        L_checkfieldint(L, 1, "h")
    };
    pico_output_draw_oval(rect);
    return 0;
}

static int l_output_draw_pixel (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y}
    Pico_Pos pos = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    pico_output_draw_pixel(pos);
    return 0;
}

static int l_output_draw_poly (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pts={{x,y,w,h}}
    lua_len(L, 1);                          // pts | n
    int n = lua_tointeger(L, -1);
    Pico_Pos poly[n];
    for (int i=1; i<=n; i++) {
        lua_geti(L, 1, i);                  // pts | n | {x,y}
        luaL_checktype(L, -1, LUA_TTABLE);
        poly[i-1] = (Pico_Pos) {
            L_checkfieldint(L, -1, "x"),
            L_checkfieldint(L, -1, "y")
        };
        lua_pop(L, 1);                      // pts | n
    }
    pico_output_draw_poly(poly, n);
    return 0;
}

static int l_output_draw_rect (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // rect={x,y,w,h}
    Pico_Rect rect = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y"),
        L_checkfieldint(L, 1, "w"),
        L_checkfieldint(L, 1, "h")
    };
    pico_output_draw_rect(rect);
    return 0;
}

static int l_output_draw_text (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // pos={x,y}
    luaL_checktype(L, 2, LUA_TSTRING);      // pos={x,y} | text
    Pico_Pos pos = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y")
    };
    pico_output_draw_text(pos, lua_tostring(L,2));
    return 0;
}

static int l_output_draw_tri (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);       // rect={x,y,w,h}
    Pico_Rect rect = {
        L_checkfieldint(L, 1, "x"),
        L_checkfieldint(L, 1, "y"),
        L_checkfieldint(L, 1, "w"),
        L_checkfieldint(L, 1, "h")
    };
    pico_output_draw_tri(rect);
    return 0;
}

static int l_output_present (lua_State* L) {
    pico_output_present();
    return 0;
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
    { "init", l_init },
    { "pos",  l_pos  },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_get[] = {
    { "size", l_get_size },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_set[] = {
    { "cursor", l_set_cursor },
    { "expert", l_set_expert },
    { "grid",   l_set_grid   },
    { "scroll", l_set_scroll },
    { "size",   l_set_size   },
    { "title",  l_set_title  },
    { NULL, NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { "draw",  l_set_color_draw  },
    { NULL, NULL }
};

static const luaL_Reg ll_set_anchor[] = {
    { "draw", l_set_anchor_draw },
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
    { "clear",   l_output_clear   },
    { "present", l_output_present },
    { "sound",   l_output_sound   },
    { "write",   l_output_write   },
    { "writeln", l_output_writeln },
    { NULL, NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "image", l_output_draw_image },
    { "line",  l_output_draw_line  },
    { "oval",  l_output_draw_oval  },
    { "pixel", l_output_draw_pixel },
    { "poly",  l_output_draw_poly  },
    { "rect",  l_output_draw_rect  },
    { "text",  l_output_draw_text  },
    { "tri",   l_output_draw_tri   },
    { NULL, NULL }
};

///////////////////////////////////////////////////////////////////////////////

int luaopen_pico (lua_State* L) {
    lua_pushlightuserdata(L, (void*)&KEY);  // K
    lua_newtable(L);                        // K | G
    lua_settable(L, LUA_REGISTRYINDEX);     // -

    luaL_newlib(L, ll_all);                 // pico

    luaL_newlib(L, ll_get);                 // pico | get
    lua_setfield(L, -2, "get");             // pico

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
        lua_pushinteger(L, PICO_KEYDOWN);       // . | G | evts | DN
        lua_setfield(L, -2, "key.dn");          // . | G | evts
        lua_pushinteger(L, PICO_KEYUP);         // . | G | evts | UP
        lua_setfield(L, -2, "key.up");          // . | G | evts
        lua_pushinteger(L, PICO_MOUSEBUTTONDOWN); // . | G | evts | DN
        lua_setfield(L, -2, "mouse.button.dn"); // . | G | evts
        lua_setfield(L, -2, "evts");            // . | G
        lua_pop(L, 1);                          // .
    }                                           // pico

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

    return 1;                               // [pico]
}
