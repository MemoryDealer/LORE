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
  class LORE_EXPORT Sprite final : public Alloc<Sprite>, public IResource
  {

  public:

    using TextureList = std::vector<TexturePtr>;

  public:

    Sprite() = default;
    ~Sprite() override = default;

    //
    // Modifiers.

    void addTexture( const Texture::Type type, const TexturePtr texture )
    {
      _textures[type].push_back( texture );
    }

    void clearTextures()
    {
      _textures.clear();
    }

    //
    // Getters.

    TexturePtr getTexture( const Texture::Type type, const size_t idx ) const
    {
      auto it = _textures.find( type );
      if ( _textures.end() != it ) {
        return it->second.at( idx );
      }
      return nullptr;
    }

    size_t getTextureCount( const Texture::Type type ) const
    {
      auto it = _textures.find( type );
      if ( _textures.end() != it ) {
        return it->second.size();
      }
      return 0;
    }

  private:

    std::map<Texture::Type, TextureList> _textures;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
