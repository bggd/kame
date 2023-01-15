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

    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector3_zero(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_newuserdata(L, sizeof(kame::math::Vector3));
    *v = kame::math::Vector3::zero();

    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector3_one(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_newuserdata(L, sizeof(kame::math::Vector3));
    *v = kame::math::Vector3::one();

    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector3_dot(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
    lua_pushnumber(L, kame::math::Vector3::dot(*a, *b));
    return 1;
}

int Vector3_lengthSquared(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    lua_pushnumber(L, kame::math::Vector3::lengthSquared(*a));
    return 1;
}

int Vector3_length(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    lua_pushnumber(L, kame::math::Vector3::length(*a));
    return 1;
}

int Vector3_normalize(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
    auto* v = new (p) kame::math::Vector3();
    *v = kame::math::Vector3::normalize(*a);
    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector3_cross(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
    auto* v = new (p) kame::math::Vector3();
    *v = kame::math::Vector3::cross(*a, *b);
    luaL_getmetatable(L, mtVector3Name);
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

    lua_pushnil(L);
    return 1;
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

    lua_pushnil(L);
    return 1;
}

int Vector3_unm(lua_State* L)
{
    auto* v = (kame::math::Vector3*)lua_touserdata(L, 1);
    *v = -(*v);
    return 1;
}

int Vector3_add(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
    auto* v = new (p) kame::math::Vector3();
    *v = *a + *b;
    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector3_sub(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
    auto* v = new (p) kame::math::Vector3();
    *v = *a - *b;
    luaL_getmetatable(L, mtVector3Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector3_mul(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    if (lua_isuserdata(L, 2))
    {
        auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
        auto* v = new (p) kame::math::Vector3();
        *v = *a * *b;
        luaL_getmetatable(L, mtVector3Name);
        lua_setmetatable(L, -2);
        return 1;
    }
    else
    {
        lua_Number b = luaL_checknumber(L, 2);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
        auto* v = new (p) kame::math::Vector3();
        *v = *a * b;
        luaL_getmetatable(L, mtVector3Name);
        lua_setmetatable(L, -2);
        return 1;
    }
}

int Vector3_div(lua_State* L)
{
    auto* a = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
    if (lua_isuserdata(L, 2))
    {
        auto* b = (kame::math::Vector3*)luaL_checkudata(L, 2, mtVector3Name);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
        auto* v = new (p) kame::math::Vector3();
        *v = *a / *b;
        luaL_getmetatable(L, mtVector3Name);
        lua_setmetatable(L, -2);
        return 1;
    }
    else
    {
        lua_Number b = luaL_checknumber(L, 2);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector3));
        auto* v = new (p) kame::math::Vector3();
        *v = *a / b;
        luaL_getmetatable(L, mtVector3Name);
        lua_setmetatable(L, -2);
        return 1;
    }
}

int openKameMathVector3(lua_State* L)
{
    static const struct luaL_Reg vector3Funcs[] = {
        {"new", Vector3_new},
        {"zero", Vector3_zero},
        {"one", Vector3_one},
        {"dot", Vector3_dot},
        {"lengthSquared", Vector3_lengthSquared},
        {"length", Vector3_length},
        {"normalize", Vector3_normalize},
        {"cross", Vector3_cross},
        {NULL, NULL}};
    static const struct luaL_Reg vector3MetaMethods[] = {
        {"__newindex", Vector3_newIndex},
        {"__index", Vector3_index},
        {"__unm", Vector3_unm},
        {"__add", Vector3_add},
        {"__sub", Vector3_sub},
        {"__mul", Vector3_mul},
        {"__div", Vector3_div},
        {NULL, NULL}};
    luaL_newmetatable(L, mtVector3Name);
    luaL_setfuncs(L, vector3MetaMethods, 0);
    luaL_newlib(L, vector3Funcs);
    return 1;
}

} // namespace kame::lua
