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

#include <LORE2D/Resource/IResource.h>
#include <LORE2D/Resource/Color.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT Box final : public Alloc<Box>, public IResource
  {

  public:

    Box() = default;
    ~Box() override = default;

    BoxPtr clone( const string& name );

    //
    // Getters.

    inline Color getBorderColor() const
    {
      return _borderColor;
    }

    inline Color getFillColor() const
    {
      return _fillColor;
    }

    inline real getBorderWidth() const
    {
      return _borderWidth;
    }

    inline real getWidth() const
    {
      return _size.x;
    }

    inline real getHeight() const
    {
      return _size.y;
    }

    inline Vec2 getSize() const
    {
      return _size;
    }

    //
    // Setters.

    inline void setBorderColor( const Color& color )
    {
      _borderColor = color;
    }

    inline void setBorderColor( const real r, const real g, const real b, const real a )
    {
      setBorderColor( Color( r, g, b, a ) );
    }

    inline void setFillColor( const Color& color )
    {
      _fillColor = color;
    }

    inline void setFillColor( const real r, const real g, const real b, const real a )
    {
      setFillColor( Color( r, g, b, a ) );
    }

    inline void setSize( const Vec2& size )
    {
      _size = size;
    }

    inline void setSize( const real w, const real h )
    {
      setSize( Vec2( w, h ) );
    }

    inline void setBorderWidth( const real width )
    {
      _borderWidth = width;
    }

  private:

    bool _fill { true };

    Color _borderColor { StockColor::White };
    Color _fillColor { 1.f, 1.f, 1.f, 0.3f };
    Vec2 _size { 1.f, 1.f };
    real _borderWidth { 0.04f };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
