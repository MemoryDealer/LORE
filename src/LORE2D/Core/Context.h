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

#include <LORE2D/Plugin/Plugins.h>
#include <LORE2D/Plugin/RenderPluginLoader.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Window/Window.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    class LORE_EXPORT Context
    {

    public:

        using ErrorListener = void (*)( int, const char* );

    public:

        explicit constexpr Context() noexcept;

        virtual ~Context();

        //
        // Rendering.

        virtual void renderFrame( const real dt ) = 0;

        //
        // Window functions.

        WindowPtr createWindow( const string& title,
                                const uint width,
                                const uint height,
                                const Window::Mode& mode = Window::Mode::Windowed );

        void destroyWindow( WindowPtr window );

        //
        // Information.

        virtual string getRenderPluginName() const = 0;

        //
        // Callbacks.

        void addErrorListener( ErrorListener listener );

        void removeErrorListener( ErrorListener listener );

        //
        // Getters.

        inline bool active() const
        {
            return _active;
        }

        //
        // Static helper functions.

        static std::unique_ptr<Context> Create( const RenderPlugin& renderPlugin );

        static void Destroy( std::unique_ptr<Context> context );

        //
        // Deleted functions/operators.

        Context& operator = ( const Context& rhs ) = delete;
        Context( const Context& rhs ) = delete;

    protected:

        static void ErrorCallback( int error, const char* desc );

    protected:

        Registry<Window> _windowRegistry;

        bool _active;

    };

    inline LORE_EXPORT std::unique_ptr<Context> CreateContext( const RenderPlugin& renderPlugin )
    {
        return Context::Create( renderPlugin );
    }

    inline LORE_EXPORT void DestroyContext( std::unique_ptr<Context> context )
    {
        Context::Destroy( std::move( context ) );
    }

#define DestroyLoreContext( c ) Lore::DestroyContext( std::move( c ) )

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
