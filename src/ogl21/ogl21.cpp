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

void setRenderState(RenderState state)
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

void drawElements(const VertexArrayObject& vao, GLenum mode, GLsizei count)
{
    for (const auto& i : vao.attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, i.vbo_id);
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, i.type, i.normalized, i.stride, (const void*)i.offset);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ibo_id);
    glDrawElements(mode, count, GL_UNSIGNED_INT, NULL);
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
    shader = nullptr;
}

GLuint Shader::getAttribLocation(const char* name)
{
    GLint loc = glGetAttribLocation(id, name);
    return (GLuint)loc;
}

void Shader::setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_TRUE, (const GLfloat*)&m);
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
    vbo = nullptr;
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
    ibo = nullptr;
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

void deleteTexture2D(Texture2D* tex)
{
    glDeleteTextures(1, &tex->id);
    tex = nullptr;
}

void Texture2D::setTexParameteri(GLenum pname, GLint param)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
}

} // namespace kame::ogl21
