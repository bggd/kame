#pragma once

#include "glad_header.hpp"

#include <kame/math/math.hpp>

#include <vector>
#include <unordered_map>
#include <string>

namespace kame::ogl {

struct Context {
    struct Capability {
        bool arb_texture_float = false;
        bool arb_draw_instanced = false;
    };

    int versionMajor = 0;
    int versionMinor = 0;
    bool isCoreProfile = false;
    bool isAvaliable = false;
    Capability capability;

private:
    Context() {}

public:
    static Context& getInstance();
    Context(const Context&) = delete;
    void operator=(const Context&) = delete;
};

struct VertexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const unsigned char* vertices);
    void setBuffer(const float* vertices);
    void setBuffer(const std::vector<kame::math::Vector3>& vertices);
    void setBufferSubData(GLintptr offset, GLsizeiptr size, const float* vertices); // glBufferSubData
};

struct IndexBuffer {
    GLuint id;
    GLsizeiptr numBytes;
    GLenum usage;

    void setBuffer(const unsigned char* vertices);
    void setBuffer(const unsigned short* vertices);
    void setBuffer(const unsigned int* vertices);
    void setBuffer(const std::vector<unsigned int>& vertices);
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

    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void drawElements(GLenum mode, GLsizei count, GLenum type);
    void drawElements(GLenum mode, GLsizei count, GLenum type, GLsizei primCount);
};

struct VertexArrayObjectBuilder {
    VertexArrayObject vao;

    VertexArrayObjectBuilder& bindAttribute(GLuint location, const VertexBuffer* vbo, GLuint componentSize, GLsizei stride, uintptr_t offset);
    VertexArrayObjectBuilder& bindIndexBuffer(const IndexBuffer* ibo);
    VertexArrayObject build() { return vao; }
};

struct Shader {
    GLint id;
    std::unordered_map<std::string, GLint> attributeMap;
    std::unordered_map<std::string, GLint> uniformMap;

    GLint getAttribLocation(const char* name);
    GLint getUniformLocation(const char* name);
    void setMatrix(const char* name, const kame::math::Matrix& m, bool transpose = false);
    void setVector4(const char* name, kame::math::Vector4 v);
    void setVector3(const char* name, kame::math::Vector3 v);
    void setFloat(const char* name, float x);
    void setInt(const char* name, int x);
};

struct Texture2D {
    GLuint id;
    int width, height;
    GLenum format;
    int numChannel;
    void setTexParameteri(GLenum pname, GLint param);
    void setTexParameterfv(GLenum pname, const GLfloat* param);
};

struct FrameBuffer {
    GLuint id;
    GLuint depthRBO;
    bool useDepthRBO;

    void setColorAttachment(GLuint index, Texture2D* tex, GLint mipmapLevel = 0);
    void setDepthAttachment(Texture2D* tex, GLint mipmapLevel = 0);
    void setDepthAttachmentFromRenderBuffer(int width, int height);
    bool checkStatus();
    void setDrawBuffer(GLenum mode);
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

    BlendState build();
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

struct RasterizerState {
    bool useCullFace = false;
    GLenum cullMode;
};

struct RasterizerStateBuilder {
    RasterizerState state;

    RasterizerStateBuilder& cullFace(GLenum mode)
    {
        state.cullMode = mode;
        state.useCullFace = true;
        return *this;
    }

    RasterizerState build()
    {
        return state;
    }
};

const char* getGlslVersionString();

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void setClearBuffer(GLbitfield mask, kame::math::Vector4 color);
void setBlendState(BlendState state);
void setDepthStencilState(DepthStencilState state);
void setRasterizerState(RasterizerState state);
void setShader(Shader* shader);
void setTexture2D(GLuint slot, Texture2D* tex);
void setRenderTarget(FrameBuffer* fbo);

Shader* createShader(const char* vert, const char* frag);
void deleteShader(Shader* shader);

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteVertexBuffer(VertexBuffer* vbo);

IndexBuffer* createIndexBuffer(GLsizeiptr numBytes, GLenum usage);
void deleteIndexBuffer(IndexBuffer* ibo);

Texture2D* loadTexture2D(const char* path, bool flipY = false);
Texture2D* loadTexture2DFromMemory(const unsigned char* src, int len, bool flipY = false);
Texture2D* createTexture2D(GLint internalFormat, int width, int height, GLenum format, GLenum type);
void deleteTexture2D(Texture2D* tex);

FrameBuffer* createFrameBuffer();
void deleteFrameBuffer(FrameBuffer* fbo);

} // namespace kame::ogl
