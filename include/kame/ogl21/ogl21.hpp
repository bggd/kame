#pragma once

#include "glad.hpp"

#include <kame/math/math.hpp>

#include <vector>

namespace kame::ogl21 {

struct Context {
    int versionMajor, versionMinor;
    bool isAvaliable;

private:
    Context() {}

public:
    static Context& getInstance();
};

struct VertexAttribute {
    const char* name;
    GLuint location;
    GLuint componentSize;
};

struct VertexAttributeBuilder {
    VertexAttribute attr;
    VertexAttributeBuilder& name(const char* s)
    {
        attr.name = s;
        return *this;
    }
    VertexAttributeBuilder& componentSize(GLuint n)
    {
        attr.componentSize = n;
        return *this;
    }
    VertexAttribute build()
    {
        return attr;
    }
};

struct InputLayout {
    std::vector<VertexAttribute> attributes;
    GLsizei stride;

    void add(VertexAttribute attr)
    {
        attributes.push_back(attr);
    }
};

struct VertexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const float* vertices);
};

struct IndexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const unsigned int* vertices);
};

struct Shader {
    GLint id;
    VertexAttribute* attributes;
    GLuint numAttr;
    InputLayout inputLayout;

    void begin();
    void end();
    void setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m);
    void drawArrays(VertexBuffer* vbo, GLenum mode, GLint first, GLsizei count);
    void drawElements(IndexBuffer* ibo, VertexBuffer* vbo, GLenum mode, GLsizei count);
};

struct Texture2D {
    GLuint id;
    int width, height;
    void setTexParameteri(GLenum pname, GLint param);
};

struct RenderState {
    bool useBlend = false;
    bool useDepth = false;
    GLenum srcRGB, srcA, dstRGB, dstA;
    GLenum blendEqRGB, blendEqA;
    GLenum depthFunc;
};

struct RenderStateBuilder {
    RenderState state;

    RenderStateBuilder& blendFunction(GLenum srcRGB, GLenum dstRGB, GLenum srcA, GLenum dstA)
    {
        state.srcRGB = srcRGB;
        state.srcA = srcA;
        state.dstRGB = dstRGB;
        state.dstA = dstA;
        state.useBlend = true;
        return *this;
    }

    RenderStateBuilder& blendEquation(GLenum modeRGB, GLenum modeA) {
        state.blendEqRGB = modeRGB;
        state.blendEqA = modeA;
        return *this;
    }

    RenderStateBuilder& depthFunc(GLenum func) {
        state.depthFunc = func;
        state.useDepth = true;
        return *this;
    }

    RenderState build()
    {
        return state;
    }
};

const char* getGlslVersionString();

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void setClearBuffer(GLbitfield mask, kame::math::Vector4f color);
void setRenderState(RenderState state);
void setTexture2D(GLuint slot, Texture2D* tex);

Shader* createShader(const char* vert, const char* frag, InputLayout layout);
void deleteShader(Shader* shader);

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteVertexBuffer(VertexBuffer* vbo);

IndexBuffer* createIndexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteIndexBuffer(IndexBuffer* ibo);

Texture2D* loadTexture2D(const char* path);
Texture2D* loadTexture2DFromMemory(const unsigned char* src, int len);
void deleteTexture2D(Texture2D* tex);

} // namespace kame::ogl21
