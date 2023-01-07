#include <all.hpp>

namespace kame::lua {

int Vector3_new(lua_State* L)
{
    int argc = lua_gettop(L);

    void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));

    if (argc == 0)
    {

        new (p) kame::math::Vector3();
    }
    else if (argc == 1)
    {
        new (p) kame::math::Vector3(luaL_checknumber(L, 1));
    }
    else if (argc == 2)
    {
        luaL_error(L, "unimplemented");
    }
    else if (argc == 3)
    {
        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);
        new (p) kame::math::Vector3(x, y, z);
    }
    else
    {
        luaL_error(L, fmt::format("{} argment size is invalid", argc).c_str());
    }

    luaL_getmetatable(L, "kame.math.Vector3");
    lua_setmetatable(L, -2);

    return 1;
}

int Vector3_index(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_touserdata(L, 1);

    size_t len = 0;
    const char* pStr = luaL_checklstring(L, 2, &len);
    std::string key(pStr, len);
    if (key == "x")
    {
        lua_pushnumber(L, v->x);
        return 1;
    }
    else if (key == "y")
    {
        lua_pushnumber(L, v->y);
        return 1;
    }
    else if (key == "z")
    {
        lua_pushnumber(L, v->z);
        return 1;
    }

    luaL_error(L, "invalid key");
    return 0;
}

int Vector3_newIndex(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_touserdata(L, 1);

    size_t len = 0;
    const char* pStr = luaL_checklstring(L, 2, &len);
    std::string key(pStr, len);
    if (key == "x")
    {
        v->x = luaL_checknumber(L, 1);
        return 0;
    }
    else if (key == "y")
    {
        v->y = luaL_checknumber(L, 2);
        return 0;
    }
    else if (key == "z")
    {
        v->z = luaL_checknumber(L, 3);
        return 0;
    }

    luaL_error(L, "invalid key");
    return 0;
}

int Vector3_unm(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_touserdata(L, 1);
    *v = -(*v);
    return 1;
}

int luaopen_kame_math_Vector3(lua_State* L)
{
    static const struct luaL_Reg vector3Funcs[] = {
        {"new", Vector3_new},
        {NULL, NULL}};
    static const struct luaL_Reg vector3MetaMethods[] = {
        {"__newindex", Vector3_newIndex},
        {"__index", Vector3_index},
        {"__unm", Vector3_unm},
        {NULL, NULL}};
    luaL_newmetatable(L, mtVector3Name);
    luaL_setfuncs(L, vector3MetaMethods, 0);
    luaL_newlib(L, vector3Funcs);
    return 1;
}

} // namespace kame::lua
