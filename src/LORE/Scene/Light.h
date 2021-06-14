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

#include <LORE/Math/Math.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/Color.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class Light
  {

    LORE_OBJECT_BODY()

    friend class Node;

    enum class Type
    {
      Directional,
      Point,
      Spot
    };


    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    Color _ambient { StockColor::Black };
    Color _diffuse { StockColor::White };
    Color _specular { StockColor::White };

    RenderTargetPtr shadowMap {};

  protected:

    Type _type { Type::Directional };

  public:

    Light() = default;
    virtual ~Light();

    virtual void init() {}

    //
    // Getters.

    inline Color getAmbient() const
    {
      return _ambient;
    }

    inline Color getDiffuse() const
    {
      return _diffuse;
    }

    inline Color getSpecular() const
    {
      return _specular;
    }

    inline Type getType() const
    {
      return _type;
    }

    //
    // Setters.

    inline void setAmbient( const Color& color )
    {
      _ambient = color;
    }

    inline void setDiffuse( const Color& color )
    {
      _diffuse = color;
    }

    inline void setSpecular( const Color& color )
    {
      _specular = color;
    }

    virtual void updateShadowTransforms( const glm::vec3& pos ) {}

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class DirectionalLight final : public Light, public Alloc<DirectionalLight>
  {

    glm::vec3 _direction;

  public:

    glm::mat4 viewProj {};

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    DirectionalLight() = default;
    ~DirectionalLight() override = default;

    void init() override;

    inline void setDirection( const glm::vec3& direction )
    {
      _direction = direction;
    }

    inline void setDirection( const real x, const real y, const real z )
    {
      setDirection( glm::vec3( x, y, z ) );
    }

    inline glm::vec3 getDirection() const
    {
      return _direction;
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class MovableLight
  /// \brief A light that can be attached to a scene node.
  class MovableLight : public Light
  {

    // Attenuation.
    real _range { 1.f };
    real _constant { 1.f };
    real _linear { .7f };
    real _quadratic { 1.8f };

    real _intensity { 1.f };

  public:

    MovableLight() = default;
    ~MovableLight() override = default;

    inline void setAttenuation( const real range,
                                const real constant,
                                const real linear,
                                const real quadratic )
    {
      _range = range;
      _constant = constant;
      _linear = linear;
      _quadratic = quadratic;
    }

    inline void setIntensity( const real intensity )
    {
      _intensity = intensity;
    }

    inline real getConstant() const
    {
      return _constant;
    }

    inline real getLinear() const
    {
      return _linear;
    }

    inline real getQuadratic() const
    {
      return _quadratic;
    }

    inline real getRange() const
    {
      return _range;
    }

    inline real getIntensity() const
    {
      return _intensity;
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class PointLight final : public MovableLight, public Alloc<PointLight>
  {

  public:

    std::vector<glm::mat4> shadowTransforms;
    glm::mat4 _shadowProj;
    real shadowFarPlane = 250.f;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    PointLight() = default;
    ~PointLight() override = default;

    void init() override;
    void updateShadowTransforms( const glm::vec3& pos ) override;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class SpotLight : public MovableLight, public Alloc<SpotLight>
  {

  public:

    SpotLight()
    {
      _type = Type::Spot;
    }
    ~SpotLight() override = default;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
