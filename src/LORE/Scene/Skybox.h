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

namespace Lore {

  class LORE_EXPORT SkyboxLayer final
  {

    glm::vec2 _parallax { 0.f, 0.f };
    real _depth { 1000.f };
    MaterialPtr _material { nullptr };
    std::shared_ptr<SpriteController> _spriteController { nullptr }; // TODO: Change to unique_ptr - using shared_ptr for now due to compile errors.
    string _name {};

  public:

    SkyboxLayer() = default;
    SkyboxLayer( const SkyboxLayer& rhs ) = default;

    SkyboxLayer( const string& name )
      : _name( name )
    {
    }

    ~SkyboxLayer() = default;

    //
    // Modifiers.

    inline void setParallax( const glm::vec2& parallax ) { _parallax = parallax; }
    inline void setDepth( const real depth ) { _depth = depth; }
    inline void setMaterial( MaterialPtr material ) { _material = material; }

    void setSprite( SpritePtr texture );
    void setScrollSpeed( const glm::vec2& speed );

    SpriteControllerPtr createSpriteController();

    //
    // Getters.

    inline glm::vec2 getParallax() const { return _parallax; }
    inline real getDepth() const { return _depth; }
    inline MaterialPtr getMaterial() const { return _material; }
    SpriteControllerPtr getSpriteController() const;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  using SkyboxLayerMap = std::map<string, SkyboxLayer>;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Skybox
  /// \brief Constitutes the entire skybox of a scene, can contain many layers
  ///   with different parallax settings and textures.
  class LORE_EXPORT Skybox final : public Alloc<Skybox>
  {

    LORE_OBJECT_BODY()

    SkyboxLayerMap _layers { };
    MaterialPtr _materialTemplate { nullptr };

  public:

    Skybox();
    ~Skybox() = default;

    //
    // Layers.

    SkyboxLayer& addLayer( const string& name );
    SkyboxLayer& getLayer( const string& name );
    void removeLayer( const string& name );
    void removeAllLayers();

    //
    // Setters.

    void setMaterialTemplate( const MaterialPtr material );

    //
    // Getters.

    inline const SkyboxLayerMap& getLayerMap() const
    {
      return _layers;
    }

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //