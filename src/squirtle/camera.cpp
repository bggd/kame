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

void CameraOrbit::update(kame::sdl::State& state, bool isActiveViewport)
{
    int relMouseX = prevMouseX - state.mouseX;
    int relMouseY = prevMouseY - state.mouseY;

    if (isActiveViewport && state.isDownLMB)
    {
        rot.x += sensitivity * float(relMouseY) * -1.0f;
        rot.y += sensitivity * float(relMouseX) * -1.0f;
        modelMtx = kame::math::Matrix::createRotationY(kame::math::helper::toRadians(rot.y)) * kame::math::Matrix::createRotationX(kame::math::helper::toRadians(rot.x));
        modelMtx = modelMtx * kame::math::Matrix::createTranslation(pan.x, pan.y, 0.0f);
    }

    if (isActiveViewport && state.wheelY != 0)
    {
        zoom += float(state.wheelY) * -1.0f;
        if (zoom < 0.0f)
        {
            zoom = 0.0f;
        }
    }

    if (isActiveViewport && state.isDownRMB)
    {
        pan.x += (1.0f - zoom) * 0.5f * 0.005f * float(relMouseX) * -1.0f;
        pan.y += (1.0f - zoom) * 0.5f * 0.005f * float(relMouseY);
        modelMtx = kame::math::Matrix::createRotationY(kame::math::helper::toRadians(rot.y)) * kame::math::Matrix::createRotationX(kame::math::helper::toRadians(rot.x));
        modelMtx = modelMtx * kame::math::Matrix::createTranslation(pan.x, pan.y, 0.0f);
    }

    prevMouseX = state.mouseX;
    prevMouseY = state.mouseY;

    viewMtx = kame::math::Matrix::createLookAt(kame::math::Vector3(0.0f, 0.0f, 2.5f + zoom), kame::math::Vector3::zero(), kame::math::Vector3(0.0f, 1.0f, 0.0f));
    projMtx = kame::math::Matrix::createPerspectiveFieldOfView_NO(verticalFov, float(state.drawableSizeX) / float(state.drawableSizeY), 1.0f, 1000.0f);
}

} // namespace kame::squirtle
