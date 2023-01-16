#pragma once

#include <all.hpp>

namespace kame::lua {

int Vector4_new(lua_State* L)
{
    int argc = lua_gettop(L);

    void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));

    if (argc == 0)
    {
        new (p) kame::math::Vector4();
    }
    else if (argc == 1)
    {
        new (p) kame::math::Vector4(luaL_checknumber(L, 1));
    }
    else if (argc == 2)
    {
        auto* v = (kame::math::Vector3*)luaL_checkudata(L, 1, mtVector3Name);
        auto w = luaL_checknumber(L, 2);
        new (p) kame::math::Vector4(*v, w);
    }
    else if (argc == 3)
    {
        luaL_error(L, "unimplemented");
    }
    else if (argc == 4)
    {
        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);
        auto w = luaL_checknumber(L, 4);
        new (p) kame::math::Vector4(x, y, z, w);
    }

    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector4_zero(lua_State* L)
{
    auto* v = (kame::math::Vector4*)lua_newuserdata(L, sizeof(kame::math::Vector4));
    *v = kame::math::Vector4::zero();

    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector4_one(lua_State* L)
{
    auto* v = (kame::math::Vector4*)lua_newuserdata(L, sizeof(kame::math::Vector4));
    *v = kame::math::Vector4::one();

    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);

    return 1;
}

int Vector4_dot(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    auto* b = (kame::math::Vector4*)luaL_checkudata(L, 2, mtVector4Name);
    lua_pushnumber(L, kame::math::Vector4::dot(*a, *b));
    return 1;
}

int Vector4_lengthSquared(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    lua_pushnumber(L, kame::math::Vector4::lengthSquared(*a));
    return 1;
}

int Vector4_length(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    lua_pushnumber(L, kame::math::Vector4::length(*a));
    return 1;
}

int Vector4_normalize(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
    auto* v = new (p) kame::math::Vector4();
    *v = kame::math::Vector4::normalize(*a);
    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector4_index(lua_State* L)
{
    auto* v = (kame::math::Vector4*)lua_touserdata(L, 1);

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
    else if (key == "w")
    {
        lua_pushnumber(L, v->w);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

int Vector4_newIndex(lua_State* L)
{
    auto* v = (kame::math::Vector4*)lua_touserdata(L, 1);

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
    else if (key == "w")
    {
        v->w = luaL_checknumber(L, 4);
        return 0;
    }

    lua_pushnil(L);
    return 1;
}

int Vector4_unm(lua_State* L)
{
    auto* v = (kame::math::Vector4*)lua_touserdata(L, 1);
    *v = -(*v);
    return 1;
}

int Vector4_add(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    auto* b = (kame::math::Vector4*)luaL_checkudata(L, 2, mtVector4Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
    auto* v = new (p) kame::math::Vector4();
    *v = *a + *b;
    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector4_sub(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    auto* b = (kame::math::Vector4*)luaL_checkudata(L, 2, mtVector4Name);
    void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
    auto* v = new (p) kame::math::Vector4();
    *v = *a - *b;
    luaL_getmetatable(L, mtVector4Name);
    lua_setmetatable(L, -2);
    return 1;
}

int Vector4_mul(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    if (lua_isuserdata(L, 2))
    {
        auto* b = (kame::math::Vector4*)luaL_checkudata(L, 2, mtVector4Name);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
        auto* v = new (p) kame::math::Vector4();
        *v = *a * *b;
        luaL_getmetatable(L, mtVector4Name);
        lua_setmetatable(L, -2);
        return 1;
    }
    else
    {
        lua_Number b = luaL_checknumber(L, 2);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
        auto* v = new (p) kame::math::Vector4();
        *v = *a * b;
        luaL_getmetatable(L, mtVector4Name);
        lua_setmetatable(L, -2);
        return 1;
    }
}

int Vector4_div(lua_State* L)
{
    auto* a = (kame::math::Vector4*)luaL_checkudata(L, 1, mtVector4Name);
    if (lua_isuserdata(L, 2))
    {
        auto* b = (kame::math::Vector4*)luaL_checkudata(L, 2, mtVector4Name);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
        auto* v = new (p) kame::math::Vector4();
        *v = *a / *b;
        luaL_getmetatable(L, mtVector4Name);
        lua_setmetatable(L, -2);
        return 1;
    }
    else
    {
        lua_Number b = luaL_checknumber(L, 2);
        void* p = lua_newuserdata(L, sizeof(kame::math::Vector4));
        auto* v = new (p) kame::math::Vector4();
        *v = *a / b;
        luaL_getmetatable(L, mtVector4Name);
        lua_setmetatable(L, -2);
        return 1;
    }
}

int openKameMathVector4(lua_State* L)
{
    static const struct luaL_Reg vector3Funcs[] = {
        {"new", Vector4_new},
        {"zero", Vector4_zero},
        {"one", Vector4_one},
        {"dot", Vector4_dot},
        {"lengthSquared", Vector4_lengthSquared},
        {"length", Vector4_length},
        {"normalize", Vector4_normalize},
        {NULL, NULL}};
    static const struct luaL_Reg vector3MetaMethods[] = {
        {"__newindex", Vector4_newIndex},
        {"__index", Vector4_index},
        {"__unm", Vector4_unm},
        {"__add", Vector4_add},
        {"__sub", Vector4_sub},
        {"__mul", Vector4_mul},
        {"__div", Vector4_div},
        {NULL, NULL}};
    luaL_newmetatable(L, mtVector4Name);
    luaL_setfuncs(L, vector3MetaMethods, 0);
    luaL_newlib(L, vector3Funcs);
    return 1;
}

} // namespace kame::lua
