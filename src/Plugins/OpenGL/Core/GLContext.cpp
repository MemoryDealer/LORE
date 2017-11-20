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

#include "GLContext.h"

#include "DebugCallback.h"

#include <LORE2D/Renderer/RendererFactory.h>

#include <Plugins/OpenGL/Input/GLInput.h>
#include <Plugins/OpenGL/Renderer/RenderAPI.h>
#include <Plugins/OpenGL/Resource/GLFont.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/Renderable/GLTexture.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Shader/GLVertexBuffer.h>
#include <Plugins/OpenGL/Window/GLRenderTarget.h>
#include <Plugins/OpenGL/Window/GLWindow.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace GLContextNS {

  const static int MinimumVersionMajor = 3;
  const static int MinimumVersionMinor = 3;

}
using namespace GLContextNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
{
  lore_log( "Initializing OpenGL render plugin context..." );
  glfwInit();
  glfwSetErrorCallback( ErrorCallback );
  lore_log( "OpenGL render plugin context initialized!" );

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
  glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );
  _offscreenContextWindow = glfwCreateWindow( 1, 1, "", nullptr, nullptr );
  glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );
  glfwMakeContextCurrent( _offscreenContextWindow );

  gladLoadGLLoader( reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) );
  glfwSwapInterval( 1 );

  // Get API version.
  int verMajor, verMinor;
  glGetIntegerv( GL_MAJOR_VERSION, &verMajor );
  glGetIntegerv( GL_MINOR_VERSION, &verMinor );
  setAPIVersion( verMajor, verMinor );

  // Setup debug callback.
  if ( verMajor >= 4 && verMinor >= 3 ) {
    GLint flags;
    glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
    if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT ) {
      glEnable( GL_DEBUG_OUTPUT );
      glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );

      glDebugMessageCallback( ContextCallbackHandler::OpenGLDebug, nullptr );
      glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    }
    else {
      log_warning( "No debug bit set in context flags\n" );
    }
  }

  _renderAPI = std::make_unique<RenderAPI>();
  _inputController = std::make_unique<GLInputController>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
  glfwDestroyWindow( _offscreenContextWindow );

  lore_log( "Terminating OpenGL render plugin context..." );
  glfwSetErrorCallback( nullptr );
  glfwTerminate();
  lore_log( "OpenGL render plugin context terminated!" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::initConfiguration()
{
  Lore::Context::initConfiguration();

  // Setup default memory pool settings.
  _poolCluster.registerPool<Font, GLFont>( 4 );
  _poolCluster.registerPool<GPUProgram, GLGPUProgram>( 16 );
  _poolCluster.registerPool<RenderTarget, GLRenderTarget>( 4 );
  _poolCluster.registerPool<Shader, GLShader>( 32 );
  _poolCluster.registerPool<Texture, GLTexture>( 64 );
  _poolCluster.registerPool<VertexBuffer, GLVertexBuffer>( 32 );
  _poolCluster.registerPool<Window, GLWindow>( 1 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::renderFrame( const float lagMultiplier )
{
  glfwPollEvents();

  Lore::Context::renderFrame( lagMultiplier );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::WindowPtr Context::createWindow( const string& title,
                                       const uint width,
                                       const uint height,
                                       const Lore::Window::Mode& mode )
{
  auto window = _poolCluster.create<Window, GLWindow>();
  window->init( title, width, height );
  window->setMode( mode );
  _windowRegistry.insert( title, window );

  lore_log( "Window " + title + " created successfully" );

  // At least one window means the context is active.
  _active = true;

  // If no active window yet, assign by default.
  if ( !_activeWindow ) {
    _activeWindow = window;
  }

  // Register input callbacks for this window.
  _inputController->createCallbacks( window );

  DebugUI::Init();

  // Return a handle.
  return window;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyWindow( Lore::WindowPtr window )
{
  const string title = window->getTitle();
  _poolCluster.destroy<Window, GLWindow>( window ); // TODO: Modify memory pool to store base & derived type?
  _windowRegistry.remove( title );

  lore_log( "Window " + title + " destroyed successfully" );

  // Context is no longer considered active if all windows have been destroyed.
  _active = !_windowRegistry.empty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ScenePtr Context::createScene( const string& name, const Lore::RendererType& rt )
{
  Lore::ScenePtr scene = Lore::Context::createScene( name, rt );
  RendererPtr rp = nullptr;

  auto lookup = _renderers.find( rt );
  if ( _renderers.end() == lookup ) {
    // This renderer type hasn't been created yet, allocate one and assign it.
    auto result = _renderers.insert( { rt, RendererFactory::Create( rt ) } );
    rp = result.first->second.get();
  }
  else {
    rp = lookup->second.get();
  }

  rp->setRenderAPI( _renderAPI.get() );
  scene->setRenderer( rp );
  return scene;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::string Context::getRenderPluginName() const
{
  return Lore::string( "OpenGL" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
