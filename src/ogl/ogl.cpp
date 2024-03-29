#include <all.hpp>

namespace kame::ogl {

BlendState BlendStateBuilder::build()
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

Context& Context::getInstance()
{
    static Context ctx;
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
    else if (ctx.versionMajor == 2 && ctx.versionMinor == 1)
    {
        return "#version 120\n";
    }
    else if (ctx.versionMajor == 2 && ctx.versionMinor == 0)
    {
        return "#version 110\n";
    }
    else
    {
        return "";
    }
}

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void setClearState(GLbitfield mask, kame::math::Vector4 color)
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

void setRasterizerState(RasterizerState state)
{
    if (state.useCullFace)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(state.cullMode);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void setShader(kame::ogl::Shader* shader)
{
    glUseProgram(shader->id);
}

void setTexture2D(GLuint slot, Texture2D* tex)
{
    assert(tex);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void setRenderTarget(GBuffer* gbuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->fbo);
}

void setRenderTargetDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void drawElementsInstanced(const VertexArrayObject& vao, GLenum mode, GLsizei count, GLenum type, GLsizei primCount)
{
    for (const auto& i : vao.attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, i.vbo_id);
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, i.type, i.normalized, i.stride, (const void*)i.offset);
        if (i.divisor > 0)
        {
            glVertexAttribDivisor(i.location, i.divisor);
        }
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ibo_id);
    glDrawElementsInstanced(mode, count, type, NULL, primCount);
    // reset divisor
    for (const auto& i : vao.attributes)
    {
        if (i.divisor > 0)
        {
            glVertexAttribDivisor(i.location, 0);
        }
    }
}

VertexArrayObject& VertexArrayObject::begin()
{
    inSetAttributes = true;
    attributes.clear();
    return *this;
}

VertexArrayObject& VertexArrayObject::bindAttribute(const VertexBuffer* vbo, GLuint location, GLuint componentSize, GLenum type, GLboolean normalized, GLsizei stride, uintptr_t offset, GLuint divisor)
{
    assert(inSetAttributes);
    assert(componentSize >= 1 || componentSize <= 4);

    VertexArrayObject::Attribute attr;
    attr.vbo_id = vbo->id;
    attr.location = location;
    attr.componentSize = componentSize;
    attr.type = type;
    attr.normalized = normalized;
    attr.stride = stride;
    attr.offset = offset;
    attr.divisor = divisor;

    attributes.push_back(attr);

    return *this;
}

VertexArrayObject& VertexArrayObject::bindIndexBuffer(const IndexBuffer* ibo)
{
    assert(inSetAttributes);
    ibo_id = ibo->id;
    return *this;
}

void VertexArrayObject::end()
{
    inSetAttributes = false;
}

void VertexArrayObject::drawArrays(GLenum mode, GLint first, GLsizei count)
{
    assert(!inSetAttributes);
    kame::ogl::drawArrays(*this, mode, first, count);
}

void VertexArrayObject::drawElements(GLenum mode, GLsizei count, GLenum type)
{
    assert(!inSetAttributes);
    kame::ogl::drawElements(*this, mode, count, type);
}

void VertexArrayObject::drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizei primCount)
{
    assert(!inSetAttributes);
    kame::ogl::drawElementsInstanced(*this, mode, count, type, primCount);
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
    if (vsCompileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength)
        {
            std::string infoLog(logLength, ' ');
            glGetShaderInfoLog(vs, logLength, NULL, &infoLog[0]);
            SPDLOG_CRITICAL("Vertex shader compilation failed: {0}", infoLog);
        }
    }
    assert(vsCompileStatus == GL_TRUE);

    GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag, NULL);
    glCompileShader(fs);
    GLint fsCompileStatus;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompileStatus);
    if (fsCompileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength)
        {
            std::string infoLog(logLength, ' ');
            glGetShaderInfoLog(fs, logLength, NULL, &infoLog[0]);
            SPDLOG_CRITICAL("Fragment shader compilation failed!\n{0}", infoLog);
        }
    }
    assert(fsCompileStatus == GL_TRUE);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint programLinkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &programLinkStatus);
    if (programLinkStatus == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength)
        {
            std::string infoLog(logLength, ' ');
            glGetProgramInfoLog(program, logLength, NULL, &infoLog[0]);
            SPDLOG_CRITICAL("Shader link failed!\n{0}", infoLog);
        }
    }
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

GLint Shader::getAttribLocation(const char* name)
{
    if (attributeMap.contains(name) == false)
    {
        GLint loc = glGetAttribLocation(id, name);
        attributeMap[name] = loc;
    }
    return attributeMap[name];
}

GLint Shader::getUniformLocation(const char* name)
{
    if (uniformMap.contains(name) == false)
    {
        GLint loc = glGetUniformLocation(id, name);
        uniformMap[name] = loc;
    }
    return uniformMap[name];
}

void Shader::setMatrix(const char* name, const kame::math::Matrix& m, bool transpose)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, (const GLfloat*)&m);
}

void Shader::setVector4(const char* name, kame::math::Vector4 v)
{
    glUniform4fv(getUniformLocation(name), 1, (const GLfloat*)&v);
}

void Shader::setVector3(const char* name, kame::math::Vector3 v)
{
    glUniform3fv(getUniformLocation(name), 1, (const GLfloat*)&v);
}

void Shader::setFloat(const char* name, float x)
{
    glUniform1fv(getUniformLocation(name), 1, (const GLfloat*)&x);
}

void Shader::setInt(const char* name, int x)
{
    glUniform1iv(getUniformLocation(name), 1, (const GLint*)&x);
}

VertexBuffer* createVertexBuffer(GLsizeiptr numBytes, GLenum usage)
{
    VertexBuffer* vbo = new VertexBuffer();
    assert(vbo);

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    assert(buffer);
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
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setBuffer(const float* vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setBuffer(const std::vector<kame::math::Vector2>& vertices)
{
    setBuffer((const float*)vertices.data());
}

void VertexBuffer::setBuffer(const std::vector<kame::math::Vector3>& vertices)
{
    setBuffer((const float*)vertices.data());
}

void VertexBuffer::setBuffer(const std::vector<kame::math::Vector4>& vertices)
{
    setBuffer((const float*)vertices.data());
}

void VertexBuffer::setBuffer(const std::vector<kame::math::Matrix>& vertices)
{
    setBuffer((const float*)vertices.data());
}

void VertexBuffer::setBufferSubData(GLintptr offset, GLsizeiptr size, const float* vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setBufferSubData(GLintptr offset, GLsizeiptr size, const std::vector<kame::math::Vector3>& vertices)
{
    setBufferSubData(offset, size, (const float*)vertices.data());
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
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const unsigned short* vertices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const unsigned int* vertices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const std::vector<unsigned int>& vertices)
{
    setBuffer((const unsigned int*)vertices.data());
}

UniformBuffer* createUniformBuffer(GLsizeiptr numBytes, GLenum usage)
{
    UniformBuffer* ubo = new UniformBuffer();
    assert(ubo);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, numBytes, NULL, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ubo->id = buffer;
    ubo->numBytes = numBytes;
    ubo->usage = usage;
    return ubo;
}

void deleteUniformBuffer(UniformBuffer* ubo)
{
    glDeleteBuffers(1, &ubo->id);
    delete ubo;
}

void UniformBuffer::setBuffer(const float* vertices)
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, numBytes, vertices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Texture2D* loadTexture2D(const char* path, bool flipY)
{
    assert(path);

    int64_t len = 0;
    char* data = kame::squirtle::loadFile(path, len);

    assert(data);

    assert(len <= std::numeric_limits<int>::max());

    return loadTexture2DFromMemory((const unsigned char*)data, len, flipY, path);
}

Texture2D* loadTexture2DFromMemory(const unsigned char* src, int len, bool flipY, const char* path)
{
    Texture2D* t = new Texture2D();
    assert(t);

    int x, y, c;
    stbi_set_flip_vertically_on_load(flipY);
    unsigned char* data = stbi_load_from_memory(src, len, &x, &y, &c, 0);
    if (!data)
    {
        SPDLOG_CRITICAL("{} ({})", path, stbi_failure_reason());
    }
    assert(data);
    assert(c > 2 && c < 5);
    SPDLOG_INFO("{} (width:{}, height:{}, channel:{})", path, x, y, c);

    GLuint tex = 0;
    glGenTextures(1, &tex);
    assert(tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (c == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        t->format = GL_RGB;
    }
    else if (c == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        t->format = GL_RGBA;
    }
    stbi_image_free(data);

    t->id = tex;
    t->width = x;
    t->height = y;
    t->numChannel = c;
    return t;
}

Texture2D* createTexture2D(GLint internalFormat, int width, int height, GLenum format, GLenum type)
{
    Texture2D* t = new Texture2D();
    assert(t);

    GLuint tex = 0;
    glGenTextures(1, &tex);
    assert(tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    t->id = tex;
    t->width = width;
    t->height = height;
    t->format = format;
    switch (format)
    {
        case GL_RGB:
            t->numChannel = 3;
            break;
        case GL_RGBA:
            t->numChannel = 4;
        default:
            t->numChannel = 0; // unknown
            break;
    }

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
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::setTexParameterfv(GLenum pname, const GLfloat* param)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterfv(GL_TEXTURE_2D, pname, param);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::generateMipmap()
{
    glBindTexture(GL_TEXTURE_2D, id);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GBuffer* createGBuffer(int width, int height)
{
    GBuffer* gb = new GBuffer();

    glGenFramebuffers(1, &gb->fbo);
    assert(gb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gb->fbo);

    GLuint rt_0 = 0;
    glGenTextures(1, &rt_0);
    assert(rt_0);
    glBindTexture(GL_TEXTURE_2D, rt_0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_0, 0);
    gb->textures.emplace_back(rt_0, width, height, GL_RGBA, 4);

    GLuint rt_1 = 0;
    glGenTextures(1, &rt_1);
    assert(rt_1);
    glBindTexture(GL_TEXTURE_2D, rt_1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_1, 0);
    gb->textures.emplace_back(rt_1, width, height, GL_RGBA, 4);

    GLuint ds = 0;
    glGenTextures(1, &ds);
    assert(ds);
    glBindTexture(GL_TEXTURE_2D, ds);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ds, 0);
    gb->textures.emplace_back(ds, width, height, GL_DEPTH_COMPONENT, 1);

    GLenum attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_NONE};
    glDrawBuffers(3, attachments);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    return gb;
}

void deleteGBuffer(GBuffer* gb)
{
    assert(gb);

    for (auto& tex : gb->textures)
    {
        glDeleteTextures(1, &tex.id);
    }

    glDeleteFramebuffers(1, &gb->fbo);

    delete gb;
}

} // namespace kame::ogl
