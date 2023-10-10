#include <all.hpp>

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix.hpp"

// copy from lua.c
/*
** Message handler used to run all chunks
*/
static int msghandler(lua_State* L)
{
    const char* msg = lua_tostring(L, 1);
    if (msg == NULL)
    {                                            /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)      /* that produces a string? */
            return 1;                            /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                                  luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1); /* append a standard traceback */
    return 1;                     /* return the traceback */
}

namespace kame::lua {

void Lua::initLua()
{
    L = luaL_newstate();
}

void Lua::shutdownLua()
{
    lua_close(L);
    L = nullptr;
}

void Lua::openKameMath()
{
    luaL_requiref(L, "kame.math.Matrix", openKameMathMatrix, 0);
    luaL_requiref(L, "kame.math.Vector3", openKameMathVector3, 0);
    luaL_requiref(L, "kame.math.Vector4", openKameMathVector4, 0);
    clearStack();
}

void Lua::openLibs()
{
    luaL_openlibs(L);
}

void Lua::clearStack()
{
    lua_settop(L, 0);
}

int Lua::getStackSize()
{
    return lua_gettop(L);
}

void Lua::pop(int n)
{
    assert(n >= 0);
    lua_pop(L, n);
}

int Lua::doFile(const char* path)
{
    int status = luaL_loadfile(L, path);
    if (status != LUA_OK)
    {
        const char* statusStr = _statusCodeToString(status);
        std::string errMsg = popString();
        SPDLOG_INFO("status: [{}], error msg: {}", statusStr, errMsg);
    }
    int base = lua_gettop(L);
    lua_pushcfunction(L, msghandler);
    lua_insert(L, base);
    status = lua_pcall(L, 0, LUA_MULTRET, base);
    if (status != LUA_OK)
    {
        const char* statusStr = _statusCodeToString(status);
        size_t len = 0;
        const char* pStr = lua_tolstring(L, -1, &len);
        std::string errMsg(pStr, len);
        SPDLOG_INFO("status: [{}], error msg: {}", statusStr, errMsg);
    }
    lua_remove(L, base);
    return status;
}

int Lua::doString(const char* code)
{
    int status = luaL_loadstring(L, code);
    if (status != LUA_OK)
    {
        const char* statusStr = _statusCodeToString(status);
        std::string errMsg = popString();
        SPDLOG_INFO("status: [{}], error msg: {}", statusStr, errMsg);
    }
    int base = lua_gettop(L);
    lua_pushcfunction(L, msghandler);
    lua_insert(L, base);
    status = lua_pcall(L, 0, LUA_MULTRET, base);
    if (status != LUA_OK)
    {
        const char* statusStr = _statusCodeToString(status);
        size_t len = 0;
        const char* pStr = lua_tolstring(L, -1, &len);
        std::string errMsg(pStr, len);
        SPDLOG_INFO("status: [{}], error msg: {}", statusStr, errMsg);
    }
    lua_remove(L, base);
    return status;
}

int Lua::getGlobal(const char* key)
{
    auto sequence = pystring::split(key, ".");
    std::string prevName;
    for (size_t i = 0; i < sequence.size(); ++i)
    {
        auto name = sequence.at(i);
        if (i == 0)
        {
            lua_getglobal(L, name.c_str());
        }
        else
        {
            if (!lua_istable(L, -1))
            {
                SPDLOG_DEBUG("'{}' is invalid access because '{}' is not table", key, prevName);
                clearStack();
                lua_pushnil(L);
                break;
            }
            lua_getfield(L, -1, name.c_str());
            lua_remove(L, -2);
        }
        prevName += "." + name;
    }
    int t = lua_type(L, -1);
    return t;
}

bool Lua::popBoolean()
{
    int b = lua_toboolean(L, -1);
    pop();
    return b;
}

std::string Lua::popString()
{
    size_t len;
    const char* cstr = lua_tolstring(L, -1, &len);
    if (cstr)
    {
        std::string str(cstr, len);
        pop();
        return str;
    }
    else
    {
        pop();
        return "";
    }
}

lua_Number Lua::popNumber(int* isnum)
{
    lua_Number n = lua_tonumberx(L, -1, isnum);
    pop();
    return n;
}

lua_Integer Lua::popInteger(int* isnum)
{
    lua_Integer n = lua_tointegerx(L, -1, isnum);
    pop();
    return n;
}

kame::math::Vector3 Lua::popVector3()
{
    auto* v = (kame::math::Vector3*)lua_touserdata(L, -1);
    pop();
    return *v;
}

kame::math::Vector4 Lua::popVector4()
{
    auto* v = (kame::math::Vector4*)lua_touserdata(L, -1);
    pop();
    return *v;
}

kame::math::Matrix Lua::popMatrix()
{
    auto* v = (kame::math::Matrix*)lua_touserdata(L, -1);
    pop();
    return *v;
}

void Lua::ipairs(std::function<void(int, int)> fn)
{
    lua_Integer e = luaL_len(L, -1);
    for (lua_Integer i = 0; i < e; ++i)
    {
        int idx = i + 1;
        int n = getStackSize();
        int type = lua_geti(L, -1, idx);
        fn(idx, type);
        pop(getStackSize() - n);
    }
    pop();
}

const char* Lua::_luaTypeToString(int type)
{
    switch (type)
    {
        case LUA_TNONE:
            return "LUA_TNONE";
        case LUA_TNIL:
            return "LUA_TNIL";
        case LUA_TBOOLEAN:
            return "LUA_TBOOLEAN";
        case LUA_TLIGHTUSERDATA:
            return "LUA_TLIGHTUSERDATA";
        case LUA_TNUMBER:
            return "LUA_TNUMBER";
        case LUA_TSTRING:
            return "LUA_TSTRING";
        case LUA_TTABLE:
            return "LUA_TTABLE";
        case LUA_TFUNCTION:
            return "LUA_TFUNCTION";
        case LUA_TUSERDATA:
            return "LUA_TUSERDATA";
        case LUA_TTHREAD:
            return "LUA_TTHREAD";
        default:
            break;
    }
    return "Unknown type";
}

const char* Lua::_statusCodeToString(int statusCode)
{
    switch (statusCode)
    {
        case LUA_OK:
            return "LUA_OK";
        case LUA_YIELD:
            return "LUA_YIELD";
        case LUA_ERRRUN:
            return "LUA_ERRRUN";
        case LUA_ERRSYNTAX:
            return "LUA_ERRSYNTAX";
        case LUA_ERRMEM:
            return "LUA_ERRMEM";
        case LUA_ERRERR:
            return "LUA_ERRERR";
        case LUA_ERRFILE:
            return "LUA_ERRFILE";
        default:
            break;
    }
    return "Unkown Status";
}

} // namespace kame::lua
