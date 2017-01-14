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

#include "GLContext.h"

#include <Plugins/OpenGL/Window/GLWindow.h>
#include <Plugins/OpenGL/Renderer/RendererFactory.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

static void APIENTRY openGlDebugCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
    printf( "DBG: %s\n", message );
    printf( "Source: " );

    switch ( source )
    {
    case GL_DEBUG_SOURCE_API:
        printf( "API" );
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf( "Window System" );
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf( "Shader Compiler" );
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf( "Third Party" );
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf( "Application" );
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf( "Other" );
        break;
    }

    printf( "\n" );
    printf( "Type: " );

    switch ( type )
    {
    case GL_DEBUG_TYPE_ERROR:
        printf( "Error" );
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf( "Deprecated Behaviour" );
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf( "Undefined Behaviour" );
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf( "Portability" );
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf( "Performance" );
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf( "Marker" );
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf( "Push Group" );
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf( "Pop Group" );
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf( "Other" );
        break;
    }

    printf( "\n" );
    printf( "ID: %d\n", id );
    printf( "Severity: " );

    switch ( severity )
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf( "High" );
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf( "Medium" );
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf( "Low" );
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf( "Notification" );
        break;
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
: Lore::Context()
, _offscreenContextWindow( nullptr )
, _renderers()
, _resourceLoader()
{
    lore_log( "Initializing OpenGL render plugin context..." );
    glfwInit();
    glfwSetErrorCallback( ErrorCallback );
    lore_log( "OpenGL render plugin context initialized!" );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
    glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );
    _offscreenContextWindow = glfwCreateWindow( 1, 1, "", nullptr, nullptr );
    glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );
    glfwMakeContextCurrent( _offscreenContextWindow );

    gladLoadGLLoader( reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) );
    glfwSwapInterval( 1 );

    // Debug.
    GLint flags;
    glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
    if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
    {
        glEnable( GL_DEBUG_OUTPUT );
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );

        glDebugMessageCallback( openGlDebugCallback, nullptr );
        glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    }
    else
    {
        printf( "No debug bit set in context flags\n" );
    }
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

void Context::renderFrame( const float dt )
{
    glfwPollEvents();

    // Render all RenderViews for each window.
    WindowRegistry::ConstIterator it = _windowRegistry.getConstIterator();
    while ( it.hasMore() ) {
        WindowPtr window = it.getNext();
        window->renderFrame();
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::WindowPtr Context::createWindow( const string& title,
                                       const uint width,
                                       const uint height,
                                       const Window::Mode& mode )
{
    auto window = std::make_unique<Window>( title, width, height );
    window->setMode( mode );

    lore_log( "Window " + title + " created successfully" );

    Lore::WindowPtr handle = _windowRegistry.insert( title, std::move( window ) );

    // At least one window means the context is active.
    _active = true;

    // Return a handle.
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyWindow( Lore::WindowPtr window )
{
    const string title = window->getTitle();

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

    scene->setRenderer( rp );
    return scene;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::string Context::getRenderPluginName() const
{
    return Lore::string( "OpenGL" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
