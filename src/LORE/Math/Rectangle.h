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

namespace Lore {

  struct Rectangle
  {

    real x { 0.f };
    real y { 0.f };
    real w { 0.f };
    real h { 0.f };

    Rectangle() = default;
    Rectangle( const real x_,
               const real y_,
               const real w_,
               const real h_ )
      : x( x_ )
      , y( y_ )
      , w( w_ )
      , h( h_ )
    { }

    bool intersects( const Rectangle& rhs ) const
    {
      /*if ( x < rhs.x + rhs.w &&
           x + w > rhs.x &&
           y < rhs.y + rhs.h &&
           h + y > rhs.y ) {
        return true;
      }
      return false;*/
      return !( rhs.x > x + w ||
                rhs.x + rhs.w < x ||
                rhs.y > y + h ||
                rhs.y + rhs.h < y
              );
    }

  };

  using Rect = Rectangle;

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
