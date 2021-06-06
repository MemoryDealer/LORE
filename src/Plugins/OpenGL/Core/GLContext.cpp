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

#include "GLContext.h"

#include "DebugCallback.h"

#include "imgui.h"

#include <LORE/Core/APIVersion.h>
#include <Plugins/OpenGL/Input/GLInput.h>
#include <Plugins/OpenGL/Renderer/RenderAPI.h>
#include <Plugins/OpenGL/Resource/GLFont.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/GLTexture.h>
#include <Plugins/OpenGL/Scene/GLMesh.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Window/GLRenderTarget.h>
#include <Plugins/OpenGL/Window/GLWindow.h>
#include <UI/imgui_impl_glfw.h>
#include <UI/imgui_impl_opengl3.h>

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
  LogWrite( Info, "Initializing OpenGL render plugin context..." );
  glfwInit();
  glfwSetErrorCallback( ErrorCallback );
  LogWrite( Info, "OpenGL render plugin context initialized!" );

  // TODO: Don't hardcode these values.
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
      LogWrite( Warning, "No debug bit set in context flags\n" );
    }
  }

  _renderAPI = std::make_unique<RenderAPI>();
  _inputController = std::make_unique<GLInputController>();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); ( void )io;
  //io.IniFilename = FileUtil::ApplyWorkingDirectory( " + UI.ini" ).c_str(); // https://github.com/ocornut/imgui/issues/454
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'misc/fonts/README.txt' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow( _offscreenContextWindow );

  LogWrite( Info, "Terminating OpenGL render plugin context..." );
  glfwSetErrorCallback( nullptr );
  glfwTerminate();
  LogWrite( Info, "OpenGL render plugin context terminated!" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::initConfiguration()
{
  Lore::Context::initConfiguration();

  // Setup default memory pool settings.
  _poolCluster.registerPool<Font, GLFont>( 4 );
  _poolCluster.registerPool<GPUProgram, GLGPUProgram>( 64 );
  _poolCluster.registerPool<Mesh, GLMesh>( 64 );
  _poolCluster.registerPool<RenderTarget, GLRenderTarget>( 16 );
  _poolCluster.registerPool<Shader, GLShader>( 64 );
  _poolCluster.registerPool<Texture, GLTexture>( 64 );
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
                                       const uint32_t width,
                                       const uint32_t height,
                                       const int sampleCount,
                                       const Lore::RendererType rendererTypeMask,
                                       const Lore::Window::Mode& mode )
{
  // Setup multisampling for this window.
  glfwWindowHint( GLFW_SAMPLES, sampleCount );
  glEnable( GL_MULTISAMPLE );

  auto window = _poolCluster.create<Window, GLWindow>();
  window->init( title, width, height, rendererTypeMask );
  window->setMode( mode );
  _windowRegistry.insert( title, window );

  LogWrite( Info, "Window %s created successfully", title.c_str() );

  // At least one window means the context is active.
  _active = true;

  // If no active window yet, assign by default.
  if ( !_activeWindow ) {
    _activeWindow = window;
  }

  // Register input callbacks for this window.
  _inputController->createCallbacks( window );

  // Return a handle.
  return window;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyWindow( Lore::WindowPtr window )
{
  const string title = window->getTitle();
  _poolCluster.destroy<Window, GLWindow>( window ); // TODO: Modify memory pool to store base & derived type?
  _windowRegistry.remove( title );

  LogWrite( Info, "Window %s destroyed successfully", title.c_str() );

  // Context is no longer considered active if all windows have been destroyed.
  _active = !_windowRegistry.empty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ScenePtr Context::createScene( const string& name, const Lore::RendererType& rt )
{
  Lore::ScenePtr scene = Lore::Context::createScene( name, rt );
  scene->getRenderer()->setRenderAPI( _renderAPI.get() );
  
  return scene;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::string Context::getRenderPluginName() const
{
  return Lore::string( "OpenGL" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
