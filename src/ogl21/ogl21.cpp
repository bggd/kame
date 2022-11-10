#include <all.hpp>

namespace kame::ogl21 {

Context& Context::getInstance()
{
    static Context ctx;
    ctx.isAvaliable = false;
    return ctx;
}

const char* getGlslVersionString()
{
    auto& ctx = Context::getInstance();
    if (ctx.versionMajor == 3 && ctx.versionMinor == 0)
    {
        return "#version 130\n";
    }
    else if (ctx.versionMajor == 3 && ctx.versionMinor == 1)
    {
        return "#version 140\n";
    }
    else if (ctx.versionMajor == 3 && ctx.versionMinor == 2)
    {
        return "#version 150\n";
    }
    else if (ctx.versionMajor == 3 && ctx.versionMinor == 3)
    {
        return "#version 330\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 0)
    {
        return "#version 400\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 1)
    {
        return "#version 410\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 2)
    {
        return "#version 420\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 3)
    {
        return "#version 430\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 4)
    {
        return "#version 440\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 5)
    {
        return "#version 450\n";
    }
    else if (ctx.versionMajor == 4 && ctx.versionMinor == 6)
    {
        return "#version 460\n";
    }
    else
    {
        return "#version 120\n";
    }
}

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void setClearBuffer(GLbitfield mask, kame::math::Vector4f color)
{
    glClear(mask);
    glClearColor(color.x, color.y, color.z, color.w);
}

void setBlendState(BlendState state)
{
    if (state.useBlend)
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(state.srcRGB, state.dstRGB, state.srcA, state.dstA);
        glBlendEquationSeparate(state.blendEqRGB, state.blendEqA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void setDepthStencilState(DepthStencilState state)
{
    if (state.useDepth)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(state.depthFunc);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void setShader(Shader* shader)
{
    glUseProgram(shader->id);
}

void setTexture2D(GLuint slot, Texture2D* tex)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void drawArrays(const VertexArrayObject& vao, GLenum mode, GLint first, GLsizei count)
{
    for (const auto& i : vao.attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, i.vbo_id);
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, i.type, i.normalized, i.stride, (const void*)i.offset);
    }
    glDrawArrays(mode, first, count);
}

void drawElements(const VertexArrayObject& vao, GLenum mode, GLsizei count, GLenum type)
{
    for (const auto& i : vao.attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, i.vbo_id);
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, i.type, i.normalized, i.stride, (const void*)i.offset);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ibo_id);
    glDrawElements(mode, count, type, NULL);
}

Shader* createShader(const char* vert, const char* frag)
{
    Shader* s = new Shader();
    assert(s);

    GLint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert, NULL);
    glCompileShader(vs);
    GLint vsCompileStatus;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vsCompileStatus);
    assert(vsCompileStatus == GL_TRUE);

    GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag, NULL);
    glCompileShader(fs);
    GLint fsCompileStatus;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompileStatus);
    assert(fsCompileStatus == GL_TRUE);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint programLinkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &programLinkStatus);
    assert(programLinkStatus == GL_TRUE);

    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    s->id = program;
    return s;
}

void deleteShader(Shader* shader)
{
    glDeleteProgram(shader->id);
    delete shader;
}

GLuint Shader::getAttribLocation(const char* name)
{
    GLint loc = glGetAttribLocation(id, name);
    return (GLuint)loc;
}

void Shader::setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m, bool transpose)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose ? GL_TRUE : GL_FALSE, (const GLfloat*)&m);
}

VertexArrayObjectBuilder& VertexArrayObjectBuilder::bindAttribute(GLuint location, const VertexBuffer* vbo, GLuint componentSize, GLsizei stride, uintptr_t offset)
{
    assert(componentSize >= 1 || componentSize <= 4);

    VertexArrayObject::Attribute attr;
    attr.location = location;
    attr.vbo_id = vbo->id;
    attr.componentSize = componentSize;
    attr.type = GL_FLOAT;
    attr.normalized = GL_FALSE;
    attr.stride = stride;
    attr.offset = offset;

    vao.attributes.push_back(attr);

    return *this;
}

VertexArrayObjectBuilder& VertexArrayObjectBuilder::bindIndexBuffer(const IndexBuffer* ibo)
{
    vao.ibo_id = ibo->id;
    return *this;
}

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage)
{
    VertexBuffer* vbo = new VertexBuffer();
    assert(vbo);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, numBytes, NULL, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vbo->id = buffer;
    vbo->numBytes = numBytes;
    vbo->usage = usage;
    return vbo;
}

void deleteVertexBuffer(VertexBuffer* vbo)
{
    glDeleteBuffers(1, &vbo->id);
    delete vbo;
}

void VertexBuffer::setBuffer(const unsigned char* vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, numBytes, NULL, usage); // orphaning
    glBufferData(GL_ARRAY_BUFFER, numBytes, vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setBuffer(const float* vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, numBytes, NULL, usage); // orphaning
    glBufferData(GL_ARRAY_BUFFER, numBytes, vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer* createIndexBuffer(GLsizeiptr numBytes, GLenum usage)
{
    IndexBuffer* ibo = new IndexBuffer();
    assert(ibo);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, NULL, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    ibo->id = buffer;
    ibo->numBytes = numBytes;
    ibo->usage = usage;
    return ibo;
}
void deleteIndexBuffer(IndexBuffer* ibo)
{
    glDeleteBuffers(1, &ibo->id);
    delete ibo;
}

void IndexBuffer::setBuffer(const unsigned char* vertices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, NULL, usage); // orphaning
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, vertices, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const unsigned short* vertices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, NULL, usage); // orphaning
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, vertices, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const unsigned int* vertices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, NULL, usage); // orphaning
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, vertices, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Texture2D* loadTexture2D(const char* path)
{
    Texture2D* t = new Texture2D();
    assert(t);

    int x, y, c;
    unsigned char* data = stbi_load(path, &x, &y, &c, 0);
    SPDLOG_INFO("loadTexture2D: {0}(width:{1}, height:{2}, channel:{3})", path, x, y, c);
    assert(data);
    assert(c > 2 && c < 5);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (c == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (c == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    stbi_image_free(data);

    t->id = tex;
    t->width = x;
    t->height = y;
    return t;
}

Texture2D* loadTexture2DFromMemory(const unsigned char* src, int len)
{
    Texture2D* t = new Texture2D();
    assert(t);

    int x, y, c;
    unsigned char* data = stbi_load_from_memory(src, len, &x, &y, &c, 4);
    SPDLOG_INFO("loadTexture2DFromMemory: (width:{0}, height:{1}, channel:{2})", x, y, c);
    assert(data);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    t->id = tex;
    t->width = x;
    t->height = y;
    return t;
}

Texture2D* createTexture2D(GLint internalFormat, int width, int height, GLenum format, GLenum type)
{
    Texture2D* t = new Texture2D();
    assert(t);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

    t->id = tex;
    t->width = width;
    t->height = height;
    return t;
}

void deleteTexture2D(Texture2D* tex)
{
    glDeleteTextures(1, &tex->id);
    delete tex;
}

void Texture2D::setTexParameteri(GLenum pname, GLint param)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
}

FrameBuffer* createFrameBuffer()
{
    FrameBuffer* fbo = new FrameBuffer();
    assert(fbo);

    GLuint buffer;
    glGenFramebuffers(1, &buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fbo->id = buffer;
    fbo->depthRBO = 0;
    return fbo;
}

void deleteFrameBuffer(FrameBuffer* fbo)
{
    if (fbo->useDepthRBO)
    {
        glDeleteRenderbuffers(1, &fbo->depthRBO);
    }
    glDeleteFramebuffers(1, &fbo->id);
    delete fbo;
}

void FrameBuffer::setColorAttachment(GLuint index, Texture2D* tex, GLint mipmapLevel = 0)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, tex->id, mipmapLevel);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::setDepthAttachmentFromRenderBuffer(int width, int height)
{
    assert(useDepthRBO == false);

    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    useDepthRBO = true;
}

bool FrameBuffer::checkStatus()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return status == GL_FRAMEBUFFER_COMPLETE;
}

} // namespace kame::ogl21
