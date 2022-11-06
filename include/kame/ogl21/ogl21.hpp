#pragma once

#include "glad.hpp"

#include <kame/math/math.hpp>

#include <vector>
#include <cassert>

namespace kame::ogl21 {

struct Context {
    int versionMajor, versionMinor;
    bool isAvaliable;

private:
    Context() {}

public:
    static Context& getInstance();
};

struct VertexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const unsigned char* vertices);
    void setBuffer(const float* vertices);
};

struct IndexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const unsigned char* vertices);
    void setBuffer(const unsigned short* vertices);
    void setBuffer(const unsigned int* vertices);
};

struct VertexArrayObject {
    struct Attribute {
        GLuint vbo_id;
        GLuint location;
        GLuint componentSize;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        uintptr_t offset;
    };
    std::vector<VertexArrayObject::Attribute> attributes;
    GLuint ibo_id = 0;
};

struct VertexArrayObjectBuilder {
    VertexArrayObject vao;

    VertexArrayObjectBuilder& bindAttribute(GLuint location, const VertexBuffer* vbo, GLuint componentSize, GLsizei stride, uintptr_t offset);
    VertexArrayObjectBuilder& bindIndexBuffer(const IndexBuffer* ibo);
    VertexArrayObject build() { return vao; }
};

struct Shader {
    GLint id;

    GLuint getAttribLocation(const char* name);
    void setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m, bool transpose);
};

struct Texture2D {
    GLuint id;
    int width, height;
    void setTexParameteri(GLenum pname, GLint param);
};

struct BlendState {
    bool useBlend = false;
    GLenum srcRGB, srcA, dstRGB, dstA;
    GLenum blendEqRGB, blendEqA;
};

struct BlendStateBuilder {
    bool useBlendFn = false;
    bool useBlendEq = false;
    BlendState state;

    BlendStateBuilder& blendFunction(GLenum srcRGB, GLenum dstRGB, GLenum srcA, GLenum dstA)
    {
        state.srcRGB = srcRGB;
        state.srcA = srcA;
        state.dstRGB = dstRGB;
        state.dstA = dstA;
        useBlendFn = true;
        return *this;
    }

    BlendStateBuilder& blendEquation(GLenum modeRGB, GLenum modeA)
    {
        state.blendEqRGB = modeRGB;
        state.blendEqA = modeA;
        useBlendEq = true;
        return *this;
    }

    BlendState build()
    {
        if (useBlendFn && useBlendEq)
        {
            state.useBlend = true;
        }
        else
        {
            assert(!useBlendFn && !useBlendEq);
        }
        return state;
    }
};

struct DepthStencilState {
    bool useDepth = false;
    GLenum depthFunc;
};

struct DepthStencilStateBuilder {
    DepthStencilState state;

    DepthStencilStateBuilder& depthFunc(GLenum func)
    {
        state.depthFunc = func;
        state.useDepth = true;
        return *this;
    }

    DepthStencilState build()
    {
        return state;
    }
};

const char* getGlslVersionString();

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void setClearBuffer(GLbitfield mask, kame::math::Vector4f color);
void setBlendState(BlendState state);
void setDepthStencilState(DepthStencilState state);
void setShader(Shader* shader);
void setTexture2D(GLuint slot, Texture2D* tex);
void drawArrays(const VertexArrayObject& vao, GLenum mode, GLint first, GLsizei count);
void drawElements(const VertexArrayObject& vao, GLenum mode, GLsizei count, GLenum type);

Shader* createShader(const char* vert, const char* frag);
void deleteShader(Shader* shader);

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteVertexBuffer(VertexBuffer* vbo);

IndexBuffer* createIndexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteIndexBuffer(IndexBuffer* ibo);

Texture2D* loadTexture2D(const char* path);
Texture2D* loadTexture2DFromMemory(const unsigned char* src, int len);
Texture2D* createTexture2D(GLint internalFormat, int width, int height, GLenum format, GLenum type);
void deleteTexture2D(Texture2D* tex);

} // namespace kame::ogl21
