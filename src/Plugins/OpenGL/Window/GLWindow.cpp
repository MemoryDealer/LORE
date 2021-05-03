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

#include "GLWindow.h"

#include "CallbackHandler.h"

#include "imgui.h"

#include <LORE/Core/APIVersion.h>
#include <LORE/Core/NotificationCenter.h>
#include <LORE/UI/UI.h>
#include <Plugins/OpenGL/Resource/GLResourceController.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <UI/imgui_impl_glfw.h>
#include <UI/imgui_impl_opengl3.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLWindow::GLWindow()
  : _window( nullptr )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLWindow::~GLWindow()
{
  glfwDestroyWindow( _window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::init( const string& title,
                     const int width,
                     const int height,
                     const Lore::RendererType rendererTypeMask )
{
  _title = title;
  Window::setDimensions( width, height );

  _window = glfwCreateWindow( _dimensions.width,
                              _dimensions.height,
                              _title.c_str(),
                              nullptr,
                              nullptr );

  // Store user pointer to Lore Window class for callbacks.
  glfwSetWindowUserPointer( _window, reinterpret_cast< void* >( this ) );

  // Store frame buffer size.
  glfwGetFramebufferSize( _window, &_frameBufferWidth, &_frameBufferHeight );
  _aspectRatio = static_cast< float >( _frameBufferWidth ) / _frameBufferHeight;

  // Setup callbacks.
  glfwSetWindowSizeCallback( _window, WindowCallbackHandler::Size );

  // Create a resource controller for each window.
  _controller = std::make_unique<GLResourceController>();

  // Create stock resources as well (make this window's context active
  // and then restore the previous one).
  GLFWwindow* currentContext = glfwGetCurrentContext();
  glfwMakeContextCurrent( _window );

  _stockController = std::make_unique<GLStockResourceController>();
  _stockController->createStockResources();

  // Create stock resources needed for this window given the expected renderer type(s).
  // TODO: Use configuration setting to initialize renderer type(s).
  _stockController->createRendererStockResources( RendererType::Forward2D );
  _stockController->createRendererStockResources( RendererType::Forward3D );

  glfwMakeContextCurrent( currentContext );

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL( _window, false );
  const string glsl_version = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";
  ImGui_ImplOpenGL3_Init( glsl_version.c_str() );

#ifdef LORE_DEBUG_UI
  _debugUI->setImGuiContext( ImGui::GetCurrentContext() );
#endif
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::renderFrame()
{
  if ( glfwWindowShouldClose( _window ) ) {
    // Post window closed event.
    WindowEventNotification n;
    n.event = WindowEventNotification::Event::Closed;
    n.window = this;
    NotificationCenter::Post<WindowEventNotification>( n );
    return;
  }

  // Render each Scene with the corresponding RenderView data.
  for ( const RenderView& rv : _renderViews ) {
    RendererPtr renderer = rv.scene->getRenderer();
    renderer->present( rv, this );
  }

  // TODO: Custom UIs.

#ifdef LORE_DEBUG_UI

  // Start the Dear ImGui frame
  if ( _debugUI->getEnabled() ) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    _debugUI->render( ImGui::GetCurrentContext() );

    ImGui::Render();

    glfwMakeContextCurrent( _window );

    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
  }
#endif

  glfwMakeContextCurrent( _window );
  glfwSwapBuffers( _window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::addRenderView( const Lore::RenderView& renderView )
{
  // Convert Viewport to gl_viewport.
  RenderView rv = renderView;
  Rect vp = rv.viewport;
  rv.gl_viewport.x = static_cast< int >( vp.x * static_cast< float >( _frameBufferWidth ) );
  rv.gl_viewport.y = static_cast< int >( vp.y * static_cast< float >( _frameBufferHeight ) );
  rv.gl_viewport.width = static_cast< int >( vp.w * static_cast< float >( _frameBufferWidth ) );
  rv.gl_viewport.height = static_cast< int >( vp.h * static_cast< float >( _frameBufferHeight ) );

  rv.gl_viewport.aspectRatio = static_cast< real >( rv.gl_viewport.width ) / rv.gl_viewport.height;

  Lore::Window::addRenderView( rv );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::setTitle( const string& title )
{
  Lore::Window::setTitle( title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::setDimensions( const int width, const int height )
{
  Lore::Window::setDimensions( width, height );

  glfwSetWindowSize( _window, width, height );

  // Store frame buffer size.
  glfwGetFramebufferSize( _window, &_frameBufferWidth, &_frameBufferHeight );

  _aspectRatio = static_cast< float >( _frameBufferWidth ) / _frameBufferHeight;

  updateRenderViews();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::setActive()
{
  glfwMakeContextCurrent( _window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLWindow::updateRenderViews()
{
  for ( auto& rv : _renderViews ) {
    rv.gl_viewport.x = static_cast< int >( rv.viewport.x * static_cast< float >( _frameBufferWidth ) );
    rv.gl_viewport.y = static_cast< int >( rv.viewport.y * static_cast< float >( _frameBufferHeight ) );
    rv.gl_viewport.width = static_cast< int >( rv.viewport.w * static_cast< float >( _frameBufferWidth ) );
    rv.gl_viewport.height = static_cast< int >( rv.viewport.h * static_cast< float >( _frameBufferHeight ) );

    // Resize post-processing render targets.
    if ( rv.camera->postProcessing ) {
      const u32 sampleCount = rv.camera->postProcessing->renderTarget->_sampleCount;
      rv.camera->initPostProcessing( rv.gl_viewport.width, rv.gl_viewport.height, sampleCount );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
