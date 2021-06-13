// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "GLStockResource.h"

#include <LORE/Core/APIVersion.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLStockResourceController::GLStockResourceController()
{
  _controller = std::make_unique<GLResourceController>();

  _factories[RendererType::Forward2D] = std::make_unique<GLStockResource2DFactory>( _controller.get() );
  _factories[RendererType::Forward3D] = std::make_unique<GLStockResource3DFactory>( _controller.get() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResourceController::createTextProgram( const string& name )
{
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Layout.

  src += "layout (location = 0) in vec4 vertex;";

  //
  // Uniforms and outs.

  src += "uniform mat4 projection;";
  src += "uniform float depth;";
  src += "out vec2 TexCoords;";

  //
  // main function.

  src += "void main() {";

  src += "gl_Position = projection * vec4(vertex.xy, depth, 1.0);";
  src += "TexCoords = vertex.zw;";

  src += "}";

  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile text vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  //
  // Ins/outs and uniforms.

  src += "in vec2 TexCoords;";
  src += "out vec4 pixel;";
  
  src += "uniform sampler2D text;";
  src += "uniform vec4 color;";

  //
  // main function.

  src += "void main() {";

  src += "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);";
  src += "pixel = color * sampled;";

  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile text fragment shader for " + name );
    // TODO: Rollback vertex shaders in case of failed fragment shader.
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // GPU program.

  auto program = _controller->create<Lore::GPUProgram>( name );
  program->init();
  program->attachShader( vsptr );
  program->attachShader( fsptr );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  program->addUniformVar( "projection" );
  program->addUniformVar( "depth" );
  program->addUniformVar( "color" );

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
