#include <all.hpp>

#define KAME_SQUIRTLE_MAX_LIGHTS 8

namespace kame::squirtle {

// clang-format off
static const char* vertexGlslText =
#include "shaders/vertex.glsl"
;
static const char* pixelGlslText =
#include "shaders/pixel.glsl"
;
// clang-format on

void Engine::initSquirtle()
{
    std::string vs = kame::ogl21::getGlslVersionString();
    vs += vertexGlslText;
    std::string fs = kame::ogl21::getGlslVersionString();
    fs += fmt::format("#define MAX_LIGHTS {0}\n", KAME_SQUIRTLE_MAX_LIGHTS);
    fs += pixelGlslText;
    shader = kame::ogl21::createShader(vs.c_str(), fs.c_str());

    root = new kame::squirtle::Node();
    assert(root);
}

void Engine::shutdownSqurtile()
{
    kame::ogl21::deleteShader(shader);
}

void updateNodeRecusrive(kame::squirtle::Engine* engine, kame::squirtle::Node* node, float dt)
{
    if (!engine->currentCamera && node->getType() == kSquirtleCameraNode)
    {
        engine->currentCamera = (kame::squirtle::CameraNode*)node;
    }
    node->onUpdate(dt);
    for (kame::squirtle::Node* child : node->getChildren())
    {
        updateNodeRecusrive(engine, child, dt);
    }
}

void updateGlobalTransformRecursive(kame::squirtle::Node* node)
{
    kame::math::Matrix4x4f local = node->getLocalTransform();
    kame::math::Matrix4x4f global = kame::math::Matrix4x4f::Identity();
    if (node->parent)
    {
        global = node->parent->getGlobalTransform();
    }
    node->setGlobalTransform(local * global);

    for (kame::squirtle::Node* c : node->children)
    {
        updateGlobalTransformRecursive(c);
    }
}

void Engine::updateNodes(float dt)
{
    updateNodeRecusrive(this, root, dt);
    updateGlobalTransformRecursive(root);
}

void drawNodeRecursive(kame::ogl21::Shader* shader, kame::squirtle::Node* node)
{
    if (node->getType() == kSquirtleMeshNode)
    {
        kame::squirtle::MeshNode* meshNode = (kame::squirtle::MeshNode*)node;
        meshNode->bufferedVBO.updateVBO(meshNode->mesh);

        kame::squirtle::VBO& vbo = meshNode->bufferedVBO.getVBO();
        if (!vbo.vaoIsCreated)
        {
            vbo.vao = kame::ogl21::VertexArrayObjectBuilder()
                          .bindAttribute(shader->getAttribLocation("vPos"), vbo.vboPositions, 3, 3 * sizeof(float), 0)
                          .bindAttribute(shader->getAttribLocation("vUV"), vbo.vboTexCoords, 2, 2 * sizeof(float), 0)
                          .bindAttribute(shader->getAttribLocation("vNormal"), vbo.vboNormals, 3, 3 * sizeof(float), 0)
                          .bindIndexBuffer(vbo.ibo)
                          .build();
        }
        kame::ogl21::setTexture2D(0, meshNode->diffuse);
        kame::ogl21::drawElements(vbo.vao, GL_TRIANGLES, meshNode->mesh->indices.size(), GL_UNSIGNED_INT);

        meshNode->bufferedVBO.swap();
    }
    for (kame::squirtle::Node* child : node->getChildren())
    {
        drawNodeRecursive(shader, child);
    }
}

void collectLightNodeRecursive(std::vector<kame::squirtle::LightNode>& lights, kame::squirtle::Node* node)
{
    if (node->getType() == kSquirtleLightNode)
    {
        kame::squirtle::LightNode* lightNode = (kame::squirtle::LightNode*)node;
        lights.push_back(*lightNode);
    }

    for (kame::squirtle::Node* child : node->getChildren())
    {
        collectLightNodeRecursive(lights, child);
    }
}

void Engine::drawNodes()
{
    lights.clear();
    collectLightNodeRecursive(lights, root);

    kame::ogl21::setShader(shader);

    if (currentCamera)
    {
        auto View = currentCamera->getViewMatrix();
        auto Proj = currentCamera->getPerspectiveMatrix();

        shader->setMatrix4x4f("uModel", kame::math::Matrix4x4f::Identity());
        shader->setMatrix4x4f("uView", View);
        shader->setMatrix4x4f("uProjection", Proj);
        shader->setVector3f("uEyePos", currentCamera->getGlobalLocation());
    }
    else
    {
        shader->setMatrix4x4f("uModel", kame::math::Matrix4x4f::Identity());
        shader->setMatrix4x4f("uView", kame::math::Matrix4x4f::Identity());
        shader->setMatrix4x4f("uProjection", kame::math::Matrix4x4f::Identity());
        shader->setVector3f("uEyePos", kame::math::Vector3f::Zero());
    }

    for (int i = 0; i < KAME_SQUIRTLE_MAX_LIGHTS; ++i)
    {
        kame::math::Vector4f position = kame::math::Vector4f::Zero();
        kame::math::Vector3f ambient = kame::math::Vector3f::Zero();
        kame::math::Vector3f diffuse = kame::math::Vector3f::Zero();
        float constantAttenuation = 0.0f;
        float linearAttenuation = 0.0f;
        float quadraticAttenuation = 0.0f;

        if (i < lights.size())
        {
            auto light = lights[i];
            if (light.getLightType() == kSquirtlePointLight)
            {
                kame::math::Vector3f lightPos = light.getGlobalLocation();
                position.x = lightPos.x;
                position.y = lightPos.y;
                position.z = lightPos.z;
                position.w = 1.0f;
                constantAttenuation = light.constantAttenuation;
                linearAttenuation = light.linearAttenuation;
                quadraticAttenuation = light.quadraticAttenuation;
            }
            else
            {
                kame::math::Vector3f lightDir = light.direction;
                position.x = lightDir.x;
                position.y = lightDir.y;
                position.z = lightDir.z;
                position.w = 0.0f;
            }
            diffuse = light.diffuse;
        }

        shader->setVector4f(fmt::format("uLights[{0}].position", i).c_str(), position);
        shader->setVector3f(fmt::format("uLights[{0}].ambient", i).c_str(), ambient);
        shader->setVector3f(fmt::format("uLights[{0}].diffuse", i).c_str(), diffuse);

        shader->setFloat(fmt::format("uLights[{0}].constantAttenuation", i).c_str(), constantAttenuation);
        shader->setFloat(fmt::format("uLights[{0}].linearAttenuation", i).c_str(), linearAttenuation);
        shader->setFloat(fmt::format("uLights[{0}].quadraticAttenuation", i).c_str(), quadraticAttenuation);
    }

    kame::ogl21::BlendState blendState = kame::ogl21::BlendStateBuilder()
                                             .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                             .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                             .build();
    kame::ogl21::DepthStencilState depthState = kame::ogl21::DepthStencilStateBuilder()
                                                    .depthFunc(GL_LESS)
                                                    .build();
    kame::ogl21::setBlendState(blendState);
    kame::ogl21::setDepthStencilState(depthState);

    drawNodeRecursive(shader, root);
}

} // namespace kame::squirtle
