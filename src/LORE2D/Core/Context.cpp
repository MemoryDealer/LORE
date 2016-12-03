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

#include "Context.h"

#include <LORE2D/Core/NotificationCenter.h>
#include <LORE2D/Core/Timestamp.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Local {

    std::shared_ptr<IRenderPluginLoader> __rpl;
    std::vector<Context::ErrorListener> __errorListeners;

}
using namespace Local;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
: _windowRegistry()
, _active( false )
{
    NotificationCenter::Subscribe<WindowEventNotification>( std::bind( &Context::onWindowEvent, this, std::placeholders::_1 ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

WindowPtr Context::createWindow( const string& title,
                                 const uint width,
                                 const uint height,
                                 const Window::Mode& mode )
{
    std::unique_ptr<Window> window = __rpl->createWindow( title, width, height );
    window->setMode( mode );

    log( "Window " + title + " created successfully" );

    _windowRegistry.insert( title, std::move( window ) );
    
    // At least one window means the context is active.
    _active = true;

    // Return a handle.
    return _windowRegistry.get( title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyWindow( WindowPtr window )
{
    const string title = window->getTitle();

    _windowRegistry.remove( title );
    log( "Window " + title + " destroyed successfully" );

    // Context is no longer considered active if all windows have been destroyed.
    _active = !_windowRegistry.empty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::addErrorListener( ErrorListener listener )
{
    __errorListeners.push_back( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::removeErrorListener( ErrorListener listener )
{
    for ( auto it = __errorListeners.begin(); it != __errorListeners.end(); )
    {
        if ( ( *it ) == listener ) {
            it = __errorListeners.erase( it );
        }
        else {
            ++it;
        }
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

std::unique_ptr<Context> Context::Create( const RenderPlugin& renderer )
{
    string file;

    // Setup required Lore objects.
    Log::AllocateLogger();
    NotificationCenter::Initialize();

    if ( __rpl.get() ) {
        __rpl.reset();
    }

    __rpl = CreateRenderPluginLoader();

    switch ( renderer ) {
    default:
        log_critical( "Unknown render plugin specified" );
        return nullptr;

    case RenderPlugin::OpenGL:
        file = "Plugin_OpenGL";
        break;
    }

    if ( !__rpl->load( file ) ) {
        return nullptr;
    }

    // Load the context class from the plugin.
    auto context = __rpl->createContext();
    return std::move( context );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::Destroy( std::unique_ptr<Context> context )
{
    context.reset();
    Log::DeleteLogger();
    NotificationCenter::Destroy();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::onWindowEvent( const Notification& n )
{
    const WindowEventNotification& wen = static_cast< const WindowEventNotification& >( n );
    destroyWindow( wen.window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::ErrorCallback( int error, const char* desc )
{
    log_error( desc );

    // Call all error listeners.
    for ( const auto& listener : __errorListeners ) {
        listener( error, desc );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
