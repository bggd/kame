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

static const char* shadowVertexGlslText =
#include "shaders/shadow-vertex.glsl"
;
static const char* shadowPixelGlslText =
#include "shaders/shadow-pixel.glsl"
;
// clang-format on

#define KAME_SQUIRTLE_SHADOW_SIZE 2048

void Engine::initSquirtle()
{
    std::string vs = kame::ogl21::getGlslVersionString();
    vs += vertexGlslText;
    std::string fs = kame::ogl21::getGlslVersionString();
    fs += fmt::format("#define MAX_LIGHTS {0}\n", KAME_SQUIRTLE_MAX_LIGHTS);
    fs += pixelGlslText;
    shader = kame::ogl21::createShader(vs.c_str(), fs.c_str());

    kame::ogl21::setShader(shader);
    shader->setInt("diffuseTexture", 0);

    std::string vsShadow = kame::ogl21::getGlslVersionString();
    vsShadow += shadowVertexGlslText;
    std::string fsShadow = kame::ogl21::getGlslVersionString();
    fsShadow += shadowPixelGlslText;
    shaderShadow = kame::ogl21::createShader(vsShadow.c_str(), fsShadow.c_str());

    root = new kame::squirtle::Node();
    assert(root);
}

void Engine::shutdownSqurtile()
{
    kame::ogl21::deleteShader(shaderShadow);
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
    kame::math::Matrix4x4f global = kame::math::Matrix4x4f::identity();
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

void drawNodeRecursive(Engine* engine, kame::squirtle::Node* node, bool isShadowPass)
{
    if (node->getType() == kSquirtleMeshNode)
    {
        kame::squirtle::MeshNode* meshNode = (kame::squirtle::MeshNode*)node;
        meshNode->bufferedVBO.updateVBO(meshNode->mesh);

        kame::squirtle::VBO& vbo = meshNode->bufferedVBO.getVBO();
        if (!vbo.vaoIsCreated)
        {
            vbo.vao = kame::ogl21::VertexArrayObjectBuilder()
                          .bindAttribute(engine->shader->getAttribLocation("vPos"), vbo.vboPositions, 3, 3 * sizeof(float), 0)
                          .bindAttribute(engine->shader->getAttribLocation("vUV"), vbo.vboTexCoords, 2, 2 * sizeof(float), 0)
                          .bindAttribute(engine->shader->getAttribLocation("vNormal"), vbo.vboNormals, 3, 3 * sizeof(float), 0)
                          .bindIndexBuffer(vbo.ibo)
                          .build();
        }
        if (isShadowPass)
        {
        }
        else
        {
            kame::ogl21::setTexture2D(0, meshNode->diffuse);
            vbo.vao.drawElements(GL_TRIANGLES, meshNode->mesh->indices.size(), GL_UNSIGNED_INT);
            meshNode->bufferedVBO.swap();
        }
    }
    for (kame::squirtle::Node* child : node->getChildren())
    {
        drawNodeRecursive(engine, child, isShadowPass);
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

void Engine::drawNodes(int viewportWidth, int viewportHeight)
{
    lights.clear();
    collectLightNodeRecursive(lights, root);

    // lighting pass
    kame::ogl21::setShader(shader);

    if (currentCamera)
    {
        auto Model = kame::math::Matrix4x4f::identity();
        auto View = currentCamera->getViewMatrix();
        auto Proj = currentCamera->getPerspectiveMatrix();

        shader->setMatrix4x4f("uModel", Model);
        shader->setMatrix4x4f("uView", View);
        shader->setMatrix4x4f("uProjection", Proj);
        Model.m14 = 0.0f;
        Model.m24 = 0.0f;
        Model.m34 = 0.0f;
        Model.m41 = 0.0f;
        Model.m42 = 0.0f;
        Model.m43 = 0.0f;
        shader->setMatrix4x4f("uInvModel", kame::math::Matrix4x4f::transpose(kame::math::Matrix4x4f::invert(Model)));
        shader->setVector3f("uEyePos", currentCamera->getGlobalLocation());
    }
    else
    {
        shader->setMatrix4x4f("uModel", kame::math::Matrix4x4f::identity());
        shader->setMatrix4x4f("uView", kame::math::Matrix4x4f::identity());
        shader->setMatrix4x4f("uProjection", kame::math::Matrix4x4f::identity());
        shader->setMatrix4x4f("uInvModel", kame::math::Matrix4x4f::identity());
        shader->setVector3f("uEyePos", kame::math::Vector3f::zero());
    }

    for (int i = 0; i < KAME_SQUIRTLE_MAX_LIGHTS; ++i)
    {
        kame::math::Vector4f position = kame::math::Vector4f::zero();
        kame::math::Vector3f ambient = kame::math::Vector3f::zero();
        kame::math::Vector3f diffuse = kame::math::Vector3f::zero();
        float constantAttenuation = 0.0f;
        float linearAttenuation = 0.0f;
        float quadraticAttenuation = 0.0f;

        if (i < lights.size())
        {
            auto light = lights[i];
            kame::math::Vector3f lightPos = light.getGlobalLocation();
            if (light.getLightType() == kSquirtlePointLight)
            {
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
                position.x = lightDir.x - lightPos.x;
                position.y = lightDir.y - lightPos.y;
                position.z = lightDir.z - lightPos.z;
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

    kame::ogl21::setViewport(0, 0, viewportWidth, viewportHeight);
    kame::ogl21::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, kame::math::Vector4f(0.5, 0.5, 0.5, 1));
    kame::ogl21::RasterizerState rasterState = kame::ogl21::RasterizerStateBuilder().cullFace(GL_BACK).build();
    kame::ogl21::setRasterizerState(rasterState);
    kame::ogl21::BlendState blendState = kame::ogl21::BlendStateBuilder()
                                             .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                             .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                             .build();
    kame::ogl21::DepthStencilState depthState = kame::ogl21::DepthStencilStateBuilder()
                                                    .depthFunc(GL_LESS)
                                                    .build();
    kame::ogl21::setBlendState(blendState);
    kame::ogl21::setDepthStencilState(depthState);

    drawNodeRecursive(this, root, false);
}

} // namespace kame::squirtle
