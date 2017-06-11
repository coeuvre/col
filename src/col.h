#ifndef _COL_H
#define _COL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define COL_CONTEXT_LUA_FIELD "COL_CONTEXT"

typedef struct ColContext {
    lua_State *L;
    int windowWidth;
    int windowHeight;
    char *windowTitle;

    int luaStateRef;
    int luaUpdateRef;
    int luaRenderRef;

    int lastError;
    char lastErrorMessage[1024];
} ColContext;

ColContext *create(void);
void update(ColContext *ctx);
void render(ColContext *ctx);

#endif  // _COL_H
