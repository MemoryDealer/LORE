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

#include <LORE/Resource/IResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class RenderTarget
  /// \brief Can be rendered to instead of a window, and then sampled as a texture
  ///   in another RenderView.
  class LORE_EXPORT RenderTarget : public IResource
  {

    LORE_OBJECT_BODY()

  public:

    TexturePtr _texture { nullptr };
    uint32_t _width { 0 };
    uint32_t _height { 0 };
    real _aspectRatio { 0.f };
    u32 _sampleCount { 0 };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    RenderTarget() = default;
    virtual ~RenderTarget() = default;

    virtual void init( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) = 0;
    virtual void initDepthShadowMap( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) = 0;
    virtual void initDepthShadowCubemap( const uint32_t width, const uint32_t height ) = 0;
    virtual void initPostProcessing( const u32 width, const u32 height, const u32 sampleCount ) = 0;
    virtual void initDoubleBuffer( const u32 width, const u32 height, const u32 sampleCount ) = 0;

    virtual void bind( const u32 idx = 0 ) const = 0;
    virtual void flush() const = 0;

    virtual void setColorAttachmentCount( const u32 count ) = 0;

    virtual TexturePtr getTexture() const = 0;

    uint32_t getWidth() const
    {
      return _width;
    }

    uint32_t getHeight() const
    {
      return _height;
    }

    real getAspectRatio() const
    {
      return _aspectRatio;
    }

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
