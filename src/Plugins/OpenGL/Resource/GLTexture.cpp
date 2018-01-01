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

#include "GLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Plugins/ThirdParty/stb_image.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLTexture::GLTexture()
: _id( 0 )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLTexture::~GLTexture()
{
    _reset();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::loadFromFile( const string& file )
{
    int width, height, n;
    unsigned char* pixels = stbi_load( file.c_str(), &width, &height, &n, STBI_rgb_alpha );
    if ( pixels ) {

        _createGLTexture( pixels, width, height );

        stbi_image_free( pixels );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
    else {
      throw Lore::Exception( "Unable to load texture at " + file );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::create( const uint32_t width, const uint32_t height )
{
  glGenTextures( 1, &_id );
  glBindTexture( GL_TEXTURE_2D, _id );

  // Create empty texture.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glBindTexture( GL_TEXTURE_2D, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::create( const int width, const int height, const Lore::Color& color )
{
    const auto size = width * height * 4;
    unsigned char* pixels = new unsigned char[size];

    const unsigned char ccolor[3] = {
        static_cast<unsigned char>( color.r * 255.f ),
        static_cast<unsigned char>( color.g * 255.f ),
        static_cast<unsigned char>( color.b * 255.f )
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

    _createGLTexture( pixels, width, height );

    delete [] pixels;
    glBindTexture( GL_TEXTURE_2D, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::bind()
{
    glBindTexture( GL_TEXTURE_2D, _id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::_createGLTexture( const unsigned char* pixels, const int width, const int height, const bool genMipMaps )
{
    glGenTextures( 1, &_id );
    glBindTexture( GL_TEXTURE_2D, _id );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Create the OpenGL texture.
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    if ( genMipMaps ) {
      glGenerateMipmap( GL_TEXTURE_2D );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLTexture::_reset()
{
    glDeleteTextures( 1, &_id );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //