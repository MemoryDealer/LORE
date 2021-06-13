#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include <LORE/Core/NotificationCenter.h>
#include <LORE/Core/Plugin/Plugins.h>
#include <LORE/Core/Plugin/RenderPluginLoader.h>
#include <LORE/Input/Input.h>
#include <LORE/Memory/PoolCluster.h>
#include <LORE/Renderer/FrameListener/FrameListenerController.h>
#include <LORE/Renderer/Renderer.h>
#include <LORE/Renderer/IRenderAPI.h>
#include <LORE/Resource/Registry.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Shader/GPUProgram.h>
#include <LORE/Window/Window.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Context
  /// \brief The single owner of all Lore functionality.
  class LORE_EXPORT Context : public KeyListener
  {

  public:

    ///
    /// \typedef ErrorListener
    /// \brief Function pointer to an error listener callback.
    using ErrorListener = void( *)( int, const char* );

  public:

    Context() noexcept;

    virtual ~Context() override;

    virtual void initConfiguration();

    //
    // Rendering.

    ///
    /// \brief Renders a single frame using configured Windows and Scenes.
    virtual void renderFrame( const real lagMultiplier = 1.f );

    //
    // Factory/Destruction functions.

    ///
    /// \brief Creates a window and returns a handle to it.
    virtual WindowPtr createWindow( const string& title,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const int sampleCount,
                                    const RendererType rendererTypeMask,
                                    const Window::Mode& mode = Window::Mode::Windowed ) = 0;

    ///
    /// \brief Destroys specified window. If this is the last remaining window,
    ///     the context will no longer be active (calls to active() will return false).
    virtual void destroyWindow( WindowPtr window ) = 0;

    ///
    /// \brief Creates a scene, assigns to it the specified renderer type, and returns a handle to it.
    virtual ScenePtr createScene( const string& name,
                                  const RendererType& rt );

    ///
    /// \brief Destroys specified scene and all of its nodes.
    virtual void destroyScene( const string& name );

    ///
    /// \brief Destroys specified scene and all of its nodes.
    virtual void destroyScene( ScenePtr scene );

    ///
    /// \brief Creates a new camera for use with a RenderView.
    CameraPtr createCamera( const string& name, const Camera::Type type );

    ///
    /// \brief Destroys specified camera.
    void destroyCamera( CameraPtr camera );

    ///
    /// \brief Destroys all created cameras.
    void destroyAllCameras();

    //
    // Information.

    ///
    /// \brief Returns name of the loaded render plugin.
    ///     Example: "OpenGL" or "Direct3D".
    virtual string getRenderPluginName() const = 0;

    //
    // Callbacks.

    ///
    /// \brief Registers ErrorListener function pointer to be called when a Lore error occurs.
    void addErrorListener( ErrorListener listener );

    ///
    /// \brief Removes ErrorListener function pointer from registered listeners.
    void removeErrorListener( ErrorListener listener );

    //
    // Getters.

    ///
    /// \brief Returns true if the context is active (requires at least one window).
    inline bool active() const
    {
      return _active;
    }

    ///
    /// \brief Returns pointer to the Context's active ResourceController, which is used to
    ///     create, destroy, load, and unload resources.
    /// \details The "active" ResourceController is determined by the active Window. Each
    ///     Window manages its own resources, and has its own ResourceController.
    ResourceControllerPtr getResourceController() const;

    ///
    /// \brief Returns pointer to the Context's active StockResourceController, which is used
    ///     for acquiring pointers to stock resources such as textures, vertex buffers,
    ///     GPUPrograms, etc.
    /// \details The "active" StockResourceController is determined by the active Window. Each
    ///     Window has its own StockResourceController.
    StockResourceControllerPtr getStockResourceController() const;

    ///
    /// \brief Returns pointer to Window in Context of the specified name.
    ///     Throws ItemIdentityException if not found.
    WindowPtr getWindow( const string& title )
    {
      return _windowRegistry.get( title );
    }

    ///
    /// \brief Gets active Window of Context.
    WindowPtr getActiveWindow() const
    {
      return _activeWindow;
    }

    ///
    /// \brief Returns specified CameraPtr if it exists.
    CameraPtr getCamera( const string& name )
    {
      auto it = _cameras.find( name );
      if ( _cameras.end() == it ) {
        throw ItemIdentityException( "Camera " + name + " does not exist" );
      }
      return it->second.get();
    }

    ///
    /// \brief Returns pointer to Scene in Context of the specified name.
    ///     Throws ItemIdentityException if not found.
    ScenePtr getScene( const string& name )
    {
      return _sceneRegistry.get( name );
    }

    ///
    /// \brief Returns InputController instance allocated by render plugin's Context.
    InputControllerPtr getInputController() const;

    //
    // Modifiers.

    ///
    /// \brief Sets active Window of Context.
    /// \details Succeeding calls to getResourceController()
    ///     will return the ResourceController for this Window. Calls to this function may
    ///     invalidate previously acquired pointers to the active ResourceController
    ///     (e.g., they can still point to the ResourceController of a different Window).
    void setActiveWindow( WindowPtr window );

    ///
    /// \brief Sets active Window of Context by Window name.
    /// \details Succeeding calls to getResourceController()
    ///     will return the ResourceController for this Window. Calls to this function may
    ///     invalidate previously acquired pointers to the active ResourceController
    ///     (e.g., they can still point to the ResourceController of a different Window).
    /// Throws ItemIdentityException if Window of specified name is not found.
    void setActiveWindow( const string& name );

    //
    // Static helper functions.

    ///
    /// \brief Creates a new Context instance and loads the specified render plugin.
    /// \return A std::unique_ptr containing the Context. The caller should maintain
    ///     the pointer until it is no longer needed and pass it to Context::Destroy().
    static std::unique_ptr<Context> Create( const RenderPlugin& renderPlugin );

    ///
    /// \brief Frees all associated memory and GPU resources of Context.
    static void Destroy( std::unique_ptr<Context> context );

    ///
    /// \brief Registers FrameListener object that will be called for every frame.
    static void RegisterFrameListener( FrameListener* listener );

    ///
    /// \brief Registers functor that will be called at the beginning of each frame, before rendering.
    static void RegisterFrameStartedCallback( FrameListenerController::FrameStartedCallback callback );

    ///
    /// \brief Registers functor that will be called at the end of each frame, after rendering.
    static void RegisterFrameEndedCallback( FrameListenerController::FrameEndedCallback callback );

    ///
    /// \brief Unregisters a previously registered FrameListener object.
    static void UnregisterFrameListener( FrameListener* listener );

    ///
    /// \brief Unregisters previously registered FrameStartedCallback functor.
    static void UnregisterFrameStartedCallback( FrameListenerController::FrameStartedCallback callback );

    ///
    /// \brief Unregisters previously registered FrameEndedCallback functor.
    static void UnregisterFrameEndedCallback( FrameListenerController::FrameEndedCallback callback );

    ///
    /// \brief Returns active window.
    static WindowPtr GetActiveWindow();

    //
    // Deleted functions/operators.

    Context& operator = ( const Context& rhs ) = delete;
    Context( const Context& rhs ) = delete;

    ///
    /// \brief Used for processing default keyboard shortcuts (such as for the DebugUI).
    void onKeyDown( const Keycode code ) override;

  protected:

    //
    // Notification handlers.

    ///
    /// \brief Handler for window event notifications.
    void onWindowEvent( const Notification& n );

    ///
    /// \brief To be called by derived render plugin Context classes, so the
    ///     Context object can know the rendering API version.
    void setAPIVersion( const int major, const int minor );

  protected:

    ///
    /// \brief Called on a render plugin error. Notifies all registered listeners.
    static void ErrorCallback( int error, const char* desc );

  protected:

    using SceneRegistry = Registry<std::unordered_map, Scene>;
    using WindowRegistry = Registry<std::map, Window>;

    using CameraMap = std::unordered_map<string, std::unique_ptr<Camera>>;
    using RendererMap = std::unordered_map <Lore::RendererType, std::unique_ptr<Lore::Renderer>>;

  protected:

    PoolCluster _poolCluster { "Primary" };
    std::unique_ptr<FrameListenerController> _frameListenerController { std::make_unique<FrameListenerController>() };

    WindowRegistry _windowRegistry {};
    SceneRegistry _sceneRegistry {};

    CameraMap _cameras { };
    RendererMap _renderers {};

    WindowPtr _activeWindow { nullptr };

    std::unique_ptr<InputController> _inputController { nullptr };
    std::unique_ptr<IRenderAPI> _renderAPI { nullptr };

    // True if one or more Windows exist in Context.
    bool _active { false };

  };

  ///
  /// \copydoc Context::Create()
  inline LORE_EXPORT std::unique_ptr<Context> CreateContext( const RenderPlugin& renderPlugin )
  {
    return Context::Create( renderPlugin );
  }

  ///
  /// \copydoc Context::Destroy()
  inline LORE_EXPORT void DestroyContext( std::unique_ptr<Context> context )
  {
    Context::Destroy( std::move( context ) );
  }

  // Helper macro to cleanup Context without having to call std::move().
#define DestroyLoreContext( c ) Lore::DestroyContext( std::move( c ) )

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
