#include <kame/kame.hpp>
#include <vector>
#include <cstdio>

#include "../common/common.hpp"

const char* vertGLSL = R"(#version 330
in vec2 vUV;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(vUV.x, vUV.y, 0.0, 1.0);
}
)";

const char* fragGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.278431, 0.447059, 0.701961, 0.25);
}
)";

const char* flippedUVGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    if (gl_FrontFacing) {
        discard;
    }
    else {
        fragColor = vec4(0.701961, 0.278431, 0.278431, 0.25);
    }
}
)";

const char* drawLinesGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

const char* drawPointsGLSL = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0);
}
)";

using namespace kame::math;

void drawUVSet(kame::ogl::Shader* shader, const std::vector<Mesh>& meshes, const std::vector<VBOMesh>& vboMeshes, GLenum mode)
{
    kame::ogl::setShader(shader);
    GLint loc = shader->getAttribLocation("vUV");
    int idx = 0;
    for (auto& m : meshes)
    {
        auto& vbo = vboMeshes[idx];
        auto vao = kame::ogl::VertexArrayObjectBuilder()
                       .bindAttribute(loc, vbo.vboUVSets[0], 2, 2 * sizeof(float), 0)
                       .bindIndexBuffer(vbo.iboIndices)
                       .build();
        vao.drawElements(mode, m.indices.size(), GL_UNSIGNED_INT);
        ++idx;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "require *.gltf\n");
        return 1;
    }

    kame::kameInit();

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.forceGLVersion(3, 3, false);
    win.openWindow("uvview", 512, 512);
    win.setVsync(true);

    kame::ogl::Shader* shaderFrontFace = kame::ogl::createShader(vertGLSL, fragGLSL);
    kame::ogl::Shader* shaderBackFace = kame::ogl::createShader(vertGLSL, flippedUVGLSL);
    kame::ogl::Shader* shaderDrawLines = kame::ogl::createShader(vertGLSL, drawLinesGLSL);
    kame::ogl::Shader* shaderDrawPoints = kame::ogl::createShader(vertGLSL, drawPointsGLSL);

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(argv[1]);
    int numUVSet = getNumberOfUVSet(gltf);
    fprintf(stdout, "Number Of UV Set: %d\n", numUVSet);
    if (numUVSet < 1)
    {
        fprintf(stderr, "require TEXCOORD_* in gltf\n");
        return 1;
    }
    std::vector<Mesh> meshes;
    std::vector<VBOMesh> vboMeshes;
    meshes.reserve(gltf->meshes.size());
    vboMeshes.reserve(gltf->meshes.size());
    for (auto& m : gltf->meshes)
    {
        meshes.emplace_back();
        auto& mesh = meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);

        vboMeshes.emplace_back();
        auto& vbo = vboMeshes.back();
        vbo.initVBOMesh(mesh);
    }
    kame::gltf::deleteGLTF(gltf);

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        kame::ogl::setViewport(0, 0, state.drawableSizeX, state.drawableSizeY);
        kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, Vector4(0.5, 0.5, 0.5, 1));
        kame::ogl::RasterizerState rasterState = kame::ogl::RasterizerStateBuilder().build();
        kame::ogl::setRasterizerState(rasterState);
        kame::ogl::BlendState blendState = kame::ogl::BlendStateBuilder()
                                               .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                               .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                               .build();
        kame::ogl::setBlendState(blendState);

        auto view = Matrix::createLookAt(Vector3(0.0f, 0.0f, 0.5f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        auto proj = Matrix::createOrthographic_NO(0.0f, 1.0f, 1.0f, 0.0f, 0.1f, 10.0f);

        kame::ogl::setShader(shaderFrontFace);
        shaderFrontFace->setMatrix("uMVP", view * proj);
        drawUVSet(shaderFrontFace, meshes, vboMeshes, GL_TRIANGLES);

        kame::ogl::setShader(shaderBackFace);
        shaderBackFace->setMatrix("uMVP", view * proj);
        drawUVSet(shaderBackFace, meshes, vboMeshes, GL_TRIANGLES);

        kame::ogl::setShader(shaderDrawLines);
        shaderDrawLines->setMatrix("uMVP", view * proj);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawUVSet(shaderDrawLines, meshes, vboMeshes, GL_TRIANGLES);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        kame::ogl::setShader(shaderDrawPoints);
        shaderDrawPoints->setMatrix("uMVP", view * proj);
        drawUVSet(shaderDrawPoints, meshes, vboMeshes, GL_POINTS);

        win.swapWindow();
    }

    cleanupVBOMeshes(vboMeshes);
    kame::ogl::deleteShader(shaderDrawPoints);
    kame::ogl::deleteShader(shaderDrawLines);
    kame::ogl::deleteShader(shaderBackFace);
    kame::ogl::deleteShader(shaderFrontFace);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}
