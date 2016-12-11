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

#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

    class Context final : public Lore::Context
    {

    public:

        explicit Context() noexcept;

        virtual ~Context() override;

        //
        // Rendering.

        virtual void renderFrame( const float dt ) override;

        //
        // Factory/Destruction functions.

        ///
        /// \brief Creates a window and returns a handle to it.
        virtual WindowPtr createWindow( const string& title,
                                        const uint width,
                                        const uint height,
                                        const Window::Mode& mode = Window::Mode::Windowed ) override;

        ///
        /// \brief Destroys specified window. If this is the last remaining window,
        ///     the context will no longer be active.
        virtual void destroyWindow( WindowPtr window ) override;

        ///
        /// \brief Creates a scene, assigns, the specified renderer, and returns a handle to it.
        virtual Lore::ScenePtr createScene( const string& name, const RendererType& rt ) override;

        //
        // Information.

        virtual string getRenderPluginName() const override;

    private:

        void _destroyWindow( WindowPtr window );

    private:

        using RendererMap = std::unordered_map <Lore::RendererType, std::unique_ptr<Lore::IRenderer>>;

    private:

        GLFWwindow* _offscreenContextWindow;
        RendererMap _renderers;

        std::queue<WindowPtr> _queuedClosingWindows;
    };

    // This is the only exported function from the render plugin, which allows the LORE2D 
    // library to load its version of Context.
    extern "C" __declspec( dllexport ) Lore::Context* __stdcall CreateContext()
    {
        Lore::Context* context = new Context();
        return context;
    }

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
