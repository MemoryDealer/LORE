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

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBuffer::VertexBuffer( const Lore::VertexBuffer::Type& type )
: Lore::VertexBuffer( type )
, _vbo( 0 )
, _vao( 0 )
, _ebo( 0 )
, _vertices()
, _indices()
{

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
        _vertices = { 0.5f, 0.5f, 0.f,      1.f, 1.f,
                      0.5f, -0.5f, 0.f,     1.f, 0.f,
                      -0.5f, -0.5f, 0.f,    0.f, 0.f,
                      -0.5f, -0.5f, 0.f,    0.f, 1.f };
        _indices = { 0.f, 1.f, 2.f, 3.f }; // FLOAT???
        break;
    }

    glGenVertexArrays( 1, &_vao );
    glGenBuffers( 1, &_vbo );
    glGenBuffers( 1, &_ebo );

    glBindVertexArray( _vao );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( _vertices ), &_vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( _indices ), &_indices, GL_STATIC_DRAW );

    // Attributes.
    uint idx = 0;
    uint offset = 0;
    int stride = 0;

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

        glVertexAttribPointer( idx, attr.size, type, GL_FALSE, stride, reinterpret_cast< GLvoid* >( offset * typeSize ) );
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

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
