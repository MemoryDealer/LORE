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

    std::unique_ptr<IRenderPluginLoader> __rpl;
    std::vector<Context::ErrorListener> __errorListeners;

}
using namespace Local;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
: _windowRegistry()
, _sceneRegistry()
, _active( false )
{
    NotificationSubscribe( WindowEventNotification, &Context::onWindowEvent );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
    NotificationUnsubscribe( WindowEventNotification, &Context::onWindowEvent );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ScenePtr Context::createScene( const string& name, const RendererType& rt )
{
    auto scene = std::make_unique<Scene>( name );

    lore_log( "Scene " + name + " created successfully" );

    ScenePtr handle = _sceneRegistry.insert( name, std::move( scene ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyScene( const string& name )
{
    _sceneRegistry.remove( name );
    lore_log( "Scene " + name + " destroyed successfully" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyScene( ScenePtr scene )
{
    destroyScene( scene->getName() );
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

    switch ( wen.event ) {

    default:
        break;

    case WindowEventNotification::Event::Closed:
        destroyWindow( wen.window );
        break;

    }
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
