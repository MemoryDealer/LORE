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

#include "GLWindow.h"

#include <LORE2D/Core/NotificationCenter.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::Window( const string& title,
                const int width,
                const int height )
: Lore::Window( title, width, height )
, _window( nullptr )
{
    _window = glfwCreateWindow( _width,
                                _height,
                                _title.c_str(),
                                nullptr,
                                nullptr );

    // Store frame buffer size.
    glfwGetFramebufferSize( _window, &_frameBufferWidth, &_frameBufferHeight );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::~Window()
{
    glfwDestroyWindow( _window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::renderFrame()
{
    if ( glfwWindowShouldClose( _window ) ) {
        // Post window closed event.
        WindowEventNotification n;
        n.event = WindowEventNotification::Event::Closed;
        n.window = this;
        NotificationCenter::Post<WindowEventNotification>( n );
        return;
    }

    glfwMakeContextCurrent( _window );

    // Render each Scene with the corresponding RenderView data.
    for ( const RenderView& rv : _renderViews ) {
        RendererPtr renderer = rv.scene->getRenderer();
        renderer->present( rv );
    }

    glfwSwapBuffers( _window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::addRenderView( const Lore::RenderView& renderView )
{
    // Convert Viewport to gl_viewport.
    RenderView rv = renderView;
    Viewport vp = rv.viewport;
    rv.gl_viewport.x = static_cast< int >( vp.x * static_cast<float>( _frameBufferWidth ) );
    rv.gl_viewport.y = static_cast< int >( vp.y * static_cast<float>( _frameBufferHeight ) );
    rv.gl_viewport.width = static_cast< int >( vp.width * static_cast<float>( _frameBufferWidth ) );
    rv.gl_viewport.height = static_cast< int >( vp.height * static_cast<float>( _frameBufferHeight ) );

    Lore::Window::addRenderView( rv );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setTitle( const string& title )
{
    Lore::Window::setTitle( title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setDimensions( const int width, const int height )
{
    Lore::Window::setDimensions( width, height );

    glfwSetWindowSize( _window, width, height );

    // Store frame buffer size.
    glfwGetFramebufferSize( _window, &_frameBufferWidth, &_frameBufferHeight );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
