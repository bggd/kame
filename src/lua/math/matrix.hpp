#pragma once

#include <all.hpp>

namespace kame::lua {

int openKameMathMatrix(lua_State* L)
{
    static const struct luaL_Reg matrixFuncs[] = {
        {NULL, NULL}};
    static const struct luaL_Reg matrixMetaMethods[] = {
        {NULL, NULL}};
    luaL_newmetatable(L, mtMatrixName);
    luaL_setfuncs(L, matrixMetaMethods, 0);
    luaL_newlib(L, matrixFuncs);
    return 1;
}

} // namespace kame::lua
