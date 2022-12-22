#include <kame/kame.hpp>

#include "../data/bunny.h"
#include "../data/bunny_diffuse.h"

#include <random>

#define SCREEN_WIDTH 640.0f
#define SCREEN_HEIGHT 480.0f

std::mt19937 randomEngine{std::random_device{}()};

struct AABB {
    kame::math::Vector2f min;
    kame::math::Vector2f max;
};

bool isCollide(const AABB& a, const AABB& b)
{
    return b.min.x <= a.max.x && a.min.x <= b.max.x && b.min.y <= a.max.y && a.min.y <= b.max.y;
}

AABB topWall = {{-32.0f, 24.0f}, {32.0f, 100.0f}};
AABB bottomWall = {{-32.0f, -100.0f}, {32.0f, -24.0f}};
AABB leftWall = {{-100.0f, -24.0f}, {-32.0f, 24.0f}};
AABB rightWall = {{32.0f, -24.0f}, {100.0f, 24.0f}};

struct Bunny : kame::squirtle::Node {

    AABB aabb;
    kame::math::Vector2f speed;
    float halfWidth = 6.13f / 2.0f;
    float halfHeight = 8.7f / 2.0f;

    Bunny()
    {

        std::uniform_real_distribution<float> px{-16.0f + halfWidth, 16.0f - halfWidth};
        std::uniform_real_distribution<float> py{-12.0f + halfHeight, 12.0f - halfHeight};
        kame::math::Vector3f pos;
        pos.x = px(randomEngine);
        pos.y = py(randomEngine);
        pos.z = 0.0f;

        std::uniform_real_distribution<float> spd{-25.0f, 25.0f};
        speed.x = spd(randomEngine) / 60.0f;
        speed.y = spd(randomEngine) / 60.0f;

        setLocation(pos);
    }

    virtual void onUpdate(float dt) override
    {
        kame::math::Vector3f pos = getLocation();
        pos.x += speed.x;
        pos.y += speed.y;

        aabb.min.x = pos.x - halfWidth;
        aabb.min.y = pos.y - halfHeight;
        aabb.max.x = pos.x + halfWidth;
        aabb.max.y = pos.y + halfHeight;

        if (isCollide(aabb, rightWall))
        {
            pos.x = rightWall.min.x - halfWidth;
            aabb.min.x = pos.x - halfWidth;
            aabb.max.x = pos.x + halfWidth;
            speed.x *= -1.0f;
        }
        else if (isCollide(aabb, leftWall))
        {
            pos.x = leftWall.max.x + halfWidth;
            aabb.min.x = pos.x - halfWidth;
            aabb.max.x = pos.x + halfWidth;
            speed.x *= -1.0f;
        }

        if (isCollide(aabb, topWall))
        {
            pos.y = topWall.min.y - halfHeight;
            speed.y *= -1.0f;
        }
        else if (isCollide(aabb, bottomWall))
        {
            pos.y = bottomWall.max.y + halfHeight;
            speed.y *= -1.0f;
        }

        setLocation(pos);
    }
};

void setTexture(kame::squirtle::Node* node, kame::ogl21::Texture2D* diffuse)
{
    if (node->getType() == kSquirtleMeshNode)
    {
        ((kame::squirtle::MeshNode*)node)->diffuse = diffuse;
    }
    for (kame::squirtle::Node* child : node->getChildren())
    {
        setTexture(child, diffuse);
    }
}

struct PerspectiveCamera : kame::squirtle::CameraNode {
    virtual kame::math::Matrix4x4f getViewMatrix() override
    {
        return kame::math::Matrix4x4f::createLookAt_RH(getGlobalLocation(), kame::math::Vector3f::zero(), kame::math::Vector3f(0.0f, 1.0f, 0.0f));
    }
};

kame::squirtle::Node* clone(kame::squirtle::Node* node)
{
    kame::squirtle::Node* newNode = nullptr;
    if (node->getType() == kSquirtleGltfNode)
    {
        newNode = new kame::squirtle::GltfNode();
        *newNode = *(kame::squirtle::GltfNode*)node;
    }
    else if (node->getType() == kSquirtleMeshNode)
    {
        newNode = new kame::squirtle::MeshNode();
        *newNode = *(kame::squirtle::MeshNode*)node;
        ((kame::squirtle::MeshNode*)newNode)->mesh = ((kame::squirtle::MeshNode*)node)->mesh;
        ((kame::squirtle::MeshNode*)newNode)->diffuse = ((kame::squirtle::MeshNode*)node)->diffuse;
        ((kame::squirtle::MeshNode*)newNode)->bufferedVBO.init(((kame::squirtle::MeshNode*)newNode)->mesh);
    }
    else if (node->getType() == kSquirtleNode)
    {
        newNode = new kame::squirtle::Node();
        *newNode = *node;
    }
    assert(newNode);
    newNode->children.clear();

    for (auto* child : node->getChildren())
    {
        newNode->addChild(clone(child));
    }

    return newNode;
}

int main(int argc, char** argv)
{
    kame::kameInit();

    kame::sdl::Window win;
    win.setOgl21DebugMode(true);
    win.setFpsCap(1.0 / 60.0);
    win.openWindow("bunnymark", SCREEN_WIDTH, SCREEN_HEIGHT);
    win.setVsync(true);

    kame::squirtle::Engine engine;
    engine.initSquirtle();

    auto* cam = new PerspectiveCamera();
    cam->fov = 45.0f;
    cam->aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
    cam->nearPlaneDistance = 0.1f;
    cam->farPlaneDistance = 100.0f;
    cam->setLocation(0.0f, 0.0f, 60.0f);

    auto* light = new kame::squirtle::LightNode();
    light->lightType = kSquirtleDirectionalLight;
    // light->diffuse = kame::math::Vector3f(0.5f, 0.5f, 0.5f);
    cam->addChild(light);

    engine.root->addChild(cam);

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTFFromMemory(examples_data_bunny_gltf, examples_data_bunny_gltf_len);
    kame::squirtle::GltfNode* node = kame::squirtle::helper::createGltfNode(gltf);
    kame::gltf::deleteGLTF(gltf);

    auto* tex = kame::ogl21::loadTexture2DFromMemory(examples_data_bunny_diffuse_png, examples_data_bunny_diffuse_png_len);
    setTexture(node, tex);

    for (;;)
    {
        win.update();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        if (state.isDownLMB)
        {
            Bunny* bunny = new Bunny();
            bunny->addChild(clone(node));
            engine.root->addChild(bunny);
        }
        engine.updateNodes(state.deltaTime > 1.0f ? 1.0 / 60.0f : state.deltaTime);
        engine.drawNodes(SCREEN_WIDTH, SCREEN_HEIGHT);
        win.swapWindow();
    }

    kame::ogl21::deleteTexture2D(tex);

    engine.shutdownSqurtile();
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}
