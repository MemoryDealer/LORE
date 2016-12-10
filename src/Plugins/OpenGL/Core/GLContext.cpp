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

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
: Lore::Context()
, _offscreenContextWindow( nullptr )
{
    log( "Initializing OpenGL render plugin context..." );
    glfwInit();
    glfwSetErrorCallback( ErrorCallback );
    log( "OpenGL render plugin context initialized!" );

    glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );
    _offscreenContextWindow = glfwCreateWindow( 1, 1, "", nullptr, nullptr );
    glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );
    glfwMakeContextCurrent( _offscreenContextWindow );

    gladLoadGLLoader( reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) );
    glfwSwapInterval( 1 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
    glfwDestroyWindow( _offscreenContextWindow );

    log( "Terminating OpenGL render plugin context..." );
    glfwSetErrorCallback( nullptr );
    glfwTerminate();
    log( "OpenGL render plugin context terminated!" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::renderFrame( const float dt )
{
    glfwPollEvents();

    RegistryIterator<Lore::Window> it = _windowRegistry.getIterator();
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
    std::unique_ptr<Window> window = std::make_unique<Window>( title, width, height );
    window->setMode( mode );

    log( "Window " + title + " created successfully" );

    _windowRegistry.insert( title, std::move( window ) );

    // At least one window means the context is active.
    _active = true;

    // Return a handle.
    return _windowRegistry.get( title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyWindow( Lore::WindowPtr window )
{
    const string title = window->getTitle();

    _windowRegistry.remove( title );
    log( "Window " + title + " destroyed successfully" );

    // Context is no longer considered active if all windows have been destroyed.
    _active = !_windowRegistry.empty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::string Context::getRenderPluginName() const
{
    return Lore::string( "OpenGL" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
