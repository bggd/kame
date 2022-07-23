#include <all.hpp>

namespace kame::ogl21 {

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

void drawArrays(const VertexBuffer* vbo, GLenum mode, GLint first, GLsizei count)
{
    assert(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    glDrawArrays(mode, first, count);
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
    GLsizei offset = 0;
    for (const auto& i : inputLayout.attributes)
    {
        glEnableVertexAttribArray(i.location);
        glVertexAttribPointer(i.location, i.componentSize, GL_FLOAT, GL_FALSE, inputLayout.stride, (const void*)offset);
        offset += i.componentSize * sizeof(float);
    }
}

void Shader::end()
{
    glUseProgram(0);
    for (const auto& i : inputLayout.attributes)
    {
        glDisableVertexAttribArray(i.location);
    }
}

void Shader::setMatrix4x4f(const char* name, const kame::math::Matrix4x4f& m)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, (const GLfloat*)&m);
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

} // namespace kame::ogl21
