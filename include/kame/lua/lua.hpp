#pragma once

#include "lua_header.hpp"

#include <string>
#include <functional>
#include <variant>
#include <map>

#include <kame/math/math.hpp>

namespace kame::lua {

static const char* const mtMatrixName = "kame.math.Matrix";
static const char* const mtVector3Name = "kame.math.Vector3";
static const char* const mtVector4Name = "kame.math.Vector4";

int openKameMathVector3(lua_State* L);
int openKameMathVector4(lua_State* L);
int openKameMathMatrix(lua_State* L);
int openKameLove2d(lua_State* L);

struct Lua {
    lua_State* L = nullptr;

    void initLua();
    void shutdownLua();

    void openKameMath();
    void openKameLove();
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
