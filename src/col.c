#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

lua_State *prepare() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    int error = luaL_loadfile(L, "main.lua");
    if (error) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        exit(EXIT_FAILURE);
    }

    return L;
}
