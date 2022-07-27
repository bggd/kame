#include <all.hpp>

namespace kame::ogl21 {

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
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void setTexture2D(GLuint slot, Texture2D* tex)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

Shader* createShader(const char* vert, const char* frag, InputLayout layout)
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

    GLsizei stride = 0;
    for (auto& i : layout.attributes)
    {
        i.location = glGetAttribLocation(program, i.name);
        stride += i.componentSize * sizeof(float);
    }

    s->id = program;
    s->inputLayout = layout;
    s->inputLayout.stride = stride;
    return s;
}

void deleteShader(Shader* shader)
{
    glDeleteProgram(shader->id);
    delete shader;
    shader = nullptr;
}

void Shader::begin()
{
    glUseProgram(id);
}

void Shader::end()
{
    glUseProgram(0);
}

void Shader::setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, (const GLfloat*)&m);
}

void Shader::drawArrays(VertexBuffer* vbo, GLenum mode, GLint first, GLsizei count)
{
    assert(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    uint32_t offset = 0;
    for (const auto& i : inputLayout.attributes)
    {
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, GL_FLOAT, GL_FALSE, inputLayout.stride, (const void*)(uintptr_t)offset);
        offset += i.componentSize * sizeof(float);
    }
    glDrawArrays(mode, first, count);
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
    assert(c == 4);

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
