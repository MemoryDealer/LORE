#pragma once
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

#include <LORE2D/Core/NotificationCenter.h>
#include <LORE2D/Plugin/Plugins.h>
#include <LORE2D/Plugin/RenderPluginLoader.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Window/Window.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Context
    /// \brief The owner of all Lore functionality. 
    class LORE_EXPORT Context
    {

    public:

        ///
        /// \typedef ErrorListener
        /// \brief Function pointer to an error listener callback.
        using ErrorListener = void (*)( int, const char* );

    public:

        explicit Context() noexcept;

        virtual ~Context();

        //
        // Rendering.

        ///
        /// \brief Renders a single frame using configured Windows and Scenes.
        virtual void renderFrame( const real dt ) = 0;

        //
        // Factory/Destruction functions.

        ///
        /// \brief Creates a window and returns a handle to it.
        virtual WindowPtr createWindow( const string& title,
                                        const uint width,
                                        const uint height,
                                        const Window::Mode& mode = Window::Mode::Windowed ) = 0;

        ///
        /// \brief Destroys specified window. If this is the last remaining window,
        ///     the context will no longer be active.
        virtual void destroyWindow( WindowPtr window ) = 0;

        ///
        /// \brief Creates a scene and returns a handle to it.
        virtual ScenePtr createScene( const string& name );

        ///
        /// \brief Destroys specified scene and all of its nodes.
        virtual void destroyScene( const string& name );
        virtual void destroyScene( ScenePtr scene );

        //
        // Information.

        ///
        /// \brief Returns name of the loaded render plugin.
        virtual string getRenderPluginName() const = 0;

        //
        // Callbacks.

        ///
        /// \brief Registers ErrorListener function pointer to be called when an error occurs.
        void addErrorListener( ErrorListener listener );

        ///
        /// \brief Removes ErrorListener function pointer from registered listeners.
        void removeErrorListener( ErrorListener listener );

        //
        // Getters.

        ///
        /// \brief Returns true if the context is active.
        inline bool active() const
        {
            return _active;
        }

        WindowPtr getWindow( const string& title )
        {
            return _windowRegistry.get( title );
        }

        ScenePtr getScene( const string& name )
        {
            return _sceneRegistry.get( name );
        }

        //
        // Static helper functions.

        ///
        /// \brief Creates a new Context instance and loads the specified render plugin.
        /// \return A std::unique_ptr containing the Context. The caller should maintain
        ///     the pointer until it is no longer needed and pass it to Context::Destroy().
        static std::unique_ptr<Context> Create( const RenderPlugin& renderPlugin );

        ///
        /// \brief Frees all associated memory of Context.
        static void Destroy( std::unique_ptr<Context> context );

        //
        // Deleted functions/operators.

        Context& operator = ( const Context& rhs ) = delete;
        Context( const Context& rhs ) = delete;

    protected:

        //
        // Notification handlers.

        ///
        /// \brief Handler for window event notifications.
        void onWindowEvent( const Notification& n );

    protected:

        ///
        /// \brief Called on a render plugin error. Notifies all registered listeners.
        static void ErrorCallback( int error, const char* desc );

    protected:

        Registry<Window> _windowRegistry;
        Registry<Scene> _sceneRegistry;

        bool _active;

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

#define DestroyLoreContext( c ) Lore::DestroyContext( std::move( c ) )

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
