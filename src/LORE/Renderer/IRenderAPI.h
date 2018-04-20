#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
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

#include <LORE/Resource/Material.h>
#include <LORE/Math/Rectangle.h>
#include <LORE/Resource/Color.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class IRenderAPI
  /// \brief Interface to render APIs to be implemented by render plugins.
  class IRenderAPI
  {

  public:

    enum ClearFlags
    {
      ColorBufferBit = 0,
      DepthBufferBit
    };

    enum PolygonMode
    {
      Fill,
      Line,
      Point
    };

    enum class CullingMode
    {
      None,
      Front,
      Back,
      FrontAndBack
    };

  public:

    virtual ~IRenderAPI() = default;

    //
    // General.

    virtual void setPolygonMode( const PolygonMode& mode ) = 0;

    virtual void setCullingMode( const CullingMode mode ) = 0;

    //
    // Viewport.

    virtual void clearColor( const real r,
                             const real g,
                             const real b,
                             const real a ) = 0;

    virtual void clear() = 0;

    virtual void setViewport( const uint32_t x,
                              const uint32_t y,
                              const uint32_t width,
                              const uint32_t height ) = 0;

    //
    // Framebuffers.

    virtual void bindDefaultFramebuffer() = 0;

    //
    // Depth testing.

    virtual void setDepthTestEnabled( const bool enabled ) = 0;

    //
    // Blending.

    virtual void setBlendingEnabled( const bool enabled ) = 0;

    virtual void setBlendingFunc( const Material::BlendFactor& src, const Material::BlendFactor& dst ) = 0;

    //
    // Debugging.
#ifdef _DEBUG
    
    virtual void getLastError( const string& prefix = "" ) = 0;

#endif

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
