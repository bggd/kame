#pragma once

#include "vector3f.hpp"
#include "vector4f.hpp"
#include "quaternion.hpp"

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

    static Matrix4x4f zero()
    {
        return Matrix4x4f(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    static Matrix4x4f identity()
    {
        return Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4x4f multiply(Matrix4x4f& a, Matrix4x4f& b)
    {
        Matrix4x4f m = Matrix4x4f::zero();
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

    static Matrix4x4f createLookAt_RH(Vector3f cameraPosition, Vector3f cameraTarget, Vector3f cameraUp)
    {
        Vector3f f = Vector3f::normalize(cameraTarget - cameraPosition);
        Vector3f s = Vector3f::normalize(Vector3f::cross(f, cameraUp));
        Vector3f u = Vector3f::cross(s, f);
        Matrix4x4f m(s.x, u.x, -f.x, 0.0f,
                     s.y, u.y, -f.y, 0.0f,
                     s.z, u.z, -f.z, 0.0f,
                     -Vector3f::dot(s, cameraPosition), -Vector3f::dot(u, cameraPosition), Vector3f::dot(f, cameraPosition), 1.0f);
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
        float a = 1.0f / tanf(fovYRadian / 2.0f);

        Matrix4x4f m(a / aspectRatio, 0.0f, 0.0f, 0.0f,
                     0.0f, a, 0.0f, 0.0f,
                     0.0f, 0.0f, -((farPlaneDistance + nearPlaneDistance) / (farPlaneDistance - nearPlaneDistance)), -1.0f,
                     0.0f, 0.0f, -((2.0f * farPlaneDistance * nearPlaneDistance) / (farPlaneDistance - nearPlaneDistance)), 0.0f);
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
        Matrix4x4f m = Matrix4x4f::zero();
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

    static Matrix4x4f CreateFromQuaternion(Quaternion q)
    {
        Vector3f r = Vector3f::transform(Vector3f(1.0f, 0.0f, 0.0f), q);
        Vector3f u = Vector3f::transform(Vector3f(0.0f, 1.0f, 0.0f), q);
        Vector3f f = Vector3f::transform(Vector3f(0.0f, 0.0f, 1.0f), q);
        return {r.x, r.y, r.z, 0.0f,
                u.x, u.y, u.z, 0.0f,
                f.x, f.y, f.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f};
    }

    static Matrix4x4f transpose(const Matrix4x4f& m)
    {
        Matrix4x4f ret;
        ret.m11 = m.m11;
        ret.m12 = m.m21;
        ret.m13 = m.m31;
        ret.m14 = m.m41;

        ret.m21 = m.m12;
        ret.m22 = m.m22;
        ret.m23 = m.m32;
        ret.m24 = m.m42;

        ret.m31 = m.m13;
        ret.m32 = m.m23;
        ret.m33 = m.m33;
        ret.m34 = m.m43;

        ret.m41 = m.m14;
        ret.m42 = m.m24;
        ret.m43 = m.m34;
        ret.m44 = m.m44;
        return ret;
    }

    static Matrix4x4f invert(const Matrix4x4f& m);
};

static Matrix4x4f operator*(Matrix4x4f& a, Matrix4x4f& b)
{
    Matrix4x4f m = Matrix4x4f::zero();
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
