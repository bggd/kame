#pragma once

#include "vector3f.hpp"
#include "vector4f.hpp"

#include <cmath>

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

    static Matrix4x4f Zero()
    {
        return Matrix4x4f(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    static Matrix4x4f Multiply(Matrix4x4f& a, Matrix4x4f& b)
    {
        Matrix4x4f m = Matrix4x4f::Zero();
        const float* A = (const float*)&a;
        const float* B = (const float*)&b;
        float* M = (float*)&m;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int k = 0; k < 4; ++k)
                {
                    M[4 * i + j] += A[4 * i + k] * B[4 * k + j];
                }
            }
        }
        return m;
    }

    static Matrix4x4f createLookAt(Vector3f cameraPosition, Vector3f cameraTarget, Vector3f cameraUp)
    {
        Vector3f cameraDirection = Vector3f::normalize(cameraPosition - cameraTarget);
        Vector3f cameraRight = Vector3f::normalize(Vector3f::cross(cameraUp, cameraDirection));
        cameraUp = Vector3f::cross(cameraDirection, cameraRight);

        Matrix4x4f m(
            cameraRight.x, cameraUp.x, cameraDirection.x, 0.0F,
            cameraRight.y, cameraUp.y, cameraDirection.y, 0.0F,
            cameraRight.z, cameraUp.z, cameraDirection.z, 0.0F,
            -cameraPosition.x, -cameraPosition.y, -cameraPosition.z, 1.0F);
        return m;
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

    // Right Handed, Negative One to One Depth([-1.0, 1.0])
    static Matrix4x4f createPerspectiveFieldOfView_RH_NO(float fovYRadian, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
    {
        float f = 1.0F / tanf(fovYRadian / 2.0F);

        Matrix4x4f m(
            f / aspectRatio, 0.0F, 0.0F, 0.0F,
            0.0F, f, 0.0F, 0.0F,
            0.0F, 0.0F, (farPlaneDistance + nearPlaneDistance) / (nearPlaneDistance - farPlaneDistance), -1.0F,
            0.0F, 0.0F, (2.0F * farPlaneDistance * nearPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0.0F);

        return m;
    }

    static Matrix4x4f createTranslation(Vector3f position)
    {
        return createTranslation(position.x, position.y, position.z);
    }

    static Matrix4x4f createTranslation(float x, float y, float z)
    {
        Matrix4x4f m(
            1.0F, 0.0F, 0.0F, 0.0F,
            0.0F, 1.0F, 0.0F, 0.0F,
            0.0F, 0.0F, 1.0F, 0.0F,
            x, y, z, 1.0F);
        return m;
    };

    static Matrix4x4f createFromAxisAngle(Vector3f axisUnit, float angleRadian)
    {
        float x = axisUnit.x;
        float y = axisUnit.y;
        float z = axisUnit.z;
        float c = std::cosf(angleRadian);
        float s = std::sinf(angleRadian);
        float t = 1.0F - c;

        // clang-format off
        Matrix4x4f m(
            (x * x * t) + c, (y * x * t) + (z * s), (x * z * t) - (y * s), 0.0F,
            (x * y * t) - (z * s), (y * y * t) + c, (y * z * t) + (x * s), 0.0F,
            (x * z * t) + (y * s), (y * z * t) - (x * s), (z * z * t) + c, 0.0F,
            0.0F, 0.0F, 0.0F, 1.0F);
        // clang-format on
        return m;
    }

    static Matrix4x4f createRotationX(float radians)
    {
        return createFromAxisAngle(Vector3f(1.0f, 0.0f, 0.0f), radians);
    }

    static Matrix4x4f createRotationY(float radians)
    {
        return createFromAxisAngle(Vector3f(0.0f, 1.0f, 0.0f), radians);
    }

    static Matrix4x4f createRotationZ(float radians)
    {
        return createFromAxisAngle(Vector3f(0.0f, 0.0f, 1.0f), radians);
    }

    static Matrix4x4f createScale(Vector3f v)
    {
        Matrix4x4f m = Matrix4x4f::Zero();
        m.m11 = v.x;
        m.m22 = v.y;
        m.m33 = v.z;
        m.m44 = 1.0f;
        return m;
    }

    static Matrix4x4f createScale(float scales)
    {
        return createScale(Vector3f(scales));
    }
};

static Matrix4x4f operator*(Matrix4x4f& a, Matrix4x4f& b)
{
    Matrix4x4f m = Matrix4x4f::Zero();
    const float* A = (const float*)&a;
    const float* B = (const float*)&b;
    float* M = (float*)&m;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                M[4 * i + j] += A[4 * i + k] * B[4 * k + j];
            }
        }
    }
    return m;
}

} // namespace kame::math