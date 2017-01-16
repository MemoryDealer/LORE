// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

GLfloat quad_vertices [] = {
    // Positions          // Colors           // Texture Coords
    0.1f,  0.1f,
    0.1f, -0.1f,
    -0.1f, -0.1f,
    -0.1f,  0.1f
};

GLuint quad_indices [] = {
    //0, 1, 2, 3
    1, 0, 2, 3
};

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBuffer::VertexBuffer( const Lore::VertexBuffer::Type& type )
: Lore::VertexBuffer( type )
, _vbo( 0 )
, _vao( 0 )
, _ebo( 0 )
, _vertices()
, _indices()
, _mode( 0 )
, _glType( GL_UNSIGNED_INT )
{
    switch ( type ) {
    default:
    case Type::Custom:
        _mode = GL_TRIANGLES;
        break;

    case Type::Quad:
        _mode = GL_TRIANGLE_STRIP;
        break;

    case Type::Triangle:
        _mode = GL_TRIANGLES;
        break;

    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBuffer::~VertexBuffer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void VertexBuffer::build()
{
    // First generate vertices and indices.
    switch ( _type ) {
    default:
        throw Lore::Exception( "Unknown vertex buffer type" );

    case VertexBuffer::Type::Quad:
        _vertices = { 0.1f, 0.1f,
                      0.1f, -0.1f,
                     -0.1f, -0.1f,
                     -0.1f, 0.1f };
        _indices = { 1, 0, 2, 3 };

        addAttribute( AttributeType::Float, 2 );
        break;

    case VertexBuffer::Type::TexturedQuad:
        _vertices = { 0.1f, 0.1f,      1.f, 1.f,
                      0.1f, -0.1f,     1.f, 0.f,
                     -0.1f, -0.1f,     0.f, 0.f,
                     -0.1f, 0.1f,      0.f, 1.f };
        _indices = { 1, 0, 2, 3 };

        addAttribute( AttributeType::Float, 2 );
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
                               stride * static_cast<GLsizei>( typeSize ),
                               reinterpret_cast< GLvoid* >( offset * typeSize ) );
        glEnableVertexAttribArray( idx++ );

        offset += attr.size;
    }

    glBindVertexArray( 0 );

    _attributes.clear(); // No longer needed.

    //Lore::VertexBuffer::build(); // Why was this here?
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void VertexBuffer::bind()
{
    glBindVertexArray( _vao );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void VertexBuffer::unbind()
{
    glBindVertexArray( 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void VertexBuffer::draw()
{
    glDrawElements( GL_TRIANGLE_STRIP, static_cast<GLsizei>( _indices.size() ), GL_UNSIGNED_INT, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
