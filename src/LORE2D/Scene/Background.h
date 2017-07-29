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

#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Math/Vector.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Background
  /// \brief Constitutes the entire background of a scene, can contain many layers
  ///   with different parallax settings and textures.
  class LORE_EXPORT Background final : public Alloc<Background>
  {

    LORE_OBJECT_BODY()

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  public:

    // TODO: Put layer in memory pool?
    class LORE_EXPORT Layer
    {

    public:

      Layer() = default;

      Layer( const string& name )
        : _name( name )
      {
      }

      ~Layer() = default;

      //
      // Getters.

      inline Vec2 getParallax() const { return _parallax; }

      inline real getDepth() const { return _depth; }

      inline MaterialPtr getMaterial() const { return _material; }

      //
      // Setters.

      inline void setParallax( const Vec2& parallax ) { _parallax = parallax; }

      inline void setDepth( const real depth ) { _depth = depth; }

      inline void setMaterial( MaterialPtr material ) { _material = material; }

      void setTexture( TexturePtr texture );

      void setScrollSpeed( const Vec2& speed );

    private:

      Vec2 _parallax { 1.f, 1.f };
      real _depth { 750.f };
      MaterialPtr _material { nullptr };
      TexturePtr _texture { nullptr };
      string _name {};

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  public:

    using LayerMap = std::map<string, Layer>;

  public:

    Background();

    ~Background() = default;

    //
    // Layers.

    Layer& addLayer( const string& name );

    Layer& getLayer( const string& name );

    void removeLayer( const string& name );

    inline const LayerMap& getLayerMap() const
    {
      return _layers;
    }

  private:

    virtual void _reset() override { }

  private:

    LayerMap _layers { };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //