#include <all.hpp>

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
    assert(n > 0);
    lua_pop(L, n);
}

int Lua::doString(const char* code)
{
    int status = luaL_dostring(L, code);
    if (status != LUA_OK)
    {
        const char* statusStr = _statusCodeToString(status);
        const char* errMsg = lua_tostring(L, -1);
        SPDLOG_INFO("status: [{}], error msg: {}", statusStr, errMsg);
        lua_pop(L, 1);
    }
    return status;
}

int Lua::getGlobal(const char* key)
{
    auto sequence = pystring::split(key, ".");
    std::string prevName;
    for (int i = 0; i < sequence.size(); ++i)
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
                SPDLOG_ERROR("'{}' is invalid access because '{}' is not table", key, prevName);
            }
            lua_getfield(L, -1, name.c_str());
            lua_remove(L, -2);
        }
        prevName += "." + name;
    }
    int t = lua_type(L, -1);
    return t;
}

bool Lua::toBoolean()
{
    int b = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return b;
}

std::string Lua::toString()
{
    size_t len;
    const char* cstr = lua_tolstring(L, -1, &len);
    lua_pop(L, -1);
    if (cstr)
    {
        return std::string(cstr, len);
    }
    else
    {
        return "";
    }
}

lua_Number Lua::toNumber(int* isnum)
{
    if (isnum)
    {
        lua_Number n = lua_tonumberx(L, -1, isnum);
        lua_pop(L, -1);
        return n;
    }
    else
    {
        lua_Number n = lua_tonumber(L, -1);
        lua_pop(L, -1);
        return n;
    }
}

lua_Integer Lua::toInteger(int* isnum)
{
    if (isnum)
    {
        lua_Integer n = lua_tointegerx(L, -1, isnum);
        lua_pop(L, -1);
        return n;
    }
    else
    {
        lua_Integer n = lua_tointeger(L, -1);
        lua_pop(L, -1);
        return n;
    }
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
        default:
            break;
    }
    return "Unkown Status";
}

} // namespace kame::lua
