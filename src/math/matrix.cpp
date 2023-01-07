#include <kame/math/math.hpp>

#include <cfloat>

#define CMP(x, y) \
    (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

kame::math::Matrix kame::math::Matrix::invert(const kame::math::Matrix& m)
{
    // clang-format off
    float determiant
            = m.m11 * m.m22 * m.m33 * m.m44 + m.m11 * m.m23 * m.m34 * m.m42 + m.m11 * m.m24 * m.m32 * m.m43
            + m.m12 * m.m21 * m.m34 * m.m43 + m.m12 * m.m23 * m.m31 * m.m44 + m.m12 * m.m24 * m.m33 * m.m41
            + m.m13 * m.m21 * m.m32 * m.m44 + m.m13 * m.m22 * m.m34 * m.m41 + m.m13 * m.m24 * m.m31 * m.m42
            + m.m14 * m.m21 * m.m33 * m.m42 + m.m14 * m.m22 * m.m31 * m.m43 + m.m14 * m.m23 * m.m32 * m.m41
            - m.m11 * m.m22 * m.m34 * m.m43 - m.m11 * m.m23 * m.m32 * m.m44 - m.m11 * m.m24 * m.m33 * m.m42
            - m.m12 * m.m21 * m.m33 * m.m44 - m.m12 * m.m23 * m.m34 * m.m41 - m.m12 * m.m24 * m.m31 * m.m43
            - m.m13 * m.m21 * m.m34 * m.m42 - m.m13 * m.m22 * m.m31 * m.m44 - m.m13 * m.m24 * m.m32 * m.m41
            - m.m14 * m.m21 * m.m32 * m.m43 - m.m14 * m.m22 * m.m33 * m.m41 - m.m14 * m.m23 * m.m31 * m.m42;
    // clang-format on

    if (CMP(determiant, 0.0f))
    {
        return kame::math::Matrix::identity();
    }

    float i_det = 1.0f / determiant;

    // clang-format off
    kame::math::Matrix result;
        result.m11 = (m.m22 * m.m33 * m.m44 + m.m23 * m.m34 * m.m42 + m.m24 * m.m32 * m.m43 - m.m22 * m.m34 * m.m43 - m.m23 * m.m32 * m.m44 - m.m24 * m.m33 * m.m42) * i_det;
        result.m12 = (m.m12 * m.m34 * m.m43 + m.m13 * m.m32 * m.m44 + m.m14 * m.m33 * m.m42 - m.m12 * m.m33 * m.m44 - m.m13 * m.m34 * m.m42 - m.m14 * m.m32 * m.m43) * i_det;
        result.m13 = (m.m12 * m.m23 * m.m44 + m.m13 * m.m24 * m.m42 + m.m14 * m.m22 * m.m43 - m.m12 * m.m24 * m.m43 - m.m13 * m.m22 * m.m44 - m.m14 * m.m23 * m.m42) * i_det;
        result.m14 = (m.m12 * m.m24 * m.m33 + m.m13 * m.m22 * m.m34 + m.m14 * m.m23 * m.m32 - m.m12 * m.m23 * m.m34 - m.m13 * m.m24 * m.m32 - m.m14 * m.m22 * m.m33) * i_det;
        result.m21 = (m.m21 * m.m34 * m.m43 + m.m23 * m.m31 * m.m44 + m.m24 * m.m33 * m.m41 - m.m21 * m.m33 * m.m44 - m.m23 * m.m34 * m.m41 - m.m24 * m.m31 * m.m43) * i_det;
        result.m22 = (m.m11 * m.m33 * m.m44 + m.m13 * m.m34 * m.m41 + m.m14 * m.m31 * m.m43 - m.m11 * m.m34 * m.m43 - m.m13 * m.m31 * m.m44 - m.m14 * m.m33 * m.m41) * i_det;
        result.m23 = (m.m11 * m.m24 * m.m43 + m.m13 * m.m21 * m.m44 + m.m14 * m.m23 * m.m41 - m.m11 * m.m23 * m.m44 - m.m13 * m.m24 * m.m41 - m.m14 * m.m21 * m.m43) * i_det;
        result.m24 = (m.m11 * m.m23 * m.m34 + m.m13 * m.m24 * m.m31 + m.m14 * m.m21 * m.m33 - m.m11 * m.m24 * m.m33 - m.m13 * m.m21 * m.m34 - m.m14 * m.m23 * m.m31) * i_det;
        result.m31 = (m.m21 * m.m32 * m.m44 + m.m22 * m.m34 * m.m41 + m.m24 * m.m31 * m.m42 - m.m21 * m.m34 * m.m42 - m.m22 * m.m31 * m.m44 - m.m24 * m.m32 * m.m41) * i_det;
        result.m32 = (m.m11 * m.m34 * m.m42 + m.m12 * m.m31 * m.m44 + m.m14 * m.m32 * m.m41 - m.m11 * m.m32 * m.m44 - m.m12 * m.m34 * m.m41 - m.m14 * m.m31 * m.m42) * i_det;
        result.m33 = (m.m11 * m.m22 * m.m44 + m.m12 * m.m24 * m.m41 + m.m14 * m.m21 * m.m42 - m.m11 * m.m24 * m.m42 - m.m12 * m.m21 * m.m44 - m.m14 * m.m22 * m.m41) * i_det;
        result.m34 = (m.m11 * m.m24 * m.m32 + m.m12 * m.m21 * m.m34 + m.m14 * m.m22 * m.m31 - m.m11 * m.m22 * m.m34 - m.m12 * m.m24 * m.m31 - m.m14 * m.m21 * m.m32) * i_det;
        result.m41 = (m.m21 * m.m33 * m.m42 + m.m22 * m.m31 * m.m43 + m.m23 * m.m32 * m.m41 - m.m21 * m.m32 * m.m43 - m.m22 * m.m33 * m.m41 - m.m23 * m.m31 * m.m42) * i_det;
        result.m42 = (m.m11 * m.m32 * m.m43 + m.m12 * m.m33 * m.m41 + m.m13 * m.m31 * m.m42 - m.m11 * m.m33 * m.m42 - m.m12 * m.m31 * m.m43 - m.m13 * m.m32 * m.m41) * i_det;
        result.m43 = (m.m11 * m.m23 * m.m42 + m.m12 * m.m21 * m.m43 + m.m13 * m.m22 * m.m41 - m.m11 * m.m22 * m.m43 - m.m12 * m.m23 * m.m41 - m.m13 * m.m21 * m.m42) * i_det;
        result.m44 = (m.m11 * m.m22 * m.m33 + m.m12 * m.m23 * m.m31 + m.m13 * m.m21 * m.m32 - m.m11 * m.m23 * m.m32 - m.m12 * m.m21 * m.m33 - m.m13 * m.m22 * m.m31) * i_det;
    // clang-format on

    return result;
}
