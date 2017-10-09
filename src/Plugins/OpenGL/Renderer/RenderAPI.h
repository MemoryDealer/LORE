#pragma once
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

#include <LORE2D/Renderer/IRenderAPI.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class RenderAPI : public Lore::IRenderAPI
  {

  public:

    virtual ~RenderAPI() override = default;

    //
    // Viewport.

    virtual void clearColor( const real r,
                             const real g,
                             const real b,
                             const real a ) override;

    virtual void clear() override;

    virtual void setViewport( const uint32_t x,
                              const uint32_t y,
                              const uint32_t width,
                              const uint32_t height ) override;

    //
    // Framebuffers.

    virtual void bindDefaultFramebuffer() override;

    //
    // Depth testing.

    virtual void setDepthTestEnabled( const bool enabled ) override;

    //
    // Blending.

    virtual void setBlendingEnabled( const bool enabled ) override;

    virtual void setBlendingFunc( const Material::BlendFactor& src, const Material::BlendFactor& dst ) override;

    //
    // Debugging.
#ifdef _DEBUG

    virtual void getLastError( const string& prefix ) override
    {
      printf( "E: %s\t%d\n", prefix.c_str(), glGetError() );
    }

#endif

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
