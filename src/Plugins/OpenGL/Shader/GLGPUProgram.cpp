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

#include "GLGPUProgram.h"

#include <LORE2D/Scene/Light.h>
#include <LORE2D/Shader/Shader.h>

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
  _reset();
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
    log_error( "Shader not loaded, not attaching to GPUProgram" + _name );
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

void GLGPUProgram::setTransformVar( const Lore::Matrix4& m )
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

void GLGPUProgram::setUniformVar( const string& id, const Lore::Matrix4& m )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    _updateUniform( uniform, m );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const Lore::Vec2& v )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glm::vec2 glmv = MathConverter::LoreToGLM( v );
    glUniform2fv( uniform, 1, glm::value_ptr( glmv ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const Lore::Vec3& v )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glm::vec3 glmv = MathConverter::LoreToGLM( v );
    glUniform3fv( uniform, 1, glm::value_ptr( glmv ) );
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

void GLGPUProgram::updateLights( Lore::Scene::LightMap::ConstIterator it )
{
  int i = 0;
  while ( it.hasMore() ) {
    auto l = it.getNext();
    const string idx( "lights[" + std::to_string( i ) + "]" );

    //
    // Update all light properties.

    auto posID = glGetUniformLocation( _program, ( idx + ".pos" ).c_str() );
    auto colorID = glGetUniformLocation( _program, ( idx + ".color" ).c_str() );
    auto constantID = glGetUniformLocation( _program, ( idx + ".constant" ).c_str() );
    auto linearID = glGetUniformLocation( _program, ( idx + ".linear" ).c_str() );
    auto quadraticID = glGetUniformLocation( _program, ( idx + ".quadratic" ).c_str() );
    auto intensityID = glGetUniformLocation( _program, ( idx + ".intensity" ).c_str() );

    glUniform2f( posID, l->getPosition().x, l->getPosition().y );
    glUniform3f( colorID, l->getColor().r, l->getColor().g, l->getColor().b );
    glUniform1f( constantID, l->getConstant() );
    glUniform1f( linearID, l->getLinear() );
    glUniform1f( quadraticID, l->getQuadratic() );
    glUniform1f( intensityID, l->getIntensity() );

    ++i;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::setUniformVar( const string& id, const glm::mat4x4& m )
{
  auto uniform = _getUniform( id );
  if ( -1 != uniform ) {
    glUniformMatrix4fv( uniform, 1, GL_FALSE, glm::value_ptr( m ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLuint GLGPUProgram::_getUniform( const string& id )
{
  auto lookup = _uniforms.find( id );
  if ( _uniforms.end() == lookup ) {
    log_warning( "Tried to get uniform " + id + " in " + _name + " which does not exist" );
    return -1;
  }

  // Return uniform GLuint value.
  return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::_updateUniform( const GLint id, const Lore::Matrix4& m )
{
  glm::mat4x4 mm = MathConverter::LoreToGLM( m );
  glUniformMatrix4fv( id, 1, GL_FALSE, glm::value_ptr( mm ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLGPUProgram::_reset()
{
  glDeleteProgram( _program );
  _uniforms.clear();
  _transform = 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
