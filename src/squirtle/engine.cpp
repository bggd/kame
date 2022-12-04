#include <all.hpp>

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

void Engine::drawNodes()
{
    kame::ogl21::setShader(shader);

    if (currentCamera)
    {
        auto View = currentCamera->getViewMatrix();
        auto Proj = currentCamera->getPerspectiveMatrix();

        shader->setMatrix4x4f("uModelViewProjection", View * Proj);
    }
    else
    {
        shader->setMatrix4x4f("uModelViewProjection", kame::math::Matrix4x4f::Identity(), true);
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
