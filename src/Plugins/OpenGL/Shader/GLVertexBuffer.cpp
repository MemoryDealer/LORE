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

#include "GLVertexBuffer.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLVertexBuffer::GLVertexBuffer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLVertexBuffer::~GLVertexBuffer()
{
  glDeleteBuffers( 1, &_vbo );
  glDeleteBuffers( 1, &_vao );
  glDeleteBuffers( 1, &_ebo );
  glDeleteBuffers( 1, &_instancedVBO );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::init( const Lore::VertexBuffer::Type& type )
{
  _type = type;

  // First generate vertices and indices.
  switch ( _type ) {
  default:
    throw Lore::Exception( "Unknown vertex buffer type" );

  case VertexBuffer::Type::QuadInstanced:
  case VertexBuffer::Type::TexturedQuadInstanced:
    throw Lore::Exception( "VertexBuffer::initInstanced() should be called for instanced types" );
    break;

  case VertexBuffer::Type::Custom:
    _mode = GL_TRIANGLES;
    // Attributes should be added by the caller.
    break;

  //
  // 2D.

  case VertexBuffer::Type::Quad:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -0.1f, -0.1f,
                  -0.1f, 0.1f,
                  0.1f, -0.1f,
                  0.1f, 0.1f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    break;

  case VertexBuffer::Type::TexturedQuad:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -0.1f, -0.1f,     0.f, 0.f,
                  -0.1f, 0.1f,      0.f, 1.f,
                  0.1f, -0.1f,      1.f, 0.f,
                  0.1f, 0.1f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case VertexBuffer::Type::Skybox2D:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -1.f, -1.f,     0.f, 0.f,
                  -1.f, 1.f,      0.f, 1.f,
                  1.f, -1.f,      1.f, 0.f,
                  1.f, 1.f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case VertexBuffer::Type::Text:
    _mode = GL_TRIANGLES;
    // Text VBs are a special case and require dynamic drawing.
    glGenVertexArrays( 1, &_vao );
    glGenBuffers( 1, &_vbo );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), nullptr );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    _attributes.clear();
    return; // Early return for special case.

  //
  // 3D.

  case VertexBuffer::Type::Cube:
    // TODO: Add rest of vertices.
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -0.1f, -0.1f,
      -0.1f, 0.1f,
      0.1f, -0.1f,
      0.1f, 0.1f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    break;
  }

  glGenVertexArrays( 1, &_vao );
  glGenBuffers( 1, &_vbo );
  glGenBuffers( 1, &_ebo );

  glBindVertexArray( _vao );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * _indices.size(), _indices.data(), GL_STATIC_DRAW );

  // Attributes.
  GLsizei idx = 0;
  GLsizei offset = 0;
  GLsizei stride = 0;

  // Get stride value from all attributes.
  for ( const auto& attr : _attributes ) {
    stride += attr.size;
  }

  // Build each attribute.
  for ( const auto& attr : _attributes ) {
    GLenum type = 0;
    size_t typeSize = 0;
    switch ( attr.type ) {
    default:
    case AttributeType::Float:
      type = GL_FLOAT;
      typeSize = sizeof( GLfloat );
      break;

    case AttributeType::Int:
      type = GL_INT;
      typeSize = sizeof( GLint );
      break;

    }

    glVertexAttribPointer( idx,
                           attr.size,
                           type,
                           GL_FALSE,
                           stride * static_cast< GLsizei >( typeSize ),
                           reinterpret_cast< GLvoid* >( offset * typeSize ) );
    glEnableVertexAttribArray( idx++ );

    offset += attr.size;
  }

  glBindVertexArray( 0 );

  _attributes.clear(); // Attributes no longer needed.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::initInstanced( const Type& type, const size_t maxCount )
{
  // First generate vertices and indices.
  switch ( type ) {
  default:
    throw Lore::Exception( "VertexBuffer type must be instanced" );

  case VertexBuffer::Type::QuadInstanced:
    init( Type::Quad );
    break;

  case VertexBuffer::Type::TexturedQuadInstanced:
    init( Type::TexturedQuad );
    break;
  }

  // Bind the existing vertex array to add instanced buffer data to it.
  glBindVertexArray( _vao );

  // Generate a new vertex buffer for instanced data.
  glGenBuffers( 1, &_instancedVBO );
  glBindBuffer( GL_ARRAY_BUFFER, _instancedVBO );
  _instancedMatrices.resize( maxCount );
  glBufferData( GL_ARRAY_BUFFER, _instancedMatrices.size() * sizeof( glm::mat4 ), &_instancedMatrices.data()[0], GL_STATIC_DRAW );

  // Set the vertex attributes for instanced matrices.
  for ( GLuint attribIdx = 2; attribIdx < 6; ++attribIdx ) {
    glEnableVertexAttribArray( attribIdx );
    glVertexAttribPointer( attribIdx, 4, GL_FLOAT, GL_FALSE, sizeof( glm::mat4 ), reinterpret_cast< void* >( ( attribIdx - 2 ) * sizeof( glm::vec4 ) ) );
    glVertexAttribDivisor( attribIdx, 1 );
  }

  glBindVertexArray( 0 );

  _type = type;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::updateInstanced( const size_t idx, const glm::mat4& matrix )
{
  _instancedMatrices[idx] = matrix;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::bind()
{
  glBindVertexArray( _vao );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::unbind()
{
  glBindVertexArray( 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::draw( const size_t instanceCount )
{
  switch ( _type ) {
  default:
    glDrawElements( _mode, static_cast< GLsizei >( _indices.size() ), GL_UNSIGNED_INT, nullptr );
    break;

  case VertexBuffer::Type::QuadInstanced:
  case VertexBuffer::Type::TexturedQuadInstanced:
    glBindBuffer( GL_ARRAY_BUFFER, _instancedVBO );
    glBufferData( GL_ARRAY_BUFFER, _instancedMatrices.size() * sizeof( glm::mat4 ), &_instancedMatrices.data()[0], GL_STATIC_DRAW );
    glDrawElementsInstanced( _mode, static_cast< GLsizei >( _indices.size() ), GL_UNSIGNED_INT, nullptr, static_cast< GLsizei >( instanceCount ) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::draw( const Lore::VertexBuffer::Vertices& verts )
{
  assert( VertexBuffer::Type::Text == _type );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferSubData( GL_ARRAY_BUFFER, 0, verts.size() * sizeof(real), verts.data() );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glDrawArrays( GL_TRIANGLES, 0, 6 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
