// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
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
  : Lore::VertexBuffer()
  , _vbo( 0 )
  , _vao( 0 )
  , _ebo( 0 )
  , _vertices()
  , _indices()
  , _mode( 0 )
  , _glType( GL_UNSIGNED_INT )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLVertexBuffer::~GLVertexBuffer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::init( const Lore::MeshType& type )
{
  _type = type;
  switch ( _type ) {

  default:
  case MeshType::Custom:
  case MeshType::Text:
    _mode = GL_TRIANGLES;
    break;

  case MeshType::Quad:
  case MeshType::TexturedQuad:
  case MeshType::Background:
    _mode = GL_TRIANGLE_STRIP;
    break;

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::build()
{
  // First generate vertices and indices.
  switch ( _type ) {
  default:
    throw Lore::Exception( "Unknown vertex buffer type" );

  case MeshType::Quad:
    _vertices = { -0.1f, -0.1f,
                  -0.1f, 0.1f,
                  0.1f, -0.1f,
                  0.1f, 0.1f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    break;

  case MeshType::TexturedQuad:
    _vertices = { -0.1f, -0.1f,     0.f, 0.f,
                  -0.1f, 0.1f,      0.f, 1.f,
                  0.1f, -0.1f,      1.f, 0.f,
                  0.1f, 0.1f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case MeshType::Background:
    _vertices = { -1.f, -1.f,     0.f, 0.f,
                  -1.f, 1.f,      0.f, 1.f,
                  1.f, -1.f,      1.f, 0.f,
                  1.f, 1.f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case MeshType::Text:
    // Text VBs are a special case and require dynamic drawing.
    glGenVertexArrays( 1, &_vao );
    glGenBuffers( 1, &_vbo );
    glBindVertexArray( _vao );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    _attributes.clear();
    return; // Early return for special case.
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

void GLVertexBuffer::draw()
{
  glDrawElements( _mode, static_cast< GLsizei >( _indices.size() ), GL_UNSIGNED_INT, 0 );
}

void GLVertexBuffer::draw( const Lore::VertexBuffer::Vertices& verts )
{
  assert( MeshType::Text == _type );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferSubData( GL_ARRAY_BUFFER, 0, verts.size() * sizeof(real), verts.data() );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glDrawArrays( GL_TRIANGLES, 0, 6 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLVertexBuffer::_reset()
{
  // TODO:
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
