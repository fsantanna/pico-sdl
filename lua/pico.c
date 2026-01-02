// Lua bindings for pico-SDL
// Compile: gcc -shared -o pico.so -fPIC /x/pico-sdl/src/pico.c ../src/hash.c pico.c -llua5.4 -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer -lSDL2_image

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>

#include "../src/pico.h"

static const char KEY;

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

static int L_checkfieldnum (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    double v = lua_tonumberx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected numeric field '%s'", k);
    }
    lua_pop(L, 1);                      // T
    return v;
}

static double L_checkfieldfloat (lua_State* L, int i, const char* k) {
    luaL_checktype(L, i, LUA_TTABLE);   // T
    lua_getfield(L, i, k);              // T | k
    int ok;
    double v = lua_tonumberx(L, -1, &ok);
    if (!ok) {
        return luaL_error(L, "expected numeric field '%s'", k);
    }
    lua_pop(L, 1);                      // T
    return v;
}

static Pico_Color _color (lua_State* L, int idx) {
    Pico_Color clr;
    if (lua_type(L, idx) == LUA_TTABLE) {  // clr = { r,g,b[,a] }
        clr = (Pico_Color) {
            L_checkfieldnum(L, idx, "r"),
            L_checkfieldnum(L, idx, "g"),
            L_checkfieldnum(L, idx, "b"),
        };
    } else {                            // r | g | b [| a]
        clr = (Pico_Color) {
            luaL_checknumber(L, idx),
            luaL_checknumber(L, idx+1),
            luaL_checknumber(L, idx+2),
        };
    }
    return clr;
}

static Pico_Pct _pct (lua_State* L, int idx) {
    Pico_Pct pct;
    if (lua_type(L, idx) == LUA_TTABLE) {
        pct.x = L_checkfieldfloat(L, idx, "x");
        pct.y = L_checkfieldfloat(L, idx, "y");
    } else {
        pct.x = luaL_checknumber(L, idx);
        pct.y = luaL_checknumber(L, idx+1);
    }
    return pct;
}

static Pico_Rect _rect_raw (lua_State* L, int idx) {
    Pico_Rect rect;
    if (lua_type(L, idx) == LUA_TTABLE) {
        rect = (Pico_Rect) {
            L_checkfieldnum(L, idx, "x"),
            L_checkfieldnum(L, idx, "y"),
            L_checkfieldnum(L, idx, "w"),
            L_checkfieldnum(L, idx, "h")
        };
    } else {
        rect = (Pico_Rect) {
            luaL_checknumber(L, idx),
            luaL_checknumber(L, idx+1),
            luaL_checknumber(L, idx+2),
            luaL_checknumber(L, idx+3)
        };
    }
    return rect;
}

static Pico_Rect_Pct _rect_pct (lua_State* L, int idx) {
    luaL_checktype(L, idx, LUA_TTABLE);
    Pico_Rect_Pct rect;
    rect.x = L_checkfieldfloat(L, idx, "x");
    rect.y = L_checkfieldfloat(L, idx, "y");
    rect.w = L_checkfieldfloat(L, idx, "w");
    rect.h = L_checkfieldfloat(L, idx, "h");

    // Check for anchor field
    lua_getfield(L, idx, "anchor");
    if (lua_type(L, -1) == LUA_TTABLE) {
        rect.anchor.x = L_checkfieldfloat(L, -1, "x");
        rect.anchor.y = L_checkfieldfloat(L, -1, "y");
    } else {
        rect.anchor = PICO_ANCHOR_C;  // Default to center
    }
    lua_pop(L, 1);

    rect.up = NULL;  // TODO: support up pointer if needed
    return rect;
}

static Pico_Pos_Pct _pos_pct (lua_State* L, int idx) {
    luaL_checktype(L, idx, LUA_TTABLE);
    Pico_Pos_Pct pos;
    pos.x = L_checkfieldfloat(L, idx, "x");
    pos.y = L_checkfieldfloat(L, idx, "y");

    // Check for anchor field
    lua_getfield(L, idx, "anchor");
    if (lua_type(L, -1) == LUA_TTABLE) {
        pos.anchor.x = L_checkfieldfloat(L, -1, "x");
        pos.anchor.y = L_checkfieldfloat(L, -1, "y");
    } else {
        pos.anchor = PICO_ANCHOR_C;  // Default to center
    }
    lua_pop(L, 1);

    pos.up = NULL;  // TODO: support up pointer if needed
    return pos;
}

static void _push_xy (lua_State* L, int x, int y) {
    lua_newtable(L);            // . | t
    lua_pushinteger(L, x);      // . | t | x
    lua_setfield(L, -2, "x");   // . | t
    lua_pushinteger(L, y);      // . | t | y
    lua_setfield(L, -2, "y");   // . | [t]
}

static void _push_rect (lua_State* L, Pico_Rect r) {
    lua_newtable(L);
    lua_pushinteger(L, r.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, r.y);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, r.w);
    lua_setfield(L, -2, "w");
    lua_pushinteger(L, r.h);
    lua_setfield(L, -2, "h");
}

static void _push_color (lua_State* L, Pico_Color c) {
    lua_newtable(L);
    lua_pushinteger(L, c.r);
    lua_setfield(L, -2, "r");
    lua_pushinteger(L, c.g);
    lua_setfield(L, -2, "g");
    lua_pushinteger(L, c.b);
    lua_setfield(L, -2, "b");
}

///////////////////////////////////////////////////////////////////////////////
// INIT
///////////////////////////////////////////////////////////////////////////////

static int l_init (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int on = lua_toboolean(L, 1);
    pico_init(on);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// INPUT
///////////////////////////////////////////////////////////////////////////////

static int l_input_delay (lua_State* L) {
    int ms = luaL_checknumber(L, 1);
    pico_input_delay(ms);
    return 0;
}

static int l_input_event (lua_State* L) {
    int type = PICO_ANY;

    // Check if first arg is event type string
    if (lua_type(L, 1) == LUA_TSTRING) {
        const char* evt_str = lua_tostring(L, 1);
        if (strcmp(evt_str, "quit") == 0) type = PICO_QUIT;
        else if (strcmp(evt_str, "key.dn") == 0) type = PICO_KEYDOWN;
        else if (strcmp(evt_str, "key.up") == 0) type = PICO_KEYUP;
        else if (strcmp(evt_str, "mouse.button.dn") == 0) type = PICO_MOUSEBUTTONDOWN;
        else if (strcmp(evt_str, "mouse.button.up") == 0) type = PICO_MOUSEBUTTONUP;
        else if (strcmp(evt_str, "mouse.motion") == 0) type = PICO_MOUSEMOTION;
        else return luaL_error(L, "invalid event type: %s", evt_str);
    }

    Pico_Event e;
    pico_input_event(&e, type);

    lua_newtable(L);

    switch (e.type) {
        case PICO_QUIT:
            lua_pushstring(L, "quit");
            lua_setfield(L, -2, "tag");
            break;
        case PICO_MOUSEMOTION:
            lua_pushstring(L, "mouse.motion");
            lua_setfield(L, -2, "tag");
            lua_pushinteger(L, e.motion.x);
            lua_setfield(L, -2, "x");
            lua_pushinteger(L, e.motion.y);
            lua_setfield(L, -2, "y");
            break;
        case PICO_MOUSEBUTTONDOWN:
        case PICO_MOUSEBUTTONUP:
            lua_pushstring(L, (e.type == PICO_MOUSEBUTTONDOWN ? "mouse.button.dn" : "mouse.button.up"));
            lua_setfield(L, -2, "tag");
            lua_pushinteger(L, e.button.x);
            lua_setfield(L, -2, "x");
            lua_pushinteger(L, e.button.y);
            lua_setfield(L, -2, "y");
            lua_pushinteger(L, e.button.button);
            lua_setfield(L, -2, "button");
            break;
        case PICO_KEYDOWN:
        case PICO_KEYUP: {
            lua_pushstring(L, (e.type == PICO_KEYDOWN ? "key.dn" : "key.up"));
            lua_setfield(L, -2, "tag");
            const char* key = SDL_GetKeyName(e.key.keysym.sym);
            lua_pushstring(L, key);
            lua_setfield(L, -2, "key");
            break;
        }
    }

    return 1;
}

static int l_input_event_ask (lua_State* L) {
    int type = PICO_ANY;
    if (lua_type(L, 1) == LUA_TSTRING) {
        const char* evt_str = lua_tostring(L, 1);
        if (strcmp(evt_str, "quit") == 0) type = PICO_QUIT;
        else if (strcmp(evt_str, "key.dn") == 0) type = PICO_KEYDOWN;
        else if (strcmp(evt_str, "key.up") == 0) type = PICO_KEYUP;
        else if (strcmp(evt_str, "mouse.button.dn") == 0) type = PICO_MOUSEBUTTONDOWN;
        else if (strcmp(evt_str, "mouse.button.up") == 0) type = PICO_MOUSEBUTTONUP;
        else if (strcmp(evt_str, "mouse.motion") == 0) type = PICO_MOUSEMOTION;
    }

    Pico_Event e;
    int occurred = pico_input_event_ask(&e, type);

    if (!occurred) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_newtable(L);
    // Similar event handling as above...
    return 1;
}

static int l_input_event_timeout (lua_State* L) {
    int type = PICO_ANY;
    int timeout = luaL_checknumber(L, 2);

    if (lua_type(L, 1) == LUA_TSTRING) {
        const char* evt_str = lua_tostring(L, 1);
        if (strcmp(evt_str, "quit") == 0) type = PICO_QUIT;
        else if (strcmp(evt_str, "key.dn") == 0) type = PICO_KEYDOWN;
        else if (strcmp(evt_str, "key.up") == 0) type = PICO_KEYUP;
        else if (strcmp(evt_str, "mouse.button.dn") == 0) type = PICO_MOUSEBUTTONDOWN;
        else if (strcmp(evt_str, "mouse.button.up") == 0) type = PICO_MOUSEBUTTONUP;
        else if (strcmp(evt_str, "mouse.motion") == 0) type = PICO_MOUSEMOTION;
    }

    Pico_Event e;
    int occurred = pico_input_event_timeout(&e, type, timeout);

    lua_pushboolean(L, occurred);
    if (!occurred) {
        return 1;
    }

    lua_newtable(L);
    // Similar event handling...
    return 2;
}

///////////////////////////////////////////////////////////////////////////////
// OUTPUT - DRAWING
///////////////////////////////////////////////////////////////////////////////

static int l_output_clear (lua_State* L) {
    pico_output_clear();
    return 0;
}

static int l_output_draw_buffer_raw (lua_State* L) {
    Pico_Rect rect = _rect_raw(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    // Get buffer dimensions
    lua_len(L, 2);
    int w = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_geti(L, 2, 1);
    if (lua_type(L, -1) != LUA_TTABLE) {
        return luaL_error(L, "expected table of tables for buffer");
    }
    lua_len(L, -1);
    int h = lua_tointeger(L, -1);
    lua_pop(L, 2);

    Pico_Color buffer[w * h];
    for (int i = 0; i < w; i++) {
        lua_geti(L, 2, i+1);
        for (int j = 0; j < h; j++) {
            lua_geti(L, -1, j+1);
            buffer[i*h + j] = _color(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    pico_output_draw_buffer_raw(rect, buffer, (Pico_Dim){w, h});
    return 0;
}

static int l_output_draw_buffer_pct (lua_State* L) {
    Pico_Rect_Pct rect = _rect_pct(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    // Similar to raw version...
    lua_len(L, 2);
    int w = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_geti(L, 2, 1);
    lua_len(L, -1);
    int h = lua_tointeger(L, -1);
    lua_pop(L, 2);

    Pico_Color buffer[w * h];
    for (int i = 0; i < w; i++) {
        lua_geti(L, 2, i+1);
        for (int j = 0; j < h; j++) {
            lua_geti(L, -1, j+1);
            buffer[i*h + j] = _color(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    pico_output_draw_buffer_pct(&rect, buffer, (Pico_Dim){w, h});
    return 0;
}

static int l_output_draw_image_raw (lua_State* L) {
    Pico_Rect rect = _rect_raw(L, 1);
    const char* path = luaL_checkstring(L, 2);
    pico_output_draw_image_raw(rect, path);
    return 0;
}

static int l_output_draw_image_pct (lua_State* L) {
    Pico_Rect_Pct rect = _rect_pct(L, 1);
    const char* path = luaL_checkstring(L, 2);
    pico_output_draw_image_pct(&rect, path);
    return 0;
}

static int l_output_draw_line_raw (lua_State* L) {
    Pico_Pos p1, p2;
    if (lua_type(L, 1) == LUA_TTABLE) {
        p1.x = L_checkfieldnum(L, 1, "x");
        p1.y = L_checkfieldnum(L, 1, "y");
        p2.x = L_checkfieldnum(L, 2, "x");
        p2.y = L_checkfieldnum(L, 2, "y");
    } else {
        p1.x = luaL_checknumber(L, 1);
        p1.y = luaL_checknumber(L, 2);
        p2.x = luaL_checknumber(L, 3);
        p2.y = luaL_checknumber(L, 4);
    }
    pico_output_draw_line_raw(p1, p2);
    return 0;
}

static int l_output_draw_line_pct (lua_State* L) {
    Pico_Pos_Pct p1 = _pos_pct(L, 1);
    Pico_Pos_Pct p2 = _pos_pct(L, 2);
    pico_output_draw_line_pct(&p1, &p2);
    return 0;
}

static int l_output_draw_pixel_raw (lua_State* L) {
    Pico_Pos pos;
    if (lua_type(L, 1) == LUA_TTABLE) {
        pos.x = L_checkfieldnum(L, 1, "x");
        pos.y = L_checkfieldnum(L, 1, "y");
    } else {
        pos.x = luaL_checknumber(L, 1);
        pos.y = luaL_checknumber(L, 2);
    }
    pico_output_draw_pixel_raw(pos);
    return 0;
}

static int l_output_draw_pixel_pct (lua_State* L) {
    Pico_Pos_Pct pos = _pos_pct(L, 1);
    pico_output_draw_pixel_pct(&pos);
    return 0;
}

static int l_output_draw_pixels (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_len(L, 1);
    int n = lua_tointeger(L, -1);
    lua_pop(L, 1);

    Pico_Pos pixels[n];
    for (int i = 0; i < n; i++) {
        lua_geti(L, 1, i+1);
        pixels[i].x = L_checkfieldnum(L, -1, "x");
        pixels[i].y = L_checkfieldnum(L, -1, "y");
        lua_pop(L, 1);
    }

    pico_output_draw_pixels(pixels, n);
    return 0;
}

static int l_output_draw_rect_raw (lua_State* L) {
    Pico_Rect rect = _rect_raw(L, 1);
    pico_output_draw_rect_raw(rect);
    return 0;
}

static int l_output_draw_rect_pct (lua_State* L) {
    Pico_Rect_Pct rect = _rect_pct(L, 1);
    pico_output_draw_rect_pct(&rect);
    return 0;
}

static int l_output_draw_tri_raw (lua_State* L) {
    Pico_Pos p1, p2, p3;
    if (lua_type(L, 1) == LUA_TTABLE) {
        p1.x = L_checkfieldnum(L, 1, "x");
        p1.y = L_checkfieldnum(L, 1, "y");
        p2.x = L_checkfieldnum(L, 2, "x");
        p2.y = L_checkfieldnum(L, 2, "y");
        p3.x = L_checkfieldnum(L, 3, "x");
        p3.y = L_checkfieldnum(L, 3, "y");
    } else {
        p1.x = luaL_checknumber(L, 1);
        p1.y = luaL_checknumber(L, 2);
        p2.x = luaL_checknumber(L, 3);
        p2.y = luaL_checknumber(L, 4);
        p3.x = luaL_checknumber(L, 5);
        p3.y = luaL_checknumber(L, 6);
    }
    pico_output_draw_tri_raw(p1, p2, p3);
    return 0;
}

static int l_output_draw_tri_pct (lua_State* L) {
    Pico_Pos_Pct p1 = _pos_pct(L, 1);
    Pico_Pos_Pct p2 = _pos_pct(L, 2);
    Pico_Pos_Pct p3 = _pos_pct(L, 3);
    pico_output_draw_tri_pct(&p1, &p2, &p3);
    return 0;
}

static int l_output_draw_oval_raw (lua_State* L) {
    Pico_Rect rect = _rect_raw(L, 1);
    pico_output_draw_oval_raw(rect);
    return 0;
}

static int l_output_draw_oval_pct (lua_State* L) {
    Pico_Rect_Pct rect = _rect_pct(L, 1);
    pico_output_draw_oval_pct(&rect);
    return 0;
}

static int l_output_draw_poly_raw (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_len(L, 1);
    int n = lua_tointeger(L, -1);
    lua_pop(L, 1);

    Pico_Pos points[n];
    for (int i = 0; i < n; i++) {
        lua_geti(L, 1, i+1);
        points[i].x = L_checkfieldnum(L, -1, "x");
        points[i].y = L_checkfieldnum(L, -1, "y");
        lua_pop(L, 1);
    }

    pico_output_draw_poly_raw(points, n);
    return 0;
}

static int l_output_draw_poly_pct (lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_len(L, 1);
    int n = lua_tointeger(L, -1);
    lua_pop(L, 1);

    Pico_Pos_Pct points[n];
    for (int i = 0; i < n; i++) {
        lua_geti(L, 1, i+1);
        points[i] = _pos_pct(L, -1);
        lua_pop(L, 1);
    }

    pico_output_draw_poly_pct(points, n);
    return 0;
}

static int l_output_draw_text_raw (lua_State* L) {
    Pico_Rect rect = _rect_raw(L, 1);
    const char* text = luaL_checkstring(L, 2);
    pico_output_draw_text_raw(rect, text);
    return 0;
}

static int l_output_draw_text_pct (lua_State* L) {
    Pico_Rect_Pct rect = _rect_pct(L, 1);
    const char* text = luaL_checkstring(L, 2);
    pico_output_draw_text_pct(&rect, text);
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
    const char* result = pico_output_screenshot(path);
    if (result) {
        lua_pushstring(L, result);
        return 1;
    }
    return 0;
}

static int l_output_screenshot_ext (lua_State* L) {
    const char* path = NULL;
    if (lua_type(L, 1) == LUA_TSTRING) {
        path = lua_tostring(L, 1);
    }
    Pico_Rect r = _rect_raw(L, 2);
    const char* result = pico_output_screenshot_ext(path, r);
    if (result) {
        lua_pushstring(L, result);
        return 1;
    }
    return 0;
}

static int l_output_sound (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    pico_output_sound(path);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// STATE - GETTERS
///////////////////////////////////////////////////////////////////////////////

static int l_get_anchor_pos (lua_State* L) {
    Pico_Anchor anc = pico_get_anchor_pos();
    _push_xy(L, anc.x, anc.y);
    return 1;
}

static int l_get_anchor_rotate (lua_State* L) {
    Pico_Anchor anc = pico_get_anchor_rotate();
    _push_xy(L, anc.x, anc.y);
    return 1;
}

static int l_get_color_clear (lua_State* L) {
    Pico_Color c = pico_get_color_clear();
    _push_color(L, c);
    return 1;
}

static int l_get_color_draw (lua_State* L) {
    Pico_Color c = pico_get_color_draw();
    _push_color(L, c);
    return 1;
}

static int l_get_crop (lua_State* L) {
    Pico_Rect r = pico_get_crop();
    _push_rect(L, r);
    return 1;
}

static int l_get_expert (lua_State* L) {
    lua_pushboolean(L, pico_get_expert());
    return 1;
}

static int l_get_flip (lua_State* L) {
    Pico_Flip f = pico_get_flip();
    _push_xy(L, f.x, f.y);
    return 1;
}

static int l_get_font (lua_State* L) {
    const char* font = pico_get_font();
    if (font) {
        lua_pushstring(L, font);
        return 1;
    }
    return 0;
}

static int l_get_fullscreen (lua_State* L) {
    lua_pushboolean(L, pico_get_fullscreen());
    return 1;
}

static int l_get_grid (lua_State* L) {
    lua_pushboolean(L, pico_get_grid());
    return 1;
}

static int l_get_key (lua_State* L) {
    int key = luaL_checknumber(L, 1);
    lua_pushboolean(L, pico_get_key(key));
    return 1;
}

static int l_get_mouse (lua_State* L) {
    Pico_Pos pos;
    int button = PICO_MOUSE_BUTTON_NONE;
    if (lua_gettop(L) > 0) {
        button = luaL_checknumber(L, 1);
    }
    int state = pico_get_mouse(&pos, button);

    lua_newtable(L);
    lua_pushinteger(L, pos.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, pos.y);
    lua_setfield(L, -2, "y");
    lua_pushboolean(L, state);
    lua_setfield(L, -2, "pressed");
    return 1;
}

static int l_get_rotate (lua_State* L) {
    lua_pushinteger(L, pico_get_rotate());
    return 1;
}

static int l_get_dim_image (lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    Pico_Dim dim = pico_get_dim_image(path);
    _push_xy(L, dim.w, dim.h);
    return 1;
}

static int l_get_text_width (lua_State* L) {
    int h = luaL_checknumber(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lua_pushinteger(L, pico_get_text_width(h, text));
    return 1;
}

static int l_get_show (lua_State* L) {
    lua_pushboolean(L, pico_get_show());
    return 1;
}

static int l_get_style (lua_State* L) {
    PICO_STYLE style = pico_get_style();
    lua_pushstring(L, (style == PICO_FILL) ? "fill" : "stroke");
    return 1;
}

static int l_get_ticks (lua_State* L) {
    lua_pushinteger(L, pico_get_ticks());
    return 1;
}

static int l_get_title (lua_State* L) {
    const char* title = pico_get_title();
    if (title) {
        lua_pushstring(L, title);
        return 1;
    }
    return 0;
}

static int l_get_view (lua_State* L) {
    int window_fullscreen;
    Pico_Dim window, world;
    Pico_Rect window_target, world_source, world_clip;

    pico_get_view(
        &window_fullscreen,
        &window, &window_target,
        &world, &world_source, &world_clip
    );

    lua_newtable(L);

    lua_pushboolean(L, window_fullscreen);
    lua_setfield(L, -2, "window_fullscreen");

    _push_xy(L, window.w, window.h);
    lua_setfield(L, -2, "window");

    _push_rect(L, window_target);
    lua_setfield(L, -2, "window_target");

    _push_xy(L, world.w, world.h);
    lua_setfield(L, -2, "world");

    _push_rect(L, world_source);
    lua_setfield(L, -2, "world_source");

    _push_rect(L, world_clip);
    lua_setfield(L, -2, "world_clip");

    return 1;
}

///////////////////////////////////////////////////////////////////////////////
// STATE - SETTERS
///////////////////////////////////////////////////////////////////////////////

static int l_set_alpha (lua_State* L) {
    int a = luaL_checknumber(L, 1);
    pico_set_alpha(a);
    return 0;
}

static int l_set_anchor_pos (lua_State* L) {
    Pico_Anchor anc;
    if (lua_type(L, 1) == LUA_TTABLE) {
        anc.x = L_checkfieldnum(L, 1, "x");
        anc.y = L_checkfieldnum(L, 1, "y");
    } else {
        anc.x = luaL_checknumber(L, 1);
        anc.y = luaL_checknumber(L, 2);
    }
    pico_set_anchor_pos(anc);
    return 0;
}

static int l_set_anchor_rotate (lua_State* L) {
    Pico_Anchor anc;
    if (lua_type(L, 1) == LUA_TTABLE) {
        anc.x = L_checkfieldnum(L, 1, "x");
        anc.y = L_checkfieldnum(L, 1, "y");
    } else {
        anc.x = luaL_checknumber(L, 1);
        anc.y = luaL_checknumber(L, 2);
    }
    pico_set_anchor_rotate(anc);
    return 0;
}

static int l_set_color_clear (lua_State* L) {
    Pico_Color clr = _color(L, 1);
    pico_set_color_clear(clr);
    return 0;
}

static int l_set_color_draw (lua_State* L) {
    Pico_Color clr = _color(L, 1);
    pico_set_color_draw(clr);
    return 0;
}

static int l_set_crop (lua_State* L) {
    if (lua_gettop(L) == 0) {
        pico_set_crop((Pico_Rect){0, 0, 0, 0});
    } else {
        Pico_Rect r = _rect_raw(L, 1);
        pico_set_crop(r);
    }
    return 0;
}

static int l_set_expert (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    pico_set_expert(lua_toboolean(L, 1));
    return 0;
}

static int l_set_flip (lua_State* L) {
    Pico_Flip flip;
    if (lua_type(L, 1) == LUA_TTABLE) {
        flip.x = L_checkfieldnum(L, 1, "x");
        flip.y = L_checkfieldnum(L, 1, "y");
    } else {
        flip.x = luaL_checknumber(L, 1);
        flip.y = luaL_checknumber(L, 2);
    }
    pico_set_flip(flip);
    return 0;
}

static int l_set_font (lua_State* L) {
    const char* path = NULL;
    if (!lua_isnil(L, 1)) {
        path = luaL_checkstring(L, 1);
    }
    pico_set_font(path);
    return 0;
}

static int l_set_grid (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    pico_set_grid(lua_toboolean(L, 1));
    return 0;
}

static int l_set_rotate (lua_State* L) {
    int angle = luaL_checknumber(L, 1);
    pico_set_rotate(angle);
    return 0;
}

static int l_set_show (lua_State* L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    pico_set_show(lua_toboolean(L, 1));
    return 0;
}

static int l_set_style (lua_State* L) {
    const char* style_str = luaL_checkstring(L, 1);
    PICO_STYLE style;
    if (strcmp(style_str, "fill") == 0) {
        style = PICO_FILL;
    } else if (strcmp(style_str, "stroke") == 0) {
        style = PICO_STROKE;
    } else {
        return luaL_error(L, "invalid style: %s", style_str);
    }
    pico_set_style(style);
    return 0;
}

static int l_set_title (lua_State* L) {
    const char* title = luaL_checkstring(L, 1);
    pico_set_title(title);
    return 0;
}

static int l_set_view_raw (lua_State* L) {
    int window_fullscreen = -1;
    Pico_Dim* window = NULL;
    Pico_Rect* window_target = NULL;
    Pico_Dim* world = NULL;
    Pico_Rect* world_source = NULL;
    Pico_Rect* world_clip = NULL;

    Pico_Dim window_val, world_val;
    Pico_Rect window_target_val, world_source_val, world_clip_val;

    if (lua_gettop(L) >= 1 && lua_isboolean(L, 1)) {
        window_fullscreen = lua_toboolean(L, 1);
    }

    if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
        window_val = (Pico_Dim){
            L_checkfieldnum(L, 2, "w"),
            L_checkfieldnum(L, 2, "h")
        };
        window = &window_val;
    }

    if (lua_gettop(L) >= 3 && !lua_isnil(L, 3)) {
        window_target_val = _rect_raw(L, 3);
        window_target = &window_target_val;
    }

    if (lua_gettop(L) >= 4 && !lua_isnil(L, 4)) {
        world_val = (Pico_Dim){
            L_checkfieldnum(L, 4, "w"),
            L_checkfieldnum(L, 4, "h")
        };
        world = &world_val;
    }

    if (lua_gettop(L) >= 5 && !lua_isnil(L, 5)) {
        world_source_val = _rect_raw(L, 5);
        world_source = &world_source_val;
    }

    if (lua_gettop(L) >= 6 && !lua_isnil(L, 6)) {
        world_clip_val = _rect_raw(L, 6);
        world_clip = &world_clip_val;
    }

    pico_set_view_raw(
        window_fullscreen,
        window, window_target,
        world, world_source, world_clip
    );

    return 0;
}

static int l_set_view_pct (lua_State* L) {
    int window_fullscreen = -1;
    Pico_Pct* window = NULL;
    Pico_Rect* window_target = NULL;
    Pico_Pct* world = NULL;
    Pico_Rect* world_source = NULL;
    Pico_Rect* world_clip = NULL;

    Pico_Pct window_val, world_val;
    Pico_Rect window_target_val, world_source_val, world_clip_val;

    if (lua_gettop(L) >= 1 && lua_isboolean(L, 1)) {
        window_fullscreen = lua_toboolean(L, 1);
    }

    if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
        window_val = _pct(L, 2);
        window = &window_val;
    }

    if (lua_gettop(L) >= 3 && !lua_isnil(L, 3)) {
        window_target_val = _rect_raw(L, 3);
        window_target = &window_target_val;
    }

    if (lua_gettop(L) >= 4 && !lua_isnil(L, 4)) {
        world_val = _pct(L, 4);
        world = &world_val;
    }

    if (lua_gettop(L) >= 5 && !lua_isnil(L, 5)) {
        world_source_val = _rect_raw(L, 5);
        world_source = &world_source_val;
    }

    if (lua_gettop(L) >= 6 && !lua_isnil(L, 6)) {
        world_clip_val = _rect_raw(L, 6);
        world_clip = &world_clip_val;
    }

    pico_set_view_pct(
        window_fullscreen,
        window, window_target,
        world, world_source, world_clip
    );

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// COLLISION DETECTION
///////////////////////////////////////////////////////////////////////////////

static int l_pos_vs_rect_raw (lua_State* L) {
    Pico_Pos pos;
    if (lua_type(L, 1) == LUA_TTABLE) {
        pos.x = L_checkfieldnum(L, 1, "x");
        pos.y = L_checkfieldnum(L, 1, "y");
    } else {
        pos.x = luaL_checknumber(L, 1);
        pos.y = luaL_checknumber(L, 2);
    }

    Pico_Rect rect = _rect_raw(L, lua_type(L, 1) == LUA_TTABLE ? 2 : 3);
    lua_pushboolean(L, pico_pos_vs_rect_raw(pos, rect));
    return 1;
}

static int l_pos_vs_rect_pct (lua_State* L) {
    Pico_Pos_Pct pos = _pos_pct(L, 1);
    Pico_Rect_Pct rect = _rect_pct(L, 2);
    lua_pushboolean(L, pico_pos_vs_rect_pct(&pos, &rect));
    return 1;
}

static int l_rect_vs_rect_raw (lua_State* L) {
    Pico_Rect r1 = _rect_raw(L, 1);
    Pico_Rect r2 = _rect_raw(L, 2);
    lua_pushboolean(L, pico_rect_vs_rect_raw(r1, r2));
    return 1;
}

static int l_rect_vs_rect_pct (lua_State* L) {
    Pico_Rect_Pct r1 = _rect_pct(L, 1);
    Pico_Rect_Pct r2 = _rect_pct(L, 2);
    lua_pushboolean(L, pico_rect_vs_rect_pct(&r1, &r2));
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
// LIBRARY REGISTRATION
///////////////////////////////////////////////////////////////////////////////

static const luaL_Reg ll_funcs[] = {
    { "init", l_init },
    { NULL, NULL }
};

static const luaL_Reg ll_input[] = {
    { "delay", l_input_delay },
    { "event", l_input_event },
    { "event_ask", l_input_event_ask },
    { "event_timeout", l_input_event_timeout },
    { NULL, NULL }
};

static const luaL_Reg ll_output[] = {
    { "clear", l_output_clear },
    { "present", l_output_present },
    { "screenshot", l_output_screenshot },
    { "screenshot_ext", l_output_screenshot_ext },
    { "sound", l_output_sound },
    { NULL, NULL }
};

static const luaL_Reg ll_output_draw[] = {
    { "buffer_raw", l_output_draw_buffer_raw },
    { "buffer_pct", l_output_draw_buffer_pct },
    { "image_raw", l_output_draw_image_raw },
    { "image_pct", l_output_draw_image_pct },
    { "line_raw", l_output_draw_line_raw },
    { "line_pct", l_output_draw_line_pct },
    { "pixel_raw", l_output_draw_pixel_raw },
    { "pixel_pct", l_output_draw_pixel_pct },
    { "pixels", l_output_draw_pixels },
    { "rect_raw", l_output_draw_rect_raw },
    { "rect_pct", l_output_draw_rect_pct },
    { "tri_raw", l_output_draw_tri_raw },
    { "tri_pct", l_output_draw_tri_pct },
    { "oval_raw", l_output_draw_oval_raw },
    { "oval_pct", l_output_draw_oval_pct },
    { "poly_raw", l_output_draw_poly_raw },
    { "poly_pct", l_output_draw_poly_pct },
    { "text_raw", l_output_draw_text_raw },
    { "text_pct", l_output_draw_text_pct },
    { NULL, NULL }
};

static const luaL_Reg ll_get[] = {
    { "crop", l_get_crop },
    { "expert", l_get_expert },
    { "fullscreen", l_get_fullscreen },
    { "grid", l_get_grid },
    { "key", l_get_key },
    { "mouse", l_get_mouse },
    { "rotate", l_get_rotate },
    { "show", l_get_show },
    { "style", l_get_style },
    { "ticks", l_get_ticks },
    { "title", l_get_title },
    { "view", l_get_view },
    { NULL, NULL }
};

static const luaL_Reg ll_get_anchor[] = {
    { "pos", l_get_anchor_pos },
    { "rotate", l_get_anchor_rotate },
    { NULL, NULL }
};

static const luaL_Reg ll_get_color[] = {
    { "clear", l_get_color_clear },
    { "draw", l_get_color_draw },
    { NULL, NULL }
};

static const luaL_Reg ll_get_dim[] = {
    { "image", l_get_dim_image },
    { NULL, NULL }
};

static const luaL_Reg ll_set[] = {
    { "alpha", l_set_alpha },
    { "crop", l_set_crop },
    { "expert", l_set_expert },
    { "flip", l_set_flip },
    { "font", l_set_font },
    { "grid", l_set_grid },
    { "rotate", l_set_rotate },
    { "show", l_set_show },
    { "style", l_set_style },
    { "title", l_set_title },
    { "view_raw", l_set_view_raw },
    { "view_pct", l_set_view_pct },
    { NULL, NULL }
};

static const luaL_Reg ll_set_anchor[] = {
    { "pos", l_set_anchor_pos },
    { "rotate", l_set_anchor_rotate },
    { NULL, NULL }
};

static const luaL_Reg ll_set_color[] = {
    { "clear", l_set_color_clear },
    { "draw", l_set_color_draw },
    { NULL, NULL }
};

static const luaL_Reg ll_collision[] = {
    { "pos_rect_raw", l_pos_vs_rect_raw },
    { "pos_rect_pct", l_pos_vs_rect_pct },
    { "rect_rect_raw", l_rect_vs_rect_raw },
    { "rect_rect_pct", l_rect_vs_rect_pct },
    { NULL, NULL }
};

int luaopen_pico_native (lua_State* L) {
    lua_pushlightuserdata(L, (void*)&KEY);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    luaL_newlib(L, ll_funcs);                       // pico

    // input
    luaL_newlib(L, ll_input);
    lua_setfield(L, -2, "input");

    // output
    luaL_newlib(L, ll_output);
    luaL_newlib(L, ll_output_draw);
    lua_setfield(L, -2, "draw");
    lua_setfield(L, -2, "output");

    // get
    luaL_newlib(L, ll_get);
    luaL_newlib(L, ll_get_anchor);
    lua_setfield(L, -2, "anchor");
    luaL_newlib(L, ll_get_color);
    lua_setfield(L, -2, "color");
    luaL_newlib(L, ll_get_dim);
    lua_setfield(L, -2, "dim");
    lua_setfield(L, -2, "get");

    // set
    luaL_newlib(L, ll_set);
    luaL_newlib(L, ll_set_anchor);
    lua_setfield(L, -2, "anchor");
    luaL_newlib(L, ll_set_color);
    lua_setfield(L, -2, "color");
    lua_setfield(L, -2, "set");

    // collision
    luaL_newlib(L, ll_collision);
    lua_setfield(L, -2, "collision");

    // Constants - anchor values
    lua_newtable(L);
    lua_pushnumber(L, PICO_ANCHOR_LEFT);
    lua_setfield(L, -2, "LEFT");
    lua_pushnumber(L, PICO_ANCHOR_CENTER);
    lua_setfield(L, -2, "CENTER");
    lua_pushnumber(L, PICO_ANCHOR_RIGHT);
    lua_setfield(L, -2, "RIGHT");
    lua_pushnumber(L, PICO_ANCHOR_TOP);
    lua_setfield(L, -2, "TOP");
    lua_pushnumber(L, PICO_ANCHOR_MIDDLE);
    lua_setfield(L, -2, "MIDDLE");
    lua_pushnumber(L, PICO_ANCHOR_BOTTOM);
    lua_setfield(L, -2, "BOTTOM");

    lua_newtable(L);  // C anchor
    lua_pushnumber(L, PICO_ANCHOR_CENTER);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, PICO_ANCHOR_MIDDLE);
    lua_setfield(L, -2, "y");
    lua_setfield(L, -2, "C");

    lua_newtable(L);  // NW anchor
    lua_pushnumber(L, PICO_ANCHOR_LEFT);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, PICO_ANCHOR_TOP);
    lua_setfield(L, -2, "y");
    lua_setfield(L, -2, "NW");

    lua_newtable(L);  // E anchor
    lua_pushnumber(L, PICO_ANCHOR_RIGHT);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, PICO_ANCHOR_MIDDLE);
    lua_setfield(L, -2, "y");
    lua_setfield(L, -2, "E");

    lua_newtable(L);  // SE anchor
    lua_pushnumber(L, PICO_ANCHOR_RIGHT);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, PICO_ANCHOR_BOTTOM);
    lua_setfield(L, -2, "y");
    lua_setfield(L, -2, "SE");

    lua_setfield(L, -2, "anchor");

    return 1;
}
