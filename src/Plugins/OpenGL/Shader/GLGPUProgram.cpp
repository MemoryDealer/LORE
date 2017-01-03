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

#include "GLGPUProgram.h"

#include <LORE2D/Shader/Shader.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgram::GPUProgram( const string& name )
: Lore::GPUProgram( name )
, _program( 0 )
, _uniforms()
{
    _program = glCreateProgram();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgram::~GPUProgram()
{
    glDeleteProgram( _program );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GPUProgram::attachShader( Lore::ShaderPtr shader )
{
    if ( !shader->isLoaded() ) {
        log_error( "Shader " + shader->getName() +
                   " is not loaded, not attaching to GPUProgram" + _name );
        return;
    }

    Lore::GPUProgram::attachShader( shader );

    GLuint id = shader->getUintId();
    glAttachShader( _program, id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GPUProgram::link()
{
    glLinkProgram( _program );

    GLint success;
    GLchar buf[512];
    glGetProgramiv( _program, GL_LINK_STATUS, &success );
    if ( !success ) {
        glGetProgramInfoLog( _program, sizeof( buf ), nullptr, buf );
        log_error( "Failed to link program + " + _name + ": " + string( buf ) );
        return false;
    }

    // Can delete shader buffers (or flag them for delete in GL) now that the program is linked.
    for ( auto pair : _shaders ) {
        ShaderPtr shader = pair.second;
        shader->unload();
    }

    return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GPUProgram::use()
{
    glUseProgram( _program );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GPUProgram::setUniformMatrix4( const string& name, const Lore::Matrix4& mat )
{
    glm::mat4x4 m = MathConverter::LoreToGLM( mat );
    GLuint location = 0;

    auto lookup = _uniforms.find( name );
    if ( _uniforms.end() != lookup ) {
        location = lookup->second;
    }
    else {
        location = glGetUniformLocation( _program, name.c_str() );
        _uniforms.insert( { name, location } );
    }

    glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr( m ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
