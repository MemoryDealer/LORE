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

#include "RenderAPI.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static GLenum ConvertBlendFactor( const Lore::Material::BlendFactor& factor )
  {
    switch ( factor ) {
    default:
    case Lore::Material::BlendFactor::Zero:
      return GL_ZERO;

    case Lore::Material::BlendFactor::One:
      return GL_ONE;

    case Lore::Material::BlendFactor::SrcColor:
      return GL_SRC_COLOR;

    case Lore::Material::BlendFactor::OneMinusSrcColor:
      return GL_ONE_MINUS_SRC_COLOR;

    case Lore::Material::BlendFactor::DstColor:
      return GL_DST_COLOR;

    case Lore::Material::BlendFactor::OneMinusDstColor:
      return GL_ONE_MINUS_DST_COLOR;

    case Lore::Material::BlendFactor::SrcAlpha:
      return GL_SRC_ALPHA;

    case Lore::Material::BlendFactor::OneMinusSrcAlpha:
      return GL_ONE_MINUS_SRC_ALPHA;

    case Lore::Material::BlendFactor::ConstantColor:
      return GL_CONSTANT_COLOR;

    case Lore::Material::BlendFactor::OneMinusConstantColor:
      return GL_ONE_MINUS_CONSTANT_COLOR;

    case Lore::Material::BlendFactor::ConstantAlpha:
      return GL_CONSTANT_ALPHA;

    case Lore::Material::BlendFactor::OneMinusConstantAlpha:
      return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::clearColor( const real r,
                            const real g,
                            const real b,
                            const real a )
{
  glClearColor( r, g, b, a );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::clear()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::setViewport( const uint32_t x,
                             const uint32_t y,
                             const uint32_t width,
                             const uint32_t height )
{
  glViewport( x, y, width, height );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::setDepthTestEnabled( const bool enabled )
{
  if ( enabled ) {
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
  }
  else {
    glDisable( GL_DEPTH_TEST );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::setBlendingEnabled( const bool enabled )
{
  if ( enabled ) {
    glEnable( GL_BLEND );
  }
  else {
    glDisable( GL_BLEND );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderAPI::setBlendingFunc( const Lore::Material::BlendFactor& src, const Lore::Material::BlendFactor& dst )
{
  glBlendFuncSeparate( ConvertBlendFactor( src ), ConvertBlendFactor( dst ), GL_ONE, GL_ONE );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
