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

#include <LORE/Resource/IResource.h>
#include <LORE/Resource/Texture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Sprite
  /// \brief A container of Textures which a Material can use.
  class LORE_EXPORT Sprite final : public Alloc<Sprite>,
                                   public IResource
  {

  public:

    Sprite() = default;
    ~Sprite() override = default;

    //
    // Modifiers.

    void addTexture( const Texture::Type type, const TexturePtr texture, const size_t frameIdx = 0, const real mixValue = 0.5f );
    void clearTextures();
    void setMixValue( const size_t frameIdx, const Texture::Type type, const size_t idx, const real value );

    //
    // Getters.

    TexturePtr getTexture( const size_t frameIdx, const Texture::Type type, const size_t idx = 0 ) const;
    size_t getTextureCount( const size_t frameIdx, const Texture::Type type ) const;
    real getMixValue( const size_t frameIdx, const Texture::Type type, const size_t idx ) const;

  private:

    using TextureList = std::vector<TexturePtr>;
    using MixValues = std::vector<real>;

    struct Frame
    {
      TextureList textures;
      MixValues mixValues;
    };

    using FrameData = std::map<Texture::Type, Frame>;

  private:

    std::vector<FrameData> _frames {};

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
