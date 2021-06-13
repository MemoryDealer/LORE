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

#include "GLRenderTarget.h"

#include <LORE/Core/Context.h>
#include <Plugins/OpenGL/Resource/GLTexture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLRenderTarget::~GLRenderTarget()
{
  glDeleteRenderbuffers( 1, &_rbo );
  glDeleteFramebuffers( _fboCount, _fbo );
  if ( _texture ) {
    Resource::DestroyTexture( _texture );
  }

  if ( _multiSampling ) {
    glDeleteFramebuffers( _fboCount, _intermediateFBO );
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
  initColorAttachments();

  _width = width;
  _height = height;
  _aspectRatio = static_cast< real >( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, _fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo[0] );

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
    glGenFramebuffers( 1, _intermediateFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, _intermediateFBO[0] );

    _intermediateTexture = Resource::CreateTexture( _name + "_int_render_target", _width, _height, 0, getResourceGroupName() );
    auto intGLTexturePtr = ResourceCast<GLTexture>( _intermediateTexture );
    auto intTextureID = intGLTexturePtr->getID();
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intTextureID, 0 );
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  _colorAttachments[0] = GL_COLOR_ATTACHMENT0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initDepthShadowMap( const uint32_t width, const uint32_t height, const uint32_t sampleCount )
{
  initColorAttachments();

  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, _fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo[0] );

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
  initColorAttachments();

  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;

  // Generate the cubemap first.
  _texture = Resource::CreateCubemap( _name + "_render_target_cubemap", _width, _height, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  auto cubemapID = glTexturePtr->getID();

  // Create a framebuffer.
  glGenFramebuffers( 1, _fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo[0] );
  glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemapID, 0 );

  // We're not using color data.
  glDrawBuffer( GL_NONE );
  glReadBuffer( GL_NONE );

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initPostProcessing( const u32 width, const u32 height, const u32 sampleCount )
{
  initColorAttachments();

  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 1, _fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo[0] );

  // Create textures and bind them to the frame buffer color attachments.
  _texture = Resource::CreateFloatingPointBuffer( _name + "_post_processing_tex", width, height, sampleCount, 2, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );
  glTexturePtr->bind( 0, 0 );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ( _multiSampling ) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, glTexturePtr->getID( 0 ), 0 );
  glTexturePtr->bind( 0, 1 );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, ( _multiSampling ) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, glTexturePtr->getID( 1 ), 0 );

  // Use render buffer object for depth/stencil buffers.
  glGenRenderbuffers( 1, &_rbo );
  glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
  if ( _multiSampling ) {
    glRenderbufferStorageMultisample( GL_RENDERBUFFER, sampleCount, GL_DEPTH_COMPONENT, _width, _height );
  }
  else {
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height );
  }
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo );

  // We are rendering pixels for blooming to the second color attachment for blurring, setup the state on this FBO.
  _colorAttachmentCount = 2;
  glDrawBuffers( _colorAttachmentCount, _colorAttachments );

  if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
    throw Lore::Exception( "Failed to create framebuffer for RenderTarget" );
  }

  glBindRenderbuffer( GL_RENDERBUFFER, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

  if ( _multiSampling ) {
    // Configure second post-processing framebuffer.
    glGenFramebuffers( 1, _intermediateFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, _intermediateFBO[0] );

    _intermediateTexture = Resource::CreateFloatingPointBuffer( _name + "_int_render_target", _width, _height, 0, 2, getResourceGroupName() );
    auto intGLTexturePtr = ResourceCast<GLTexture>( _intermediateTexture );
    intGLTexturePtr->bind( 0, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intGLTexturePtr->getID( 0 ), 0 );
    intGLTexturePtr->bind( 0, 1 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, intGLTexturePtr->getID( 1 ), 0 );

    // Also setup the color attachment state for the blit target FBO.
    glDrawBuffers( _colorAttachmentCount, _colorAttachments );

    if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
      throw Lore::Exception( "Failed to create intermediate framebuffer for RenderTarget" );
    }
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initDoubleBuffer( const u32 width, const u32 height, const u32 sampleCount )
{
  initColorAttachments();

  _width = width;
  _height = height;
  _aspectRatio = static_cast<real>( _width ) / _height;
  _multiSampling = !!( sampleCount );
  _sampleCount = sampleCount;

  // Create a framebuffer.
  glGenFramebuffers( 2, _fbo );

  // Create textures and bind them to the frame buffer color attachments.
  _texture = Resource::CreateFloatingPointBuffer( _name + "_post_processing_tex", width, height, sampleCount, 2, getResourceGroupName() );
  auto glTexturePtr = ResourceCast<GLTexture>( _texture );

  for ( int i = 0; i < 2; ++i ) {
    glBindFramebuffer( GL_FRAMEBUFFER, _fbo[i] );
    glTexturePtr->bind( 0, i );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ( _multiSampling ) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, glTexturePtr->getID( i ), 0 );
  }

  if ( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) {
    throw Lore::Exception( "Failed to create framebuffer for RenderTarget" );
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

  if ( _multiSampling ) {
    // Configure second post-processing framebuffer.
    glGenFramebuffers( 2, _intermediateFBO );

    _intermediateTexture = Resource::CreateFloatingPointBuffer( _name + "_int_render_target", _width, _height, 0, 2, getResourceGroupName() );
    auto intGLTexturePtr = ResourceCast<GLTexture>( _intermediateTexture );

    for ( int i = 0; i < 2; ++i ) {
      glBindFramebuffer( GL_FRAMEBUFFER, _intermediateFBO[i] );
      intGLTexturePtr->bind( 0, i );
      glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intGLTexturePtr->getID( i ), 0 );
    }
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  _fboCount = 2;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::bind( const u32 fboIdx ) const
{
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo[fboIdx] );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::flush() const
{
  if ( _multiSampling ) {
    for ( u32 i = 0; i < _fboCount; ++i ) {
      glBindFramebuffer( GL_READ_FRAMEBUFFER, _fbo[i] );
      glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _intermediateFBO[i] );

      // Blit each color attachment individually (otherwise only the first one will work).
      for ( u32 colorAttachment = 0; colorAttachment < _colorAttachmentCount; ++colorAttachment ) {
        glReadBuffer( GL_COLOR_ATTACHMENT0 + colorAttachment );
        glDrawBuffer( GL_COLOR_ATTACHMENT0 + colorAttachment );
        glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST );
      }

      // Restore the original framebuffer state.
      glDrawBuffers( _colorAttachmentCount, _colorAttachments );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::setColorAttachmentCount( const u32 count )
{
  assert( count <= MaxColorAttachments );
  _colorAttachmentCount = count;
  glDrawBuffers( _colorAttachmentCount, _colorAttachments );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLRenderTarget::initColorAttachments()
{
  for ( u32 i = 0; i < MaxColorAttachments; ++i ) {
    _colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
