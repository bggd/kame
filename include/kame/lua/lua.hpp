#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <functional>

#include <kame/math/math.hpp>

namespace kame::lua {

static const char* mtMatrixName = "kame.math.Matrix";
static const char* mtVector3Name = "kame.math.Vector3";
static const char* mtVector4Name = "kame.math.Vector4";

struct Lua {
    lua_State* L = nullptr;

    void initLua();
    void shutdownLua();

    void openKameMath();
    void openLibs();

    void clearStack();
    int getStackSize();
    void pop(int n = 1);

    int doFile(const char* path);
    int doString(const char* code);

    int getGlobal(const char* key);

    bool popBoolean();
    std::string popString();
    lua_Number popNumber(int* isnum = nullptr);
    lua_Integer popInteger(int* isnum = nullptr);
    kame::math::Vector3 popVector3();
    kame::math::Vector4 popVector4();
    kame::math::Matrix popMatrix();

    void ipairs(std::function<void(int, int)> fn);

    static const char* _luaTypeToString(int type);
    static const char* _statusCodeToString(int statusCode);
};

} // namespace kame::lua
