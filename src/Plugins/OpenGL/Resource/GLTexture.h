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

#include <LORE/Resource/Texture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class GLTexture : public Lore::Texture,
                    public Alloc<GLTexture>
  {

  public:

    GLTexture() = default;

    ~GLTexture() override;

    void loadFromFile( const string& file, const bool srgb ) override;

    void loadCubemap( const std::vector<string>& files ) override;

    void create( const uint32_t width, const uint32_t height, const uint32_t sampleCount ) override;

    void create( const int width, const int height, const Color& color ) override;

    void createDepth( const uint32_t width, const uint32_t ) override;

    void createCubemap( const uint32_t width, const uint32_t height ) override;

    void createFloatingPoint( const u32 width, const u32 height, const u32 sampleCount ) override;

    void bind( const uint32_t idx ) override;

    void setDefaultActiveTexture() override;

    //
    // Getters.

    GLuint getID() const
    {
        return _id;
    }

  private:

    void _createGLTexture( const unsigned char* pixels, const int width, const int height, const bool srgb = true, const bool genMipMaps = true );

  private:

    GLuint _id { 0 };
    GLenum _target { GL_TEXTURE_2D };

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
