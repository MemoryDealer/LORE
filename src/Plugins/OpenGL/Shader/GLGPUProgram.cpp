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

#include "GLGPUProgram.h"

#include <LORE/Scene/Light.h>
#include <LORE/Shader/Shader.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLGPUProgram::GLGPUProgram()
  : Lore::GPUProgram()
  , _program( 0 )
  , _uniforms()
  , _transform( 0 )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLGPUProgram::~GLGPUProgram()
{
  glDeleteProgram( _program );
  _uniforms.clear();
  _transform = 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::init()
{
  _program = glCreateProgram();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::attachShader( Lore::ShaderPtr shader )
{
  if ( !shader->isLoaded() ) {
    LogWrite( Error, "Shader not loaded, not attaching to GPUProgram %s", _name.c_str() );
    return;
  }

  Lore::GPUProgram::attachShader( shader );

  GLuint id = shader->getUintId(); // TODO: Use getData("", &voidptr);
  glAttachShader( _program, id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GLGPUProgram::link()
{
  glLinkProgram( _program );

  GLint success = 0;
  GLchar buf[512];
  glGetProgramiv( _program, GL_LINK_STATUS, &success );
  if ( !success ) {
    glGetProgramInfoLog( _program, sizeof( buf ), nullptr, buf );
    LogWrite( Error, "Failed to link program %s (%s): %s", _name.c_str(), glGetError(), buf );
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

void GLGPUProgram::use()
{
  glUseProgram( _program );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::addTransformVar( const string& id )
{
  _transform = glGetUniformLocation( _program, id.c_str() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setTransformVar( const glm::mat4& m )
{
  _updateUniform( _transform, m );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::addUniformVar( const string& id )
{
  GLuint uniform = glGetUniformLocation( _program, id.c_str() );
  _uniforms.insert( { id, uniform } );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const glm::mat4& m )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    _updateUniform( uniform, m );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const glm::vec2& v )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform2fv( uniform, 1, glm::value_ptr( v ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const glm::vec3& v )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform3fv( uniform, 1, glm::value_ptr( v ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const glm::vec4& v )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform4fv( uniform, 1, glm::value_ptr( v ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const real r )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform1f( uniform, static_cast< GLfloat >( r ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const uint32_t i )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform1ui( uniform, static_cast< GLuint >( i ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const int i )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniform1i( uniform, static_cast<GLint>( i ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLuint GLGPUProgram::_getUniform( const string& id )
{
  auto lookup = _uniforms.find( id );
  if ( _uniforms.end() == lookup ) {
    LogWrite( Warning, "Tried to get uniform %s in %s which does not exist", id.c_str(), _name.c_str() );
    return -1;
  }

  // Return uniform GLuint value.
  return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::_updateUniform( const GLint id, const glm::mat4& m )
{
  glUniformMatrix4fv( id, 1, GL_FALSE, glm::value_ptr( m ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
