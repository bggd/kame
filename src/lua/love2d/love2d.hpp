#include <all.hpp>

namespace kame::lua {

struct LuaApp : kame::love2d::App {
    lua_State* L = nullptr;

    void onCall(const char* funcName)
    {
        if (lua_istable(L, -1))
        {
            if (lua_getfield(L, -1, funcName) != LUA_TNIL)
            {
                lua_call(L, 0, 0);
            }
            else
            {
                lua_pop(L, 1);
            }
        }
    }

    void load() override
    {
        onCall("load");
    }

    void update(float dt) override
    {
        if (lua_istable(L, -1))
        {
            if (lua_getfield(L, -1, "update") != LUA_TNIL)
            {
                lua_pushnumber(L, dt);
                lua_call(L, 1, 0);
            }
            else
            {
                lua_pop(L, 1);
            }
        }
    }

    void draw() override
    {
        onCall("draw");
    }
};

int Love2d_runApp(lua_State* L)
{
    int argc = lua_gettop(L);
    if (argc < 2)
    {
        luaL_error(L, "require 2 argument");
    }

    LuaApp app;
    kame::love2d::Config conf;
    std::string title = "kame";

    if (lua_getfield(L, -1, "window") == LUA_TTABLE)
    {
        lua_getfield(L, -1, "title");
        if (!lua_isnil(L, -1) && lua_isstring(L, -1))
        {
            size_t len;
            const char* str = lua_tolstring(L, -1, &len);
            title = std::string(str, len);
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "width");
        if (!lua_isnil(L, -1) && lua_isinteger(L, -1))
        {
            conf.window.width = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "height");
        if (!lua_isnil(L, -1) && lua_isinteger(L, -1))
        {
            conf.window.height = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }

    lua_pop(L, 2);

    app.L = L;

    kame::love2d::runApp(app, conf);

    return 0;
}

int openKameLove2d(lua_State* L)
{
    static const struct luaL_Reg love2dFuncs[] = {
        {"runApp", Love2d_runApp},
        {NULL, NULL}};
    luaL_newlib(L, love2dFuncs);
    return 1;
}
} // namespace kame::lua
