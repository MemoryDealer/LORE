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

#include <LORE/Math/Math.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/Color.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class Light final : public Alloc<Light>
  {

    LORE_OBJECT_BODY()

  public:

    Light() = default;
    ~Light() override = default;

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

    inline real getIntensity() const
    {
      return _intensity;
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

    inline void setAttenuation( const real intensity,
                                const real constant,
                                const real linear,
                                const real quadratic )
    {
      _intensity=intensity;
      _constant=constant;
      _linear=linear;
      _quadratic=quadratic;
    }

    inline void setIntensity( const real intensity )
    {
      _intensity=intensity;
    }

  private:

    friend class Node;

  private:

    Color _ambient { StockColor::Black };
    Color _diffuse { StockColor::White };
    Color _specular { StockColor::White };

    // Attenuation.
    real _constant { 1.f };
    real _linear { .09f };
    real _quadratic { .032f };
    real _intensity { 1.f };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
