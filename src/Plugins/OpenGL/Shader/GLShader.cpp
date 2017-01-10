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

#include "GLShader.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Shader::Shader( const string& name, const Shader::Type& type )
: Lore::Shader( name, type )
, _shader( 0 )
{
    GLenum shaderType = 0;
    switch ( type ) {
    default:
        log_error( "Unknown shader type requested for Shader " + _name );
        return;

    case Shader::Type::Vertex:
        shaderType = GL_VERTEX_SHADER;
        break;

    case Shader::Type::Fragment:
        shaderType = GL_FRAGMENT_SHADER;
        break;
    }

    _shader = glCreateShader( shaderType );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Shader::~Shader()
{
    unload();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Shader::loadFromFile( const string& file )
{

    return false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Shader::loadFromSource( const string& source )
{
    const char* psrc = source.c_str();

    glShaderSource( _shader, 1, &psrc, nullptr );
    glCompileShader( _shader );

    GLint success;
    GLchar buf[512];
    glGetShaderiv( _shader, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( _shader, sizeof( buf ), nullptr, buf );
        log_error( "Failed to load and compile shader " + _name + ": " + buf );
        return false;
    }

    _loaded = true;

    return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Shader::unload()
{
    glDeleteShader( _shader );
    _loaded = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //