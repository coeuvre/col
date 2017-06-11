#include "libcol.h"

static const char *COL_LUA_SOURCE =
    "local M = {}\n"
    "return M\n";

static int l_register(lua_State *L) {
    if (lua_gettop(L) != 3) {
        return luaL_error(L, "expecting exactly 3 arguments");
    }

    luaL_checktype(L, 1, LUA_TTABLE);     // state
    luaL_checktype(L, 2, LUA_TFUNCTION);  // update
    luaL_checktype(L, 3, LUA_TFUNCTION);  // render

    lua_getfield(L, LUA_REGISTRYINDEX, COL_CONTEXT_LUA_FIELD);
    ColContext *ctx = *(ColContext **)lua_touserdata(L, -1);
    lua_pop(L, 1);

    ctx->luaRenderRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ctx->luaUpdateRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ctx->luaStateRef = luaL_ref(L, LUA_REGISTRYINDEX);

    return 0;
}

static const luaL_Reg col[] = {
    {"register", l_register},
    {0, 0},
};

int luaopen_col(lua_State *L) {
    luaL_checkversion(L);
    int error = luaL_dostring(L, COL_LUA_SOURCE);
    if (error) {
        lua_error(L);
    }
    luaL_setfuncs(L, col, 0);
    return 1;
}
