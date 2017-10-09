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

#include "GLRenderTarget.h"

#include <LORE2D/Core/Context.h>
#include <Plugins/OpenGL/Resource/Renderable/GLTexture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLRenderTarget::~GLRenderTarget()
{
  _reset();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::create( const uint32_t width, const uint32_t height )
{
  _width = width;
  _height = height;
  _aspectRatio = static_cast< real >( _width ) / _height;

  // Create a framebuffer.
  glGenFramebuffers( 1, &_fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

  // Generate empty texture to bind to framebuffer.
  _texture = Lore::Resource::CreateTexture( _name + "_render_target", _width, _height );
  auto glTexturePtr = static_cast< GLTexture* >( _texture );
  auto textureID = glTexturePtr->getID();

  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0 );

  // Generate render buffer object for depth/stencil buffers.
  glGenRenderbuffers( 1, &_rbo );
  glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
  glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height );
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo );

  if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
    throw Lore::Exception( "Failed to create framebuffer for RenderTarget" );
  }

  glBindRenderbuffer( GL_RENDERBUFFER, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::bind()
{
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::_reset()
{
  glDeleteRenderbuffers( 1, &_rbo );
  glDeleteFramebuffers( 1, &_fbo );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
