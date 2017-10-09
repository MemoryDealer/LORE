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

#include "Context.h"

#include <LORE2D/Core/APIVersion.h>
#include <LORE2D/Core/NotificationCenter.h>
#include <LORE2D/Core/Timestamp.h>
#include <LORE2D/Renderer/SceneGraphVisitor.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Renderable/Textbox.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Local {

  std::unique_ptr<IRenderPluginLoader> __rpl;
  std::vector<Context::ErrorListener> __errorListeners;
  Context* _activeContextPtr = nullptr;

}
using namespace Local;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::Context() noexcept
{
  NotificationSubscribe( WindowEventNotification, &Context::onWindowEvent );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Context::~Context()
{
  NotificationUnsubscribe( WindowEventNotification, &Context::onWindowEvent );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::initConfiguration()
{
  // Setup default memory pool settings.
  _poolCluster.registerPool<Background>( 16 );
  _poolCluster.registerPool<Camera>( 16 );
  _poolCluster.registerPool<Entity>( 16 );
  _poolCluster.registerPool<Light>( 32 );
  _poolCluster.registerPool<Material>( 32 );
  _poolCluster.registerPool<Mesh>( 8 );
  _poolCluster.registerPool<Node>( 1024 );
  _poolCluster.registerPool<Scene>( 4 );
  _poolCluster.registerPool<Textbox>( 32 );

  // TODO: Parse pool settings from cfg file (Lua).
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::renderFrame( const real lagMultiplier )
{
  _frameListenerController->frameStarted();

  // Render all RenderViews for each window.
  WindowRegistry::ConstIterator it = _windowRegistry.getConstIterator();
  while ( it.hasMore() ) {
    WindowPtr window = it.getNext();
    window->renderFrame();
  }

  _frameListenerController->frameEnded();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ScenePtr Context::createScene( const string& name, const RendererType& rt )
{
  auto scene = _poolCluster.create<Scene>();
  scene->setName( name );
  _sceneRegistry.insert( name, scene );

  lore_log( "Scene " + name + " created successfully" );

  return scene;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyScene( const string& name )
{
  auto scene = _sceneRegistry.get( name );
  _poolCluster.destroy<Scene>( scene );
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
  for ( auto it = __errorListeners.begin(); it != __errorListeners.end(); ) {
    if ( ( *it ) == listener ) {
      it = __errorListeners.erase( it );
    }
    else {
      ++it;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceControllerPtr Context::getResourceController() const
{
  return _activeWindow->getResourceController();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceControllerPtr Context::getStockResourceController() const
{
  return _activeWindow->getStockResourceController();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::setActiveWindow( WindowPtr window )
{
  _activeWindow = window;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::setActiveWindow( const string& name )
{
  auto window = _windowRegistry.get( name );
  setActiveWindow( window );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

std::unique_ptr<Context> Context::Create( const RenderPlugin& renderer )
{
  if ( _activeContextPtr ) {
    throw Lore::Exception( "A Lore context already exists for this process!" );
  }

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

  Resource::AssignContext( context.get() );
  StockResource::AssignContext( context.get() );
  MemoryAccess::_SetPrimaryPoolCluster( &context->_poolCluster );
  _activeContextPtr = context.get();

  // Apply configuration settings or use defaults.
  context->initConfiguration();

  return std::move( context );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::Destroy( std::unique_ptr<Context> context )
{
  context.reset();
  Resource::AssignContext( nullptr );
  StockResource::AssignContext( nullptr );
  MemoryAccess::_SetPrimaryPoolCluster( nullptr );
  Log::DeleteLogger();
  NotificationCenter::Destroy();
  _activeContextPtr = nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::RegisterFrameListener( FrameListener* listener )
{
  _activeContextPtr->_frameListenerController->registerFrameListener( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::RegisterFrameStartedCallback( FrameListenerController::FrameStartedCallback callback )
{
  _activeContextPtr->_frameListenerController->registerFrameStartedCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::RegisterFrameEndedCallback( FrameListenerController::FrameEndedCallback callback )
{
  _activeContextPtr->_frameListenerController->registerFrameEndedCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::UnregisterFrameListener( FrameListener* listener )
{
  _activeContextPtr->_frameListenerController->unregisterFrameListener( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::UnregisterFrameStartedCallback( FrameListenerController::FrameStartedCallback callback )
{
  _activeContextPtr->_frameListenerController->unregisterFrameStartedCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::UnregisterFrameEndedCallback( FrameListenerController::FrameEndedCallback callback )
{
  _activeContextPtr->_frameListenerController->unregisterFrameEndedCallback( callback );
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

void Context::setAPIVersion( const int major, const int minor )
{
  APIVersion::Set( major, minor );
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
