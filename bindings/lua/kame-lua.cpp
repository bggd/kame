#include <kame/lua/lua.hpp>

#if _WIN32
#define KAME_LUA_API __declspec(dllexport)
#else
#define KAME_LUA_API
#endif

extern "C" {

KAME_LUA_API int luaopen_kame_math_Vector3(lua_State* L)
{
    return kame::lua::openKameMathVector3(L);
}

KAME_LUA_API int luaopen_kame_math_Vector4(lua_State* L)
{
    return kame::lua::openKameMathVector4(L);
}

KAME_LUA_API int luaopen_kame_math_Matrix(lua_State* L)
{
    return kame::lua::openKameMathMatrix(L);
}

KAME_LUA_API int luaopen_kame_love2d(lua_State* L)
{
    return kame::lua::openKameLove2d(L);
}

} // extern "C"
