#include <all.hpp>

kame::math::Quaternionf kame::math::Quaternionf::slerp(kame::math::Quaternionf a, kame::math::Quaternionf b, float amount)
{
    float dotProduct = kame::math::Quaternionf::dot(a, b);

    if (dotProduct < 0.0f)
    {
        b = -b;
        dotProduct = -dotProduct;
    }

    if (dotProduct > 0.995f)
    {
        return kame::math::Quaternionf::normalize(a + ((b - a) * amount));
    }

    float theta0 = acosf(dotProduct);
    float theta = amount * theta0;
    float sin_theta = sinf(theta);
    float sin_theta0 = sinf(theta0);

    float scaleA = cosf(theta) - dotProduct * sin_theta / sin_theta0;
    float scaleB = sin_theta / sin_theta0;

    return (a * scaleA) + (b * scaleB);
}
