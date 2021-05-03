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

#include "GLRenderTarget.h"

#include <LORE/Core/Context.h>
#include <Plugins/OpenGL/Resource/GLTexture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLRenderTarget::~GLRenderTarget()
{
  glDeleteRenderbuffers( 1, &_rbo );
  glDeleteFramebuffers( 1, &_fbo );
  if ( _texture ) {
    Resource::DestroyTexture( _texture );
  }

  if ( _multiSampling ) {
    glDeleteFramebuffers( 1, &_intermediateFBO );
    Resource::DestroyTexture( _intermediateTexture );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr GLRenderTarget::getTexture() const
{
  return ( _multiSampling ) ? _intermediateTexture : _texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::init( const uint32_t width, const uint32_t height, const uint32_t sampleCount )
{
  _width = width;
  _height = height;
  _aspectRatio = static_cast< real >( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, &_fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

  // Generate empty texture to bind to framebuffer.
  _texture = Resource::CreateTexture( _name + "_render_target", _width, _height, sampleCount, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  auto textureID = glTexturePtr->getID();

  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ( _multiSampling ) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, textureID, 0 );

  // Generate render buffer object for depth/stencil buffers.
  glGenRenderbuffers( 1, &_rbo );
  glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
  if ( _multiSampling ) {
    glRenderbufferStorageMultisample( GL_RENDERBUFFER, sampleCount, GL_DEPTH24_STENCIL8, width, height );
  }
  else {
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height );
  }
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo );

  if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
    throw Lore::Exception( "Failed to create framebuffer for RenderTarget" );
  }

  glBindRenderbuffer( GL_RENDERBUFFER, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

  if ( _multiSampling ) {
    // Configure second post-processing framebuffer.
    glGenFramebuffers( 1, &_intermediateFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, _intermediateFBO );

    _intermediateTexture = Resource::CreateTexture( _name + "_int_render_target", _width, _height, 0, getResourceGroupName() );
    auto intGLTexturePtr = ResourceCast<GLTexture>( _intermediateTexture );
    auto intTextureID = intGLTexturePtr->getID();
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intTextureID, 0 );
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initDepthShadowMap( const uint32_t width, const uint32_t height, const uint32_t sampleCount )
{
  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, &_fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

  // Generate empty texture to bind to framebuffer.
  _texture = Resource::CreateDepthTexture( _name + "_render_target", _width, _height, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  auto textureID = glTexturePtr->getID();

  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0 );

  // We're not using color data.
  glDrawBuffer( GL_NONE );
  glReadBuffer( GL_NONE );

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initDepthShadowCubemap( const uint32_t width, const uint32_t height )
{
  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;

  // Generate the cubemap first.
  _texture = Resource::CreateCubemap( _name + "_render_target_cubemap", _width, _height, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  auto cubemapID = glTexturePtr->getID();

  // Create a framebuffer.
  glGenFramebuffers( 1, &_fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
  glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemapID, 0 );

  // We're not using color data.
  glDrawBuffer( GL_NONE );
  glReadBuffer( GL_NONE );

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initPostProcessing( const u32 width, const u32 height, const u32 sampleCount )
{
  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, &_fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

  _texture = Resource::CreateFloatingPointBuffer( _name + "_post_processing_tex", width, height, sampleCount, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  auto textureID = glTexturePtr->getID();

  glBindTexture( GL_TEXTURE_2D, textureID );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ( _multiSampling ) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, textureID, 0 );

  // Generate render buffer object for depth/stencil buffers.
  glGenRenderbuffers( 1, &_rbo );
  glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
  if ( _multiSampling ) {
    glRenderbufferStorageMultisample( GL_RENDERBUFFER, sampleCount, GL_DEPTH_COMPONENT, _width, _height );
  }
  else {
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height );
  }
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo );

  if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
    throw Lore::Exception( "Failed to create framebuffer for RenderTarget" );
  }

  glBindRenderbuffer( GL_RENDERBUFFER, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

  if ( _multiSampling ) {
    // Configure second post-processing framebuffer.
    glGenFramebuffers( 1, &_intermediateFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, _intermediateFBO );

    _intermediateTexture = Resource::CreateFloatingPointBuffer( _name + "_int_render_target", _width, _height, 0, getResourceGroupName() );
    auto intGLTexturePtr = ResourceCast<GLTexture>( _intermediateTexture );
    auto intTextureID = intGLTexturePtr->getID();
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intTextureID, 0 );
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::bind() const
{
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::flush() const
{
  if ( _multiSampling ) {
    glBindFramebuffer( GL_READ_FRAMEBUFFER, _fbo );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _intermediateFBO );
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
