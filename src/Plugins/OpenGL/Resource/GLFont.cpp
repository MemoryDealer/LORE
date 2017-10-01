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

#include <Plugins/OpenGL/Resource/GLFont.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

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

  FT_Set_Pixel_Sizes( face, 0, size );

  //
  // Generate glyph textures for each desired character using font.

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // Disable byte-alignment restriction.
  for ( GLubyte c = 0; c < 128; ++c ) {

    // Load a glyph.
    if ( FT_Load_Char( face, c, FT_LOAD_RENDER ) ) {
      log_error( "Failed to load character " + std::to_string( c ) );
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

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    Glyph glyph = {
      texture,
      static_cast<GLuint>( face->glyph->advance.x ),
      Lore::IVec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
      Lore::IVec2( face->glyph->bitmap_left, face->glyph->bitmap_top )
    };

    _glyphs.insert( { c, glyph } );
  }

  glBindTexture( GL_TEXTURE_2D, 0 );

  log_information( "Successfully loaded font " + file );

  // Clean up FT resources.
  FT_Done_Face( face );
  FT_Done_FreeType( ft );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLFont::_reset()
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
