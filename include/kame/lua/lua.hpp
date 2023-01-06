#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>

#include <kame/math/math.hpp>

namespace kame::lua {

static const char* mtVector3Name = "kame.math.Vector3";

struct Lua {
    lua_State* L = nullptr;

    void initLua();
    void shutdownLua();

    void openKameMath();
    void openLibs();

    void clearStack();
    int getStackSize();
    void pop(int n = 1);

    int doString(const char* code);

    int getGlobal(const char* key);

    bool popBoolean();
    std::string popString();
    lua_Number popNumber(int* isnum = nullptr);
    lua_Integer popInteger(int* isnum = nullptr);
    kame::math::Vector3 popVector3();

    static const char* _luaTypeToString(int type);
    static const char* _statusCodeToString(int statusCode);
};

} // namespace kame::lua
