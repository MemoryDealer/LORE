#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016-2017 Jordan Sparks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files ( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE/Scene/Mesh.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class GLMesh : public Lore::Mesh,
                 public Alloc<GLMesh>
  {

  public:

    GLMesh() = default;
    ~GLMesh() override;

    void init( const Type type ) override;
    void init( const CustomMeshData& data ) override;
    void initInstanced( const Type type, const size_t maxCount ) override;

    void updateInstanced( const size_t idx, const glm::mat4& matrix ) override;

    void draw( const GPUProgramPtr program, const size_t instanceCount ) override;
    void draw( const Vertices& verts ) override;

  private:

    GLuint _vbo { 0 }; // Vertex buffer object.
    GLuint _vao { 0 }; // Vertex array object.
    GLuint _ebo { 0 }; // Element buffer object.

    GLuint _instancedVBO { 0 };
    std::vector<glm::mat4> _instancedMatrices { };

    std::vector<GLfloat> _vertices { };
    std::vector<GLuint> _indices { };

    GLenum _mode { GL_TRIANGLE_STRIP };
    GLenum _glType { GL_UNSIGNED_INT };

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
