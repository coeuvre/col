#include "col.h"
#include "libcol.h"

static void checkLuaError(ColContext *ctx, int error) {
    if (error) {
        ctx->lastError = error;
        snprintf(ctx->lastErrorMessage, sizeof(ctx->lastErrorMessage),
                 "%s\n", lua_tostring(ctx->L, -1));
    }
}

static void initLua(ColContext *ctx) {
    lua_State *L = luaL_newstate();

    ctx->L = L;
    ctx->luaStateRef = LUA_NOREF;
    ctx->luaUpdateRef = LUA_NOREF;
    ctx->luaRenderRef = LUA_NOREF;

    luaL_openlibs(L);
    luaL_requiref(L, "col", luaopen_col, 1);
    lua_pop(L, 1);

    ColContext** ctxRef = lua_newuserdata(L, sizeof(ColContext **));
    *ctxRef = ctx;
    lua_setfield(L, LUA_REGISTRYINDEX, COL_CONTEXT_LUA_FIELD);
}

static void loadLuaMain(ColContext *ctx) {
    lua_State *L = ctx->L;

    int error = luaL_dofile(L, "main.lua");
    checkLuaError(ctx, error);
}

ColContext *create(void) {
    ColContext *ctx = malloc(sizeof(ColContext));

    ctx->windowWidth = 800;
    ctx->windowHeight = 600;
    ctx->windowTitle = "COL";
    ctx->lastError = 0;
    ctx->lastErrorMessage[0] = 0;

    initLua(ctx);
    loadLuaMain(ctx);

    return ctx;
}

void update(ColContext *ctx) {
    lua_State *L = ctx->L;

    if (ctx->luaUpdateRef != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->luaUpdateRef);
        if (ctx->luaStateRef != LUA_NOREF) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->luaStateRef);
        } else {
            lua_pushnil(L);
        }
        int error = lua_pcall(L, 1, 0, 0);
        checkLuaError(ctx, error);
    }
}

void render(ColContext *ctx) {
    lua_State *L = ctx->L;

    if (ctx->luaRenderRef != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->luaRenderRef);
        if (ctx->luaStateRef != LUA_NOREF) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->luaStateRef);
        } else {
            lua_pushnil(L);
        }
        int error = lua_pcall(L, 1, 0, 0);
        checkLuaError(ctx, error);
    }
}
