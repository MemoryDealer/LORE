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

#include "Window.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::Window( const string& title,
                const int width,
                const int height )
: _title( title )
, _width( width )
, _height( height )
, _frameBufferWidth( 0 )
, _frameBufferHeight( 0 )
, _mode( Mode::Windowed )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::~Window()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::addRenderView( const RenderView& renderView )
{
    // Verify this render view does not already exist.
    for ( const auto& rv : _renderViews ) {
        if ( rv == renderView ) {
            throw Lore::Exception( "RenderView " + renderView.name +
                                   " already exists in Window " + _title );
        }
    }

    log( "Adding RenderView " + renderView.name + " to Window " + _title );
    _renderViews.push_back( renderView );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::removeRenderView( const RenderView& renderView )
{
    removeRenderView( renderView.name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::removeRenderView( const string& name )
{
    for ( auto it = _renderViews.begin(); it != _renderViews.end(); ) {
        const RenderView& rv = ( *it );
        if ( rv.name == name ) {
            log( "Removing RenderView " + name + " from Window " + _title );
            it = _renderViews.erase( it );
            break;
        }
        else {
            ++it;
        }
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderView& Window::getRenderView( const string& name )
{
    for ( auto& rv : _renderViews ) {
        if ( name == rv.name ) {
            return rv;
        }
    }

    throw Lore::Exception( "RenderView " + name + " does not exist in Window " + _title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::resetRenderViews()
{
    _renderViews.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setTitle( const string& title )
{
    _title = title;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setDimensions( const int width, const int height )
{
    _width = width;
    _height = height;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setMode( const Mode& mode )
{
    _mode = mode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
