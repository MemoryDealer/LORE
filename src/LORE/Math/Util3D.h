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

namespace Lore {

  ///
  /// \brief Return the min/max points of provided points.
  template< typename Vec >
  std::pair< Vec, Vec > GetExtents( const Vec* pts, size_t stride, size_t count )
  {
    unsigned char* base = ( unsigned char* )pts;
    Vec pmin( *( Vec* )base );
    Vec pmax( *( Vec* )base );
    for ( size_t i = 0; i < count; ++i, base += stride ) {
      const Vec& pt = *( Vec* )base;
      pmin = glm::min( pmin, pt );
      pmax = glm::max( pmax, pt );
    }

    return make_pair( pmin, pmax );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \brief Centers geometry around the origin and scales it to fit in a size^3 box.
  template< typename Vec >
  void CenterAndScale( Vec* pts, size_t stride, size_t count, const typename Vec::value_type& size )
  {
    using Scalar = typename Vec::value_type;

    // get min/max extents
    std::pair< Vec, Vec > exts = GetExtents( pts, stride, count );

    // center and scale 
    const Vec center = ( exts.first * Scalar( 0.5 ) ) + ( exts.second * Scalar( 0.5f ) );

    const Scalar factor = size / glm::compMax( exts.second - exts.first );
    unsigned char* base = ( unsigned char* )pts;
    for ( size_t i = 0; i < count; ++i, base += stride ) {
      Vec& pt = *( Vec* )base;
      pt = ( ( pt - center ) * factor );
    }
  }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
