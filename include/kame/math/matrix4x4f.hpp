#pragma once

#include "vector4f.hpp"

namespace kame::math {

struct Matrix4x4f {

    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;

    Matrix4x4f() {}
    Matrix4x4f(Vector4f row1, Vector4f row2, Vector4f row3, Vector4f row4)
    {
        Matrix4x4f(row1.x, row1.y, row1.y, row1.z, row2.x, row2.y, row2.y, row2.z, row3.x, row3.y, row3.y, row3.z, row4.x, row4.y, row4.y, row4.z);
    }
    Matrix4x4f(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
    {
        this->m11 = m11;
        this->m12 = m12;
        this->m13 = m13;
        this->m14 = m14;
        this->m21 = m21;
        this->m22 = m22;
        this->m23 = m23;
        this->m24 = m24;
        this->m31 = m31;
        this->m32 = m32;
        this->m33 = m33;
        this->m34 = m34;
        this->m41 = m41;
        this->m42 = m42;
        this->m43 = m43;
        this->m44 = m44;
    }

    // Right Handed, Negative One to One Depth([-1.0, 1.0])
    static Matrix4x4f createOrthographic_RH_NO(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
    {
        float tX = -((right + left) / (right - left));
        float tY = -((top + bottom) / (top - bottom));
        float tZ = -((zFarPlane + zNearPlane) / (zFarPlane - zNearPlane));

        Matrix4x4f m(
            2.0F / (right - left), 0.0F, 0.0F, 0.0F,
            0.0F, 2.0F / (top - bottom), 0.0F, 0.0F,
            0.0F, 0.0F, -2.0F / (zFarPlane - zNearPlane), 0.0F,
            tX, tY, tZ, 1.0F);

        return m;
    }
};

static Matrix4x4f operator*(Matrix4x4f& a, Matrix4x4f& b)
{
    Matrix4x4f m;
    const float* A = (const float*)&a;
    const float* B = (const float*)&b;
    float* M = (float*)&m;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++i)
            {
                M[4 * i + j] += A[4 * i + k] * B[4 * k + j];
            }
        }
    }
    return m;
}

} // namespace kame::math