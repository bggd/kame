#include <kame/math/math.hpp>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <gtest/gtest.h>

using namespace kame::math;
using namespace kame::math::helper;

TEST(Matrix4x4, Identity)
{
    Matrix m = Matrix::identity();
    glm::mat4 glmMat(1.0f);

    float* M = (float*)&m;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmMat)[i]);
    }
}

TEST(Matrix4x4, TRS)
{
    Matrix T = Matrix::createTranslation(Vector3(3.0f, 4.0f, 5.0f));
    glm::mat4 glmT = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 5.0f));

    float* M = (float*)&T;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmT)[i]);
    }

    Matrix R = Matrix::createRotationX(toRadians(90.0f));
    glm::mat4 glmR = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0));

    M = (float*)&R;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmR)[i]);
    }

    Matrix S = Matrix::createScale(0.5f);
    glm::mat4 glmS = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    M = (float*)&S;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmS)[i]);
    }

    Matrix TRS = S * R * T;
    glm::mat4 glmTRS = glmT * glmR * glmS;

    M = (float*)&TRS;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmTRS)[i]);
    }
}

TEST(Matrix4x4, ViewMatrix)
{
    Matrix View = Matrix::createLookAt(Vector3(3.0f, 4.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0));
    glm::mat4 glmView = glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float* M = (float*)&View;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmView)[i]);
    }
}

TEST(Matrix4x4, ProjectionMatrix)
{
    Matrix Proj = Matrix::createPerspectiveFieldOfView_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    glm::mat4 glmProj = glm::perspectiveRH_NO(glm::radians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);

    float* M = (float*)&Proj;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmProj)[i]);
    }
}

TEST(Matrix4x4, MVP)
{
    Matrix View = Matrix::createLookAt(Vector3(3.0f, 4.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0));
    Matrix Proj = Matrix::createPerspectiveFieldOfView_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    Matrix MVP = View * Proj;

    glm::mat4 glmView = glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 glmProj = glm::perspectiveRH_NO(glm::radians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    glm::mat4 glmMVP = glmProj * glmView;

    float* M = (float*)&MVP;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmMVP)[i]);
    }
}

TEST(Matrix4x4, OrthoMatrix)
{
    Matrix Ortho = Matrix::createOrthographic_NO(-320.0f, 320.0f, -240.0f, 240.0f, 0.01f, 100.0f);
    glm::mat4 glmOrtho = glm::orthoRH_NO(-320.0f, 320.0f, -240.0f, 240.0f, 0.01f, 100.0f);

    float* M = (float*)&Ortho;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmOrtho)[i]);
    }
}

TEST(Matrix4x4, Transpose)
{
    Matrix tp = Matrix::transpose(Matrix::createLookAt(Vector3(3.0f, 4.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0)));

    glm::mat4 glmTp = glm::transpose(glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    float* M = (float*)&tp;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmTp)[i]);
    }
}

TEST(Matrix4x4, Inverse)
{
    Matrix invIdent = Matrix::invert(Matrix::identity());
    glm::mat4 glmInvIdent = glm::inverse(glm::mat4(1.0f));

    float* M = (float*)&invIdent;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmInvIdent)[i]);
    }

    Matrix inv = Matrix::invert(Matrix::createTranslation(Vector3(3.0f, 4.0f, 5.0f)));
    glm::mat4 glmInv = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 5.0f)));

    M = (float*)&inv;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmInv)[i]);
    }
}

TEST(Vector3, Transform)
{
    Matrix View = Matrix::createLookAt(Vector3(3.0f, 4.0f, 5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0));
    Matrix Proj = Matrix::createPerspectiveFieldOfView_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    Matrix MVP = View * Proj;

    glm::mat4 glmView = glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 glmProj = glm::perspectiveRH_NO(glm::radians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    glm::mat4 glmMVP = glmProj * glmView;

    Vector3 pos = Vector3::zero();
    pos = Vector3::transform(pos, MVP);

    glm::vec4 glmPos(0.0f, 0.0f, 0.0f, 1.0f);
    glmPos = glmMVP * glmPos;

    EXPECT_FLOAT_EQ(pos.x, glmPos.x);
    EXPECT_FLOAT_EQ(pos.y, glmPos.y);
    EXPECT_FLOAT_EQ(pos.z, glmPos.z);
}

TEST(Quaternion, AddSub)
{
    Quaternion q0 = Quaternion(1.0f, 2.0f, 3.0f, 1.0f);
    Quaternion q1 = Quaternion(4.0f, 5.0f, 6.0f, 1.0f);

    glm::quat glmQ0 = glm::quat(1.0f, 2.0f, 3.0f, 1.0f);
    glm::quat glmQ1 = glm::quat(4.0f, 5.0f, 6.0f, 1.0f);

    Quaternion q2 = q0 + q1;
    glm::quat glmQ2 = glmQ0 + glmQ1;

    EXPECT_FLOAT_EQ(q2.x, glmQ2.w);
    EXPECT_FLOAT_EQ(q2.y, glmQ2.x);
    EXPECT_FLOAT_EQ(q2.z, glmQ2.y);
    EXPECT_FLOAT_EQ(q2.w, glmQ2.z);

    q2 = q0 - q1;
    glmQ2 = glmQ0 - glmQ1;

    EXPECT_FLOAT_EQ(q2.x, glmQ2.w);
    EXPECT_FLOAT_EQ(q2.y, glmQ2.x);
    EXPECT_FLOAT_EQ(q2.z, glmQ2.y);
    EXPECT_FLOAT_EQ(q2.w, glmQ2.z);
}

#include <kame/gltf/gltf.hpp>

TEST(Gltf, base64)
{
    std::vector<uint8_t> d = kame::gltf::decodeBase64("TWFu", 0);
    EXPECT_EQ('M', d[0]);
    EXPECT_EQ('a', d[1]);
    EXPECT_EQ('n', d[2]);
    EXPECT_EQ(3, d.size());

    d = kame::gltf::decodeBase64("TWE=", 0);
    EXPECT_EQ('M', d[0]);
    EXPECT_EQ('a', d[1]);
    EXPECT_EQ(2, d.size());

    d = kame::gltf::decodeBase64("TQ==", 0);
    EXPECT_EQ('M', d[0]);
    EXPECT_EQ(1, d.size());
}

#include <kame/lua/lua.hpp>

TEST(Lua, LuaState)
{
    kame::lua::Lua lua;
    lua.initLua();

    lua.doString(R"(
x = 1
y = 'two'
isBool = true
foo = { bar = 'baz'; }
r = { g = { b = { a = 'rgba' } } }
)");
    EXPECT_EQ(lua.getStackSize(), 0);

    EXPECT_TRUE(lua.getGlobal("x") == LUA_TNUMBER);
    int isnum = 0;
    lua_Integer n = lua.popInteger(&isnum);
    EXPECT_TRUE(isnum);
    EXPECT_EQ(n, 1);
    EXPECT_TRUE(lua.getGlobal("y") == LUA_TSTRING);
    n = lua.popInteger(&isnum);
    EXPECT_FALSE(isnum);
    EXPECT_TRUE(lua.getGlobal("isBool") == LUA_TBOOLEAN);
    EXPECT_TRUE(lua.popBoolean());
    EXPECT_TRUE(lua.getGlobal("foo.bar") == LUA_TSTRING);
    EXPECT_EQ(lua.popString(), "baz");
    EXPECT_TRUE(lua.getGlobal("r.g.b.a") == LUA_TSTRING);
    EXPECT_EQ(lua.getStackSize(), 1);

    lua.clearStack();
    EXPECT_EQ(lua.getStackSize(), 0);

    lua.doString(R"(
return 1, 2, 3
)");
    EXPECT_EQ(lua.getStackSize(), 3);

    lua.shutdownLua();
}

TEST(Lua, Vector3)
{
    kame::lua::Lua lua;
    lua.initLua();

    lua.openKameMath();
    lua.openLibs();

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
local v = Vector3.new(1, 2, 3)
v.z = 100.0
return v.x, v.y, v.z
)");
    EXPECT_EQ(lua.getStackSize(), 3);
    float z = lua.popNumber();
    float y = lua.popNumber();
    float x = lua.popNumber();
    EXPECT_EQ(lua.getStackSize(), 0);
    EXPECT_FLOAT_EQ(x, 1.0f);
    EXPECT_FLOAT_EQ(y, 2.0f);
    EXPECT_FLOAT_EQ(z, 100.0f);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
local v = Vector3.new(1, 2, 3)
v.z = 100.0
v = -v
return v.x, v.y, v.z
)");
    z = lua.popNumber();
    y = lua.popNumber();
    x = lua.popNumber();
    EXPECT_FLOAT_EQ(x, -1.0f);
    EXPECT_FLOAT_EQ(y, -2.0f);
    EXPECT_FLOAT_EQ(z, -100.0f);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
local v = Vector3.new(-1.0, 0.0, 1.0)
return v
)");

    kame::math::Vector3 v = lua.popVector3();
    EXPECT_FLOAT_EQ(v.x, -1.0);
    EXPECT_FLOAT_EQ(v.y, 0.0);
    EXPECT_FLOAT_EQ(v.z, 1.0);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
return Vector3.new(1.0) + Vector3.new(1.0)
)");

    v = lua.popVector3();
    EXPECT_FLOAT_EQ(v.x, 2.0);
    EXPECT_FLOAT_EQ(v.y, 2.0);
    EXPECT_FLOAT_EQ(v.z, 2.0);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
return Vector3.new(0.0) - Vector3.new(1.0)
)");

    v = lua.popVector3();
    EXPECT_FLOAT_EQ(v.x, -1.0);
    EXPECT_FLOAT_EQ(v.y, -1.0);
    EXPECT_FLOAT_EQ(v.z, -1.0);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
local v = Vector3.new(2.0) * Vector3.new(5.0)
return v * -1.0
)");

    v = lua.popVector3();
    EXPECT_FLOAT_EQ(v.x, -10.0);
    EXPECT_FLOAT_EQ(v.y, -10.0);
    EXPECT_FLOAT_EQ(v.z, -10.0);

    lua.doString(R"(
local Vector3 = require 'kame.math.Vector3'
return Vector3.new(1.0) / Vector3.new(60.0)
)");

    v = lua.popVector3();
    EXPECT_FLOAT_EQ(v.x, 1.0 / 60.0);
    EXPECT_FLOAT_EQ(v.y, 1.0 / 60.0);
    EXPECT_FLOAT_EQ(v.z, 1.0 / 60.0);

    lua.shutdownLua();
}
