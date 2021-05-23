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

#include "GLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Plugins/ThirdParty/stb_image.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLTexture::~GLTexture()
{
  glDeleteTextures( _texCount, _id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::loadFromFile( const string& file, const bool srgb )
{
  _target = GL_TEXTURE_2D;
  stbi_set_flip_vertically_on_load( 1 );

  int width, height, n;
  // Always load with four components so we can have a single code path (e.g., no changing GL_UNPACK_ALIGNMENT etc.).
  unsigned char* pixels = stbi_load( file.c_str(), &width, &height, &n, STBI_rgb_alpha );
  if ( pixels ) {
    glGenTextures( 1, _id );
    glBindTexture( _target, _id[0] );

    glTexImage2D( _target, 0, ( srgb ) ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    const bool genMipMaps = true;
    if ( genMipMaps ) {
      glGenerateMipmap( _target );
    }

    /*glTexParameteri( _target, GL_TEXTURE_WRAP_S, ( _format == GL_RGBA ) ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( _target, GL_TEXTURE_WRAP_T, ( _format == GL_RGBA ) ? GL_CLAMP_TO_EDGE : GL_REPEAT );*/
    glTexParameteri( _target, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( _target, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, ( genMipMaps ) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
    glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    stbi_image_free( pixels );
    glBindTexture( _target, 0 );
  }
  else {
    throw Lore::Exception( "Unable to load texture at " + file );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::loadCubemap( const std::vector<string>& files )
{
  _target = GL_TEXTURE_CUBE_MAP;

  // Generate cubemap texture.
  glGenTextures( 1, _id );
  glBindTexture( _target, _id[0] );

  // Load each cubemap texture.
  int width, height, comp;
  for ( uint32_t i = 0; i < files.size(); ++i ) {
    unsigned char* data = stbi_load( files[i].c_str(), &width, &height, &comp, 0 );
    if ( data ) {
      glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    ( STBI_rgb == comp ) ? GL_RGB : GL_RGBA,
                    width,
                    height,
                    0,
                    ( STBI_rgb == comp ) ? GL_RGB : GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data );
    }
    else {
      throw Lore::Exception( "Unable to load cubemap texture " + files[i] );
    }
    stbi_image_free( data );
  }

  glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( _target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( _target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( _target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

  glBindTexture( _target, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::create( const uint32_t width, const uint32_t height, const uint32_t sampleCount )
{
  glGenTextures( 1, _id );

  if ( sampleCount ) {
    _target = GL_TEXTURE_2D_MULTISAMPLE;
    glBindTexture( _target, _id[0] );

    // Create texture.
    glTexImage2DMultisample( _target, sampleCount, GL_RGB, width, height, GL_TRUE );
  }
  else {
    _target = GL_TEXTURE_2D;
    glBindTexture( _target, _id[0] );

    // Create texture.
    glTexImage2D( _target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );

    glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  }

  glBindTexture( _target, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::createDepth( const uint32_t width, const uint32_t height )
{
  glGenTextures( 1, _id );
  glBindTexture( GL_TEXTURE_2D, _id[0] );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

  glBindTexture( _target, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::createCubemap( const uint32_t width, const uint32_t height )
{
  _target = GL_TEXTURE_CUBE_MAP;
  glGenTextures( 1, _id );
  glBindTexture( _target, _id[0] );

  for ( uint32_t i = 0; i < 6; ++i ) {
    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
  }

  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

  glBindTexture( _target, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::createFloatingPoint( const u32 width, const u32 height, const u32 sampleCount, const u32 texCount )
{
  assert( texCount <= MaxTextures );

  if ( sampleCount ) {
    _target = GL_TEXTURE_2D_MULTISAMPLE;
    glGenTextures( texCount, _id );

    for ( u32 i = 0; i < texCount; ++i ) {
      glBindTexture( _target, _id[i] );

      // Create texture.
      glTexImage2DMultisample( _target, sampleCount, GL_RGBA16F, width, height, GL_TRUE );

      glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
  }
  else {
    _target = GL_TEXTURE_2D;
    glGenTextures( texCount, _id );

    for ( u32 i = 0; i < texCount; ++i ) {
      glBindTexture( _target, _id[i] );

      glTexImage2D( _target, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr );

      glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
  }

  glBindTexture( _target, 0 );

  _texCount = texCount;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::create( const int width, const int height, const Lore::Color& color )
{
  const auto size = width * height * 4;
  unsigned char* pixels = new unsigned char[size];

  const unsigned char ccolor[3] = {
    static_cast< unsigned char >( color.r * 255.f ),
    static_cast< unsigned char >( color.g * 255.f ),
    static_cast< unsigned char >( color.b * 255.f )
  };

  for ( int i = 0; i < size; ++i ) {
    switch ( i % 4 ) {
    case 0:
      pixels[i] = ccolor[0];
      break;

    case 1:
      pixels[i] = ccolor[1];
      break;

    case 2:
      pixels[i] = ccolor[2];
      break;

    case 3:
      pixels[i] = 255;
      break;
    }
  }

  _createGLTexture( pixels, width, height, false );

  delete[] pixels;
  glBindTexture( GL_TEXTURE_2D, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::bind( const u32 activeIdx, const u32 texIdx )
{
  glActiveTexture( GL_TEXTURE0 + activeIdx);
  glBindTexture( _target, _id[texIdx] );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::setDefaultActiveTexture()
{
  glActiveTexture( GL_TEXTURE0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::_createGLTexture( const unsigned char* pixels, const int width, const int height, const bool srgb, const bool genMipMaps )
{
  glGenTextures( 1, _id );
  glBindTexture( _target, _id[0] );

  // Create the OpenGL texture.
  glTexImage2D( _target, 0, ( srgb ) ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

  if ( genMipMaps ) {
    glGenerateMipmap( _target );
  }

  glTexParameteri( _target, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( _target, GL_TEXTURE_WRAP_T, GL_REPEAT );

  glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, ( genMipMaps ) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
  glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
