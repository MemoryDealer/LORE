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

#include "Sprite.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Sprite::addTexture( const Texture::Type type, const TexturePtr texture, const size_t frameIdx, const real mixValue )
{
  if ( frameIdx < _frames.size() ) {
    // Add to existing frame.
    FrameData& data = _frames[frameIdx];
    auto& frame = data[type];
    frame.textures.push_back( texture );
    frame.mixValues.push_back( mixValue );
  }
  else if ( frameIdx == _frames.size() ) {
    // New frame.
    FrameData data;
    auto& frame = data[type];
    frame.textures.push_back( texture );
    frame.mixValues.push_back( mixValue );
    _frames.push_back( data );
  }
  else {
    LogWrite( Error, "Frame index %llu too large", TO_LLU( frameIdx ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Sprite::clearTextures()
{
  _frames.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Sprite::setMixValue( const size_t frameIdx, const Texture::Type type, const size_t idx, const real value )
{
  if ( frameIdx < _frames.size() ) {
    auto& frame = _frames[frameIdx][type];
    if ( idx < frame.mixValues.size() ) {
      frame.mixValues[idx] = value;
    }
    else {
      LogWrite( Error, "Mix value index %.2f too large, not setting mix value", value );
    }
  }
  else {
    LogWrite( Error, "Frame index %llu too large, not setting mix value", TO_LLU( frameIdx ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Sprite::getTexture( const size_t frameIdx, const Texture::Type type, const size_t idx ) const
{
  if ( frameIdx < _frames.size() ) {
    const FrameData& data = _frames[frameIdx];
    auto lookup = data.find( type );
    if ( data.end() != lookup ) {
      const Frame& frame = lookup->second;
      if ( idx < frame.textures.size() ) {
        return frame.textures[idx];
      }
    }
  }
  LogWrite( Error, "Failed to get texture at frame %llu, index %llu", TO_LLU( frameIdx ), TO_LLU( idx ) );
  return nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

u8 Sprite::getTextureCount( const size_t frameIdx, const Texture::Type type ) const
{
  if ( frameIdx < _frames.size() ) {
    const FrameData& data = _frames[frameIdx];
    auto lookup = data.find( type );
    if ( data.end() != lookup ) {
      const Frame& frame = lookup->second;
      return static_cast<u8>( frame.textures.size() );
    }
  }
  return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

real Sprite::getMixValue( const size_t frameIdx, const Texture::Type type, const size_t idx ) const
{
  if ( frameIdx < _frames.size() ) {
    const FrameData& data = _frames[frameIdx];
    auto lookup = data.find( type );
    if ( data.end() != lookup ) {
      const Frame& frame = lookup->second;
      if ( idx < frame.textures.size() ) {
        return frame.mixValues[idx];
      }
    }
  }
  return 1.0f; // Default 1.0 to maintain the prior texture when mixing.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
