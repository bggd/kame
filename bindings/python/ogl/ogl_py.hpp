#pragma once

#include <kame/ogl/ogl.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initOGL(py::module& m)
{
    using VertexBuffer = kame::ogl::VertexBuffer;
    py::class_<VertexBuffer>(m, "VertexBuffer")
        .def("setBuffer", py::overload_cast<const unsigned char*>(&VertexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const float*>(&VertexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const std::vector<kame::math::Vector3>&>(&VertexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const std::vector<kame::math::Vector2>&>(&VertexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const std::vector<kame::math::Matrix>&>(&VertexBuffer::setBuffer))
        .def("setBufferSubData", py::overload_cast<GLintptr, GLsizeiptr, const float*>(&VertexBuffer::setBufferSubData))
        .def("setBufferSubData", py::overload_cast<GLintptr, GLsizeiptr, const std::vector<kame::math::Vector3>&>(&VertexBuffer::setBufferSubData));

    using IndexBuffer = kame::ogl::IndexBuffer;
    py::class_<IndexBuffer>(m, "IndexBuffer")
        .def("setBuffer", py::overload_cast<const unsigned char*>(&IndexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const unsigned short*>(&IndexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const unsigned int*>(&IndexBuffer::setBuffer))
        .def("setBuffer", py::overload_cast<const std::vector<unsigned int>&>(&IndexBuffer::setBuffer));

    using VertexArrayObject = kame::ogl::VertexArrayObject;
    py::class_<VertexArrayObject>(m, "VertexArrayObject")
        .def("begin", &VertexArrayObject::begin)
        .def("bindAttribute", &VertexArrayObject::bindAttribute)
        .def("bindIndexBuffer", &VertexArrayObject::bindIndexBuffer)
        .def("end", &VertexArrayObject::end)
        .def("drawArrays", &VertexArrayObject::drawArrays)
        .def("drawElements", &VertexArrayObject::drawElements)
        .def("drawElementsInstanced", &VertexArrayObject::drawElementsInstanced);

    using Shader = kame::ogl::Shader;
    py::class_<Shader>(m, "ShaderayObject")
        .def("getAttribLocation", &Shader::getAttribLocation)
        .def("getUniformLocation", &Shader::getUniformLocation)
        .def("setMatrix", &Shader::setMatrix)
        .def("setVector4", &Shader::setVector4)
        .def("setVector3", &Shader::setVector3)
        .def("setFloat", &Shader::setFloat)
        .def("setInt", &Shader::setInt);

    using Texture2D = kame::ogl::Texture2D;
    py::class_<Texture2D>(m, "Texture2D")
        .def("setTexParameteri", &Texture2D::setTexParameteri)
        .def("setTexParameterfv", &Texture2D::setTexParameterfv)
        .def("generateMipmap", &Texture2D::generateMipmap);

    using FrameBuffer = kame::ogl::FrameBuffer;
    py::class_<FrameBuffer>(m, "FrameBuffer")
        .def("setColorAttachment", &FrameBuffer::setColorAttachment)
        .def("setDepthAttachment", &FrameBuffer::setDepthAttachment)
        .def("setDepthAttachmentFromRenderBuffer", &FrameBuffer::setDepthAttachmentFromRenderBuffer)
        .def("checkStatus", &FrameBuffer::checkStatus)
        .def("setDrawBuffer", &FrameBuffer::setDrawBuffer);

    using BlendState = kame::ogl::BlendState;
    py::class_<BlendState>(m, "BlendState");

    using BlendStateBuilder = kame::ogl::BlendStateBuilder;
    py::class_<BlendStateBuilder>(m, "BlendStateBuilder")
        .def("blendFunction", &BlendStateBuilder::blendFunction)
        .def("blendEquation", &BlendStateBuilder::blendEquation)
        .def("build", &BlendStateBuilder::build);

    using DepthStencilState = kame::ogl::DepthStencilState;
    py::class_<DepthStencilState>(m, "DepthStencilState");

    using DepthStencilStateBuilder = kame::ogl::DepthStencilStateBuilder;
    py::class_<DepthStencilStateBuilder>(m, "DepthStencilStateBuilder")
        .def("depthFunc", &DepthStencilStateBuilder::depthFunc)
        .def("build", &DepthStencilStateBuilder::build);

    using RasterizerState = kame::ogl::RasterizerState;
    py::class_<RasterizerState>(m, "RasterizerState");

    using RasterizerStateBuilder = kame::ogl::RasterizerStateBuilder;
    py::class_<RasterizerStateBuilder>(m, "RasterizerStateBuilder")
        .def("cullFace", &RasterizerStateBuilder::cullFace)
        .def("build", &RasterizerStateBuilder::build);

    m.def("getGlslVersionString", kame::ogl::getGlslVersionString);

    m.def("setViewport", kame::ogl::setViewport);
    m.def("setClearBuffer", kame::ogl::setClearBuffer);
    m.def("setBlendState", kame::ogl::setBlendState);
    m.def("setDepthStencilState", kame::ogl::setDepthStencilState);
    m.def("setRasterizerState", kame::ogl::setRasterizerState);
    m.def("setShader", kame::ogl::setShader);
    m.def("setTexture2D", kame::ogl::setTexture2D);
    m.def("setRenderTarget", kame::ogl::setRenderTarget);

    m.def("createShader", kame::ogl::createShader);
    m.def("deleteShader", kame::ogl::deleteShader);

    m.def("createVertexBuffer", kame::ogl::createVertexBuffer);
    m.def("deleteVertexBuffer", kame::ogl::deleteVertexBuffer);

    m.def("createIndexBuffer", kame::ogl::createIndexBuffer);
    m.def("deleteIndexBuffer", kame::ogl::deleteIndexBuffer);

    m.def("loadTexture2D", kame::ogl::loadTexture2D);
    m.def("loadTexture2DFromMemory", kame::ogl::loadTexture2DFromMemory);
    m.def("createTexture2D", kame::ogl::createTexture2D);
    m.def("deleteTexture2D", kame::ogl::deleteTexture2D);

    m.def("createFrameBuffer", kame::ogl::createFrameBuffer);
    m.def("deleteFrameBuffer", kame::ogl::deleteFrameBuffer);
}
