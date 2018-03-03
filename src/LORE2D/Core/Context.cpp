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

#include <LORE2D/Config/Config.h>
#include <LORE2D/Core/APIVersion.h>
#include <LORE2D/Core/NotificationCenter.h>
#include <LORE2D/Core/Timestamp.h>
#include <LORE2D/Core/CLI/CLI.h>
#include <LORE2D/Core/DebugUI/DebugUI.h>
#include <LORE2D/Input/Input.h>
#include <LORE2D/Renderer/RendererFactory.h>
#include <LORE2D/Renderer/SceneGraphVisitor.h>
#include <LORE2D/Resource/Box.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Sprite.h>
#include <LORE2D/Resource/Textbox.h>
#include <LORE2D/Scene/SpriteController.h>
#include <LORE2D/UI/UI.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Local {

  static RenderPluginLoader __rpl;
  static std::vector<Context::ErrorListener> __errorListeners;
  static Context* _activeContextPtr = nullptr;

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

  // Explicitly destroy Scene objects before resources, otherwise the order of destruction
  // in the pool cluster can lead to crashes, since resources (such as Box or Light) can be destroyed
  // before the owning object tries to delete them (AABBs for Boxes).
  _poolCluster.unregisterPool<Scene>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::initConfiguration()
{
  // Setup default memory pool settings. (TODO: Move out of Context.cpp).
  _poolCluster.registerPool<Background>( 16 );
  _poolCluster.registerPool<Box>( 100024 );
  _poolCluster.registerPool<Camera>( 16 );
  _poolCluster.registerPool<Entity>( 16 );
  _poolCluster.registerPool<Light>( 32 );
  _poolCluster.registerPool<Material>( 32 );
  _poolCluster.registerPool<Mesh>( 8 );
  _poolCluster.registerPool<Node>( 10024 );
  _poolCluster.registerPool<Scene>( 4 );
  _poolCluster.registerPool<Sprite>( 32 );
  _poolCluster.registerPool<SpriteAnimationSet>( 8 );
  _poolCluster.registerPool<Textbox>( 8 );
  _poolCluster.registerPool<UI>( 8 );
  _poolCluster.registerPool<UIPanel>( 8 );
  _poolCluster.registerPool<UIElement>( 16 );

  // TODO: Parse pool/config settings from cfg file (Lua).
  Config::SetValue( "RenderAABBs", false );

  // Setup CLI.
  CLI::Init();

  Input::AddKeyListener( this );
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

  // Assign the specified renderer to this scene.
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

CameraPtr Context::createCamera( const string& name )
{
  auto camera = _poolCluster.create<Camera>();
  camera->_name = name;
  _cameraRegistry.insert( name, camera );

  return camera;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::destroyCamera( CameraPtr camera )
{
  auto name = camera->getName();
  _cameraRegistry.remove( name );

  _poolCluster.destroy<Camera>( camera );
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

InputControllerPtr Context::getInputController() const
{
  return _inputController.get();
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

  __rpl.free();

  switch ( renderer ) {
  default:
    log_critical( "Unknown render plugin specified" );
    return nullptr;

  case RenderPlugin::OpenGL:
    file = "Plugin_OpenGL";
    break;
  }

  if ( !__rpl.load( file ) ) {
    return nullptr;
  }

  // Load the context class from the plugin.
  auto context = __rpl.createContext();

  CLI::AssignContext( context.get() );
  Input::AssignContext( context.get() );
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
  // TODO: Use null object pattern to prevent segfaults in the following.
  CLI::AssignContext( nullptr );
  Input::AssignContext( nullptr );
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

WindowPtr Context::GetActiveWindow()
{
  return _activeContextPtr->getActiveWindow();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Context::onKeyDown( const Keycode code )
{
  // Process default DebugUI keys.
  if ( DebugUI::IsEnabled() ) {
    switch ( code ) {
    default:
      break;

    case Keycode::GraveAccent:
      DebugUI::DisplayConsole();
      break;

    case Keycode::B: {
      const string value( "RenderAABBs" );
      auto renderAABBs = Config::GetValue( value );
      Config::SetValue( value, !GET_VARIANT<bool>( renderAABBs ) );
    } break;

    case Keycode::F: {
      if ( DebugUI::IsStatsUIEnabled() ) {
        DebugUI::HideStats();
      }
      else {
        DebugUI::DisplayStats();
      }
    } break;

    }
  }
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
