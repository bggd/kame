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

int luaopen_kame_math_Vector3(lua_State* L);
void Lua::openKameMath()
{
    luaL_requiref(L, "kame.math.Vector3", luaopen_kame_math_Vector3, 0);
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
        pop();
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
        default:
            break;
    }
    return "Unkown Status";
}

} // namespace kame::lua
