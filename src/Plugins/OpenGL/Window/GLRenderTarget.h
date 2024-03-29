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

#include <LORE/Memory/Alloc.h>
#include <LORE/Window/RenderTarget.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class GLRenderTarget : public Lore::RenderTarget,
                         public Alloc<GLRenderTarget>
  {

    static constexpr auto MaxFBO = 2;

    GLuint _fbo[MaxFBO] { 0 };
    u32 _fboCount = 1;
    GLuint _intermediateFBO[MaxFBO] { 0 };
    GLuint _rbo { 0 };

    TexturePtr _intermediateTexture { nullptr };

    bool _multiSampling { false };

    static constexpr auto MaxColorAttachments = 8;
    u32 _colorAttachmentCount { 1 };
    u32 _colorAttachments[MaxColorAttachments] {};

  public:

    GLRenderTarget() = default;
    ~GLRenderTarget() override;

    TexturePtr getTexture() const override;
    void init( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) override;
    void initDepthShadowMap( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) override;
    void initDepthShadowCubemap( const uint32_t width, const uint32_t height ) override;
    void initPostProcessing( const u32 width, const u32 height, const u32 sampleCount ) override;
    void initDoubleBuffer( const u32 width, const u32 height, const u32 sampleCount ) override;
    void bind( const u32 fboIdx = 0 ) const override;
    void flush() const override;
    void setColorAttachmentCount( const u32 count ) override;

    void initColorAttachments();

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
