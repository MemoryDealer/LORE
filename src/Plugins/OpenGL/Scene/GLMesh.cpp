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

#include "GLMesh.h"

#include <LORE/Resource/ResourceController.h>
#include <LORE/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLMesh::~GLMesh()
{
  glDeleteBuffers( 1, &_vbo );
  glDeleteBuffers( 1, &_vao );
  glDeleteBuffers( 1, &_ebo );
  glDeleteBuffers( 1, &_instancedVBO );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLMesh::init( const Lore::Mesh::Type type )
{
  _type = type;

  // First generate vertices and indices.
  switch ( _type ) {
  default:
    throw Lore::Exception( "Unknown Mesh type" );

  case Mesh::Type::QuadInstanced:
  case Mesh::Type::TexturedQuadInstanced:
    throw Lore::Exception( "Mesh::initInstanced() should be called for instanced types" );
    break;

  case Mesh::Type::Custom:
    _mode = GL_TRIANGLES;
    // Attributes should be added by the caller.
    break;

    //
    // 2D.

  case Mesh::Type::Quad:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -0.1f, -0.1f,
      -0.1f, 0.1f,
      0.1f, -0.1f,
      0.1f, 0.1f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    break;

  case Mesh::Type::TexturedQuad:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -0.1f, -0.1f,     0.f, 0.f,
      -0.1f, 0.1f,      0.f, 1.f,
      0.1f, -0.1f,      1.f, 0.f,
      0.1f, 0.1f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case Mesh::Type::FullscreenQuad:
    _mode = GL_TRIANGLE_STRIP;
    _vertices = { -1.f, -1.f,     0.f, 0.f,
      -1.f, 1.f,      0.f, 1.f,
      1.f, -1.f,      1.f, 0.f,
      1.f, 1.f,       1.f, 1.f };
    _indices = { 0, 1, 2, 3 };

    addAttribute( AttributeType::Float, 2 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case Mesh::Type::Cubemap:
    _mode = GL_TRIANGLES;
    _vertices = {
      -1000.0f,  1000.0f, -1000.0f,
      -1000.0f, -1000.0f, -1000.0f,
      1000.0f, -1000.0f, -1000.0f,
      1000.0f, -1000.0f, -1000.0f,
      1000.0f,  1000.0f, -1000.0f,
      -1000.0f,  1000.0f, -1000.0f,

      -1000.0f, -1000.0f,  1000.0f,
      -1000.0f, -1000.0f, -1000.0f,
      -1000.0f,  1000.0f, -1000.0f,
      -1000.0f,  1000.0f, -1000.0f,
      -1000.0f,  1000.0f,  1000.0f,
      -1000.0f, -1000.0f,  1000.0f,

      1000.0f, -1000.0f, -1000.0f,
      1000.0f, -1000.0f,  1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      1000.0f,  1000.0f, -1000.0f,
      1000.0f, -1000.0f, -1000.0f,

      -1000.0f, -1000.0f,  1000.0f,
      -1000.0f,  1000.0f,  1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      1000.0f, -1000.0f,  1000.0f,
      -1000.0f, -1000.0f,  1000.0f,

      -1000.0f,  1000.0f, -1000.0f,
      1000.0f,  1000.0f, -1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      1000.0f,  1000.0f,  1000.0f,
      -1000.0f,  1000.0f,  1000.0f,
      -1000.0f,  1000.0f, -1000.0f,

      -1000.0f, -1000.0f, -1000.0f,
      -1000.0f, -1000.0f,  1000.0f,
      1000.0f, -1000.0f, -1000.0f,
      1000.0f, -1000.0f, -1000.0f,
      -1000.0f, -1000.0f,  1000.0f,
      1000.0f, -1000.0f,  1000.0f
    };

    addAttribute( AttributeType::Float, 3 );
    break;

  case Mesh::Type::Text:
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

  case Mesh::Type::Quad3D:
    // Generate quad with normals.
    _mode = GL_TRIANGLE_STRIP;
    // TODO: Combine Quad3D and Quad - have 2D renderer use normals etc.
    _vertices = {
      -0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f,
      0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,
      -0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,
      -0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f
    };
    _indices = { 0, 1, 2, 3, 4, 5 };

    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 3 );
    break;

  case Mesh::Type::TexturedQuad3D:
    // Generate quad with normals and texture coordinates.
    _mode = GL_TRIANGLE_STRIP;
    _vertices = {
      -0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
      0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
      0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
      -0.5f,  0.5f, 0.f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
    };
    _indices = { 0, 1, 2, 3, 4, 5 };

    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 2 );
    break;

  case Mesh::Type::Cube:
    _mode = GL_TRIANGLES;
    _vertices = {
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 3 );
    break;

  case Mesh::Type::TexturedCube:
    _mode = GL_TRIANGLES;
    //     _vertices = {
    //       -1.0, -1.0,  1.0,   0.f, 0.f,
    //       1.0, -1.0,  1.0,    1.f, 0.f,
    //       -1.0,  1.0,  1.0,   0.f, 1.f,
    //       1.0,  1.0,  1.0,    1.f, 1.f,
    //       -1.0, -1.0, -1.0,   1.f, 1.f,
    //       1.0, -1.0, -1.0,    0.f, 1.f,
    //       -1.0,  1.0, -1.0,   1.f, 0.f,
    //       1.0,  1.0, -1.0,    0.f, 0.f
    //     };
    //     _indices = { 0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1 };
    //     _vertices = {
    //       -1.f, -1.f, -1.f,   0.f, 0.f,
    //       1.f, -1.f, -1.f,    1.f, 0.f,
    //       1.f, 1.f, -1.f,     1.f, 1.f,
    //       -1.f, 1.f, -1.f,    0.f, 1.f,
    //       -1.f, -1.f, 1.f,    0.f, 1.f,
    //       1.f, -1.f, 1.f,     1.f, 1.f,
    //       1.f, 1.f, 1.f,      1.f, 0.f,
    //       -1.f, 1.f, 1.f,     0.f, 0.f
    //     };
    //     _indices = {
    //       0, 1, 3, 3, 1, 2,
    //       1, 5, 2, 2, 5, 6,
    //       5, 4, 6, 6, 4, 7,
    //       4, 0, 7, 7, 0, 3,
    //       3, 2, 7, 7, 2, 6,
    //       4, 5, 0, 0, 5, 1
    //     };

    _vertices = {
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 3 );
    addAttribute( AttributeType::Float, 2 );
    break;
  }

  glGenVertexArrays( 1, &_vao );
  glGenBuffers( 1, &_vbo );

  glBindVertexArray( _vao );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW );

  if ( !_indices.empty() ) {
    glGenBuffers( 1, &_ebo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * _indices.size(), _indices.data(), GL_STATIC_DRAW );
  }

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

void GLMesh::init( const CustomMeshData& data )
{
  // This is a custom mesh type.
  _type = Mesh::Type::Custom;
  _mode = GL_TRIANGLES;

  glGenVertexArrays( 1, &_vao );
  glGenBuffers( 1, &_vbo );
  glGenBuffers( 1, &_ebo );
  glBindVertexArray( _vao );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * data.verts.size(), data.verts.data(), GL_STATIC_DRAW );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof( uint32_t ), data.indices.data(), GL_STATIC_DRAW );

  // Setup vertex attributes.
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );

  // Vertex normals.
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void* )offsetof( Vertex, normal ) );

  // Texture coordinates.
  glEnableVertexAttribArray( 2 );
  glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void* )offsetof( Vertex, texCoords ) );

  // Vertex tangents.
  glEnableVertexAttribArray( 3 );
  glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void* )offsetof( Vertex, tangent ) );

  // Vertex bitangents.
  glEnableVertexAttribArray( 4 );
  glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void* )offsetof( Vertex, bitangent ) );

  _indices = data.indices;

  glBindVertexArray( 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLMesh::initInstanced( const Type type, const size_t maxCount )
{
  // First generate vertices and indices.
  switch ( type ) {
  default:
    throw Lore::Exception( "Mesh type must be instanced" );

  case Mesh::Type::QuadInstanced:
    init( Type::Quad );
    break;

  case Mesh::Type::TexturedQuadInstanced:
    init( Type::TexturedQuad );
    break;

  case Mesh::Type::Quad3DInstanced:
    init( Type::Quad3D );
    break;

  case Mesh::Type::TexturedQuad3DInstanced:
    init( Type::TexturedQuad3D );
    break;

  case Mesh::Type::CubeInstanced:
    init( Type::Cube );
    break;

  case Mesh::Type::TexturedCubeInstanced:
    init( Type::TexturedCube );
    break;
  }

  // Bind the existing vertex array to add instanced buffer data to it.
  glBindVertexArray( _vao );

  // Generate a new vertex buffer for instanced data.
  glGenBuffers( 1, &_instancedVBO );
  glBindBuffer( GL_ARRAY_BUFFER, _instancedVBO );
  _instancedMatrices.resize( maxCount );
  glBufferData( GL_ARRAY_BUFFER, _instancedMatrices.size() * sizeof( glm::mat4 ), &_instancedMatrices.data()[0], GL_STATIC_DRAW );

  // HACK: 2D instanced matrices must be generated with different attribute indices (???).
  const auto vec4Size = sizeof( glm::vec4 );
  switch ( type ) {
  default:
    // Set the vertex attributes for instanced matrices.
    for ( GLuint attribIdx = 3; attribIdx <= 6; ++attribIdx ) {
      glEnableVertexAttribArray( attribIdx );
      glVertexAttribPointer( attribIdx, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast< void* >( ( attribIdx - 3 ) * vec4Size ) );
      glVertexAttribDivisor( attribIdx, 1 );
    }
    break;

  case Mesh::Type::QuadInstanced:
  case Mesh::Type::TexturedQuadInstanced:
    // Set the vertex attributes for instanced matrices.
    for ( GLuint attribIdx = 2; attribIdx < 6; ++attribIdx ) {
      glEnableVertexAttribArray( attribIdx );
      glVertexAttribPointer( attribIdx, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast< void* >( ( attribIdx - 2 ) * vec4Size ) );
      glVertexAttribDivisor( attribIdx, 1 );
    }
    break;

  }

  glBindVertexArray( 0 );

  _type = type;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLMesh::updateInstanced( const size_t idx, const glm::mat4& matrix )
{
  _instancedMatrices[idx] = matrix;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLMesh::draw( const Lore::GPUProgramPtr program, const size_t instanceCount )
{
  // Bind any textures that are assigned to this mesh.
  // TODO: Sprite animations (e.g., replace 0 with spriteFrame).
  const auto diffuseCount = _sprite.getTextureCount( 0, Texture::Type::Diffuse );
  const auto specularCount = _sprite.getTextureCount( 0, Texture::Type::Specular );
  int textureUnit = 0;
  for ( int i = 0; i < diffuseCount; ++i ) {
    auto texture = _sprite.getTexture( 0, Texture::Type::Diffuse, i );
    texture->bind( textureUnit );
    program->setUniformVar( "diffuseTexture" + std::to_string( i ), textureUnit );
    ++textureUnit;
  }
  for ( int i = 0; i < specularCount; ++i ) {
    auto texture = _sprite.getTexture( 0, Texture::Type::Specular, i );
    texture->bind( textureUnit );
    program->setUniformVar( "specularTexture" + std::to_string( i ), textureUnit );
    ++textureUnit;
  }
  // Set mix values.
  if ( diffuseCount ) {
    for ( int i = 0; i < static_cast<int>( program->getDiffuseSamplerCount() ); ++i ) {
      program->setUniformVar( "diffuseMixValues[" + std::to_string( i ) + "]",
                              _sprite.getMixValue( 0, Texture::Type::Diffuse, i ) );
    }
  }

  glBindVertexArray( _vao );
  switch ( _type ) {
  default:
  case Mesh::Type::Custom:
    glDrawElements( _mode, static_cast< GLsizei >( _indices.size() ), GL_UNSIGNED_INT, nullptr );
    break;

  case Mesh::Type::QuadInstanced:
  case Mesh::Type::TexturedQuadInstanced:
  case Mesh::Type::Quad3DInstanced:
  case Mesh::Type::TexturedQuad3DInstanced:
    glBindBuffer( GL_ARRAY_BUFFER, _instancedVBO );
    glBufferData( GL_ARRAY_BUFFER, _instancedMatrices.size() * sizeof( glm::mat4 ), &_instancedMatrices.data()[0], GL_STATIC_DRAW );
    glDrawElementsInstanced( _mode, static_cast< GLsizei >( _indices.size() ), GL_UNSIGNED_INT, nullptr, static_cast< GLsizei >( instanceCount ) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    break;

  case Mesh::Type::CubeInstanced:
  case Mesh::Type::TexturedCubeInstanced:
    glBindBuffer( GL_ARRAY_BUFFER, _instancedVBO );
    glBufferData( GL_ARRAY_BUFFER, _instancedMatrices.size() * sizeof( glm::mat4 ), &_instancedMatrices.data()[0], GL_STATIC_DRAW );
    glDrawArraysInstanced( _mode, 0, 36, static_cast< GLsizei >( instanceCount ) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    break;

  case Mesh::Type::Quad3D:
  case Mesh::Type::TexturedQuad3D:
    glDrawArrays( _mode, 0, 6 );
    break;

  case Mesh::Type::Cube:
  case Mesh::Type::TexturedCube:
  case Mesh::Type::Cubemap:
    glDrawArrays( _mode, 0, 36 );
    break;
  }
  glBindVertexArray( 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLMesh::draw( const Lore::Vertices& verts )
{
  assert( Mesh::Type::Text == _type );

  glBindVertexArray( _vao );
  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferSubData( GL_ARRAY_BUFFER, 0, verts.size() * sizeof( real ), verts.data() );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glDrawArrays( GL_TRIANGLES, 0, 6 );
  glBindVertexArray( 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
