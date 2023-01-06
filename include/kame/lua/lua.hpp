#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>

namespace kame::lua {

struct Lua {
    lua_State* L = nullptr;

    void initLua();
    void shutdownLua();

    void openLibs();

    void clearStack();
    int getStackSize();
    void pop(int n = 0);

    int doString(const char* code);

    int getGlobal(const char* key);

    // Lua::to* functions pop the stack.
    bool toBoolean();
    std::string toString();
    lua_Number toNumber(int* isnum = nullptr);
    lua_Integer toInteger(int* isnum = nullptr);

    const char* _statusCodeToString(int statusCode);
};

} // namespace kame::lua
