#include <kame/math/math.hpp>

#include "../glm/glm/glm.hpp"
#include "../glm/glm/ext.hpp"

#include <gtest/gtest.h>

using namespace kame::math;
using namespace kame::math::helper;

TEST(Matrix4x4, Identity)
{
    Matrix4x4f m = Matrix4x4f::Identity();
    glm::mat4 glmMat(1.0f);

    float* M = (float*)&m;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmMat)[i]);
    }
}

TEST(Matrix4x4, TRS)
{
    Matrix4x4f T = Matrix4x4f::createTranslation(Vector3f(3.0f, 4.0f, 5.0f));
    glm::mat4 glmT = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 5.0f));

    float* M = (float*)&T;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmT)[i]);
    }

    Matrix4x4f R = Matrix4x4f::createRotationX(toRadians(90.0f));
    glm::mat4 glmR = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0));

    M = (float*)&R;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmR)[i]);
    }

    Matrix4x4f S = Matrix4x4f::createScale(0.5f);
    glm::mat4 glmS = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    M = (float*)&S;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmS)[i]);
    }

    Matrix4x4f TRS = S * R * T;
    glm::mat4 glmTRS = glmT * glmR * glmS;

    M = (float*)&TRS;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmTRS)[i]);
    }
}

TEST(Matrix4x4, ViewMatrix)
{
    Matrix4x4f View = Matrix4x4f::createLookAt_RH(Vector3f(3.0f, 4.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0));
    glm::mat4 glmView = glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float* M = (float*)&View;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmView)[i]);
    }
}

TEST(Matrix4x4, ProjectionMatrix)
{
    Matrix4x4f Proj = Matrix4x4f::createPerspectiveFieldOfView_RH_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    glm::mat4 glmProj = glm::perspectiveRH_NO(glm::radians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);

    float* M = (float*)&Proj;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmProj)[i]);
    }
}

TEST(Matrix4x4, MVP)
{
    Matrix4x4f View = Matrix4x4f::createLookAt_RH(Vector3f(3.0f, 4.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0));
    Matrix4x4f Proj = Matrix4x4f::createPerspectiveFieldOfView_RH_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    Matrix4x4f MVP = View * Proj;

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
    Matrix4x4f Ortho = Matrix4x4f::createOrthographic_RH_NO(-320.0f, 320.0f, -240.0f, 240.0f, 0.01f, 100.0f);
    glm::mat4 glmOrtho = glm::orthoRH_NO(-320.0f, 320.0f, -240.0f, 240.0f, 0.01f, 100.0f);

    float* M = (float*)&Ortho;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmOrtho)[i]);
    }
}

TEST(Matrix4x4, Transpose)
{
    Matrix4x4f tp = Matrix4x4f::transpose(Matrix4x4f::createLookAt_RH(Vector3f(3.0f, 4.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0)));

    glm::mat4 glmTp = glm::transpose(glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    float* M = (float*)&tp;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmTp)[i]);
    }
}

TEST(Vector3, Transform)
{
    Matrix4x4f View = Matrix4x4f::createLookAt_RH(Vector3f(3.0f, 4.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0));
    Matrix4x4f Proj = Matrix4x4f::createPerspectiveFieldOfView_RH_NO(toRadians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    Matrix4x4f MVP = View * Proj;

    glm::mat4 glmView = glm::lookAtRH(glm::vec3(3.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 glmProj = glm::perspectiveRH_NO(glm::radians(45.0f), 4.0f / 3.0f, 0.001f, 1000.0f);
    glm::mat4 glmMVP = glmProj * glmView;

    Vector3f pos = Vector3f::Zero();
    pos = Vector3f::transform(pos, MVP);

    glm::vec4 glmPos(0.0f, 0.0f, 0.0f, 1.0f);
    glmPos = glmMVP * glmPos;

    EXPECT_FLOAT_EQ(pos.x, glmPos.x);
    EXPECT_FLOAT_EQ(pos.y, glmPos.y);
    EXPECT_FLOAT_EQ(pos.z, glmPos.z);
}
