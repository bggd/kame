#include <kame/math/math.hpp>

#include "../thirdparty/glm/glm/glm.hpp"
#include "../thirdparty/glm/glm/ext.hpp"

#include <gtest/gtest.h>

using namespace kame::math;
using namespace kame::math::helper;

TEST(Matrix4x4, Identity)
{
    Matrix4x4f m = Matrix4x4f::identity();
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

TEST(Matrix4x4, Inverse)
{
    Matrix4x4f invIdent = Matrix4x4f::invert(Matrix4x4f::identity());
    glm::mat4 glmInvIdent = glm::inverse(glm::mat4(1.0f));

    float* M = (float*)&invIdent;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmInvIdent)[i]);
    }

    Matrix4x4f inv = Matrix4x4f::invert(Matrix4x4f::createTranslation(Vector3f(3.0f, 4.0f, 5.0f)));
    glm::mat4 glmInv = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 5.0f)));

    M = (float*)&inv;
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(M[i], glm::value_ptr(glmInv)[i]);
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

    Vector3f pos = Vector3f::zero();
    pos = Vector3f::transform(pos, MVP);

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
