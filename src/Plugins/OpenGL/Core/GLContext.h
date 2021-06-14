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

#include <LORE/Lore.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class Context final : public Lore::Context
  {

    GLFWwindow* _offscreenContextWindow { nullptr };

  public:

    explicit Context() noexcept;
    ~Context() override;

    void initConfiguration() override;

    //
    // Rendering.

    void renderFrame( const float lagMultiplier = 1.f ) override;

    //
    // Factory/Destruction functions.

    ///
    /// \brief Creates a window and returns a handle to it.
    WindowPtr createWindow( const string& title,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const int sampleCount,
                                    const RendererType rendererTypeMask,
                                    const Window::Mode& mode = Window::Mode::Windowed ) override;

    ///
    /// \brief Destroys specified window. If this is the last remaining window,
    ///     the context will no longer be active.
    void destroyWindow( WindowPtr window ) override;

    ///
    /// \brief Creates a scene, assigns, the specified renderer, and returns a handle to it.
    Lore::ScenePtr createScene( const string& name, const RendererType& rt ) override;

    //
    // Information.

    string getRenderPluginName() const override;

  };

#if LORE_PLATFORM == LORE_WINDOWS
  // This is the only exported function from the render plugin, which allows the LORE 
  // library to load its version of Context.
  extern "C" __declspec( dllexport ) Lore::Context* __stdcall CreateContext()
  {
    Lore::Context* context = new Context();
    return context;
  }
#endif

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
