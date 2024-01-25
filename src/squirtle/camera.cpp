#include <all.hpp>

// radians on calc
static float fovH2V(float H, float width, float height)
{
    float V = 2.0f * atanf(tanf(H / 2.0f) * (height / width));
    return V;
}

namespace kame::squirtle {

CameraOrbit::CameraOrbit(float horizontalFov, float width, float height)
{
    targetHorizontalFov = horizontalFov;
    targetWidth = width;
    targetHeight = height;
    verticalFov = fovH2V(targetHorizontalFov, targetWidth, targetHeight);

    modelMtx = kame::math::Matrix::identity();
    viewMtx = kame::math::Matrix::identity();
    projMtx = kame::math::Matrix::identity();
}

void CameraOrbit::update(kame::sdl::State& state)
{
    int relMouseX = prevMouseX - state.mouseX;
    int relMouseY = prevMouseY - state.mouseY;

    if (state.isDownLMB)
    {
        rot.x += sensitivity * float(relMouseY) * -1.0f;
        rot.y += sensitivity * float(relMouseX) * -1.0f;
        modelMtx = kame::math::Matrix::createRotationY(kame::math::helper::toRadians(rot.y)) * kame::math::Matrix::createRotationX(kame::math::helper::toRadians(rot.x));
        modelMtx = modelMtx * kame::math::Matrix::createTranslation(pan.x, pan.y, 0.0f);
    }

    if (state.wheelY != 0)
    {
        zoom += float(state.wheelY) * -1.0f * 0.5f;
        if (zoom < 0.0f)
        {
            zoom = 0.0f;
        }
    }

    if (state.isDownRMB)
    {
        if (zoom > 0.0f)
        {
            pan.x += zoom * 0.5f * 0.005f * float(relMouseX) * -1.0f;
            pan.y += zoom * 0.5f * 0.005f * float(relMouseY);
        }
        else
        {
            pan.x += 0.5f * 0.005f * float(relMouseX) * -1.0f;
            pan.y += 0.5f * 0.005f * float(relMouseY);
        }
        modelMtx = kame::math::Matrix::createRotationY(kame::math::helper::toRadians(rot.y)) * kame::math::Matrix::createRotationX(kame::math::helper::toRadians(rot.x));
        modelMtx = modelMtx * kame::math::Matrix::createTranslation(pan.x, pan.y, 0.0f);
    }

    prevMouseX = state.mouseX;
    prevMouseY = state.mouseY;

    viewMtx = kame::math::Matrix::createLookAt(kame::math::Vector3(0.0f, 0.0f, 1.0f + zoom), kame::math::Vector3::zero(), kame::math::Vector3(0.0f, 1.0f, 0.0f));
    projMtx = kame::math::Matrix::createPerspectiveFieldOfView_NO(verticalFov, float(state.drawableSizeX) / float(state.drawableSizeY), 0.1f, 100.0f);
}

} // namespace kame::squirtle
