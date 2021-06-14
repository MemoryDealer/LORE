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

#include <Plugins/OpenGL/Resource/GLFont.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  constexpr auto ScaleFactor = 0.001f;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLFont::loadFromFile( const string& file, const uint32_t size )
{
  FT_Library ft;

  // Initialize FreeType library.
  if ( FT_Init_FreeType( &ft ) ) {
    throw Lore::Exception( "FREETYPE: Unable to initialize FreeType library" );
  }

  FT_Face face;
  if ( FT_New_Face( ft, file.c_str(), 0, &face ) ) {
    throw Lore::Exception( "FREETYPE: Unable to load font " + file );
  }

  FT_Set_Pixel_Sizes( face, 0, size * 8 );

  //
  // Generate glyph textures for each desired character using font.

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // Disable byte-alignment restriction.
  for ( GLubyte c = 0; c < 128; ++c ) {

    // Load a glyph.
    if ( FT_Load_Char( face, c, FT_LOAD_RENDER ) ) {
      LogWrite( Error, "Failed to load character %c", c );
      continue;
    }

    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RED,
                  face->glyph->bitmap.width,
                  face->glyph->bitmap.rows,
                  0,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  face->glyph->bitmap.buffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    Glyph glyph = {
      texture,
      static_cast<GLuint>( face->glyph->advance.x ),
      glm::tvec2<uint32_t>( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
      glm::tvec2<uint32_t>( face->glyph->bitmap_left, face->glyph->bitmap_top )
    };

    _glyphs.insert( { c, glyph } );
  }

  glBindTexture( GL_TEXTURE_2D, 0 );

  LogWrite( Info, "Successfully loaded font %s", file.c_str() );

  // Clean up FT resources.
  FT_Done_Face( face );
  FT_Done_FreeType( ft );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::Vertices GLFont::generateVertices( const char c,
                                                       const real x,
                                                       const real y,
                                                       const real scale )
{
  const Glyph& glyph = _glyphs.at( c );

  const GLfloat xpos = x + glyph.bearing.x * scale * ScaleFactor;
  const GLfloat ypos = y - ( glyph.size.y - glyph.bearing.y ) * scale * ScaleFactor;
  const GLfloat w = glyph.size.x * scale * ScaleFactor;
  const GLfloat h = glyph.size.y * scale * ScaleFactor;

  Lore::Vertices vertices = {
    xpos, ypos + h, 0.f, 0.f,
    xpos, ypos,     0.f, 1.f,
    xpos + w, ypos, 1.f, 1.f,
    xpos, ypos + h, 0.f, 0.f,
    xpos + w, ypos, 1.f, 1.f,
    xpos + w, ypos + h, 1.f, 0.f
  };

  // Copy will be avoided with RVO.
  return vertices;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLFont::bindTexture( const char c )
{
  const Glyph& glyph = _glyphs.at( c );
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, glyph.textureID );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

real GLFont::advanceGlyphX( const char c, const real x, const real scale )
{
  const Glyph& glyph = _glyphs.at( c );
  return x + ( glyph.advance >> 6 ) * ( scale * ScaleFactor ) + 0.005f;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

real GLFont::getWidth( const char c )
{
  const Glyph& glyph = _glyphs.at( c );
  return static_cast< float >( glyph.advance >> 6 ) * ScaleFactor + 0.005f;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
