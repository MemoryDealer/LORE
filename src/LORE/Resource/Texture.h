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

#include <LORE/Resource/Color.h>
#include <LORE/Resource/IResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  // TODO: How to handle multi-texturing? With current rendering pipeline,
  // it may be best to have a link to the next texture for each texture object.
  // Then let the material inform the renderer to iterate the linked list.
  class LORE_EXPORT Texture : public IResource
  {

  public:

    enum class Type
    {
      Diffuse,
      Specular,
      Normal,
      Cubemap
    };

  public:

    Texture() = default;
    virtual ~Texture() = default;

    virtual void loadFromFile( const string& file, const bool srgb = true ) = 0;
    virtual void loadCubemap( const std::vector<string>& files ) = 0;

    virtual void create( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) = 0;
    virtual void create( const int width, const int height, const Color& color ) = 0;
    virtual void createDepth( const uint32_t width, const uint32_t ) = 0;
    virtual void createCubemap( const uint32_t width, const uint32_t height ) = 0;
    virtual void createFloatingPoint( const u32 width, const u32 height, const u32 sampleCount, const u32 texCount ) = 0;

    virtual void bind( const u32 activeIdx = 0, const u32 texIdx = 0 ) = 0;
    virtual void setDefaultActiveTexture() = 0;

    virtual unsigned int getID( const u32 idx = 0 ) const = 0;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
