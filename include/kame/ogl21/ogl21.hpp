#pragma once

#include "glad.hpp"

#include <kame/math/math.hpp>

#include <vector>

namespace kame::ogl21 {

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

struct Shader {
    GLint id;
    VertexAttribute* attributes;
    GLuint numAttr;
    InputLayout inputLayout;

    void begin();
    void end();
    void setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m);
};

struct VertexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const float* vertices);
};

struct Texture2D {
    GLuint id;
};

struct RenderState {
    bool useBlend = false;
    GLenum srcRGB, srcA, dstRGB, dstA;
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

    RenderState build()
    {
        return state;
    }
};

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void setClearBuffer(GLbitfield mask, kame::math::Vector4f color);
void setRenderState(RenderState state);
void drawArrays(const VertexBuffer* vbo, GLenum mode, GLint first, GLsizei count);

Shader* createShader(const char* vert, const char* frag, InputLayout layout);
void deleteShader(Shader* shader);

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteVertexBuffer(VertexBuffer* vbo);

} // namespace kame::ogl21
