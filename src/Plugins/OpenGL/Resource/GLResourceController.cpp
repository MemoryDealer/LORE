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

#include "GLResourceController.h"

#include <LORE2D/Resource/Material.h>
#include <LORE2D/Scene/Camera.h>

#include <Plugins/OpenGL/Resource/GLFont.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/Renderable/GLTexture.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Shader/GLVertexBuffer.h>
#include <Plugins/OpenGL/Window/GLRenderTarget.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::ResourceController()
  : Lore::ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::~ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr ResourceController::loadTexture( const string& name, const string& file, const string& groupName )
{
  auto texture = MemoryAccess::GetPrimaryPoolCluster()->create<Texture, GLTexture>();
  texture->setName( name );
  texture->setResourceGroupName( groupName );
  texture->loadFromFile( file );

  _getGroup( groupName )->textures.insert( name, texture );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::FontPtr ResourceController::loadFont( const string& name, const string& file, const uint32_t size, const string& groupName )
{
  auto font = MemoryAccess::GetPrimaryPoolCluster()->create<Font, GLFont>();
  font->setName( name );
  font->setResourceGroupName( groupName );
  font->loadFromFile( file, size );

  _getGroup( groupName )->fonts.insert( name, font );
  return font;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr ResourceController::createGPUProgram( const string& name, const string& groupName )
{
  auto program = MemoryAccess::GetPrimaryPoolCluster()->create<GPUProgram, GLGPUProgram>();
  program->setName( name );
  program->setResourceGroupName( groupName );
  program->init();

  _getGroup( groupName )->programs.insert( name, program );
  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceController::createVertexShader( const string& name, const string& groupName )
{
  auto shader = MemoryAccess::GetPrimaryPoolCluster()->create<Shader, GLShader>();
  shader->setName( name );
  shader->setResourceGroupName( groupName );
  shader->init( Shader::Type::Vertex );

  _getGroup( groupName )->vertexShaders.insert( name, shader );
  return shader;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceController::createFragmentShader( const string& name, const string& groupName )
{
  auto shader = MemoryAccess::GetPrimaryPoolCluster()->create<Shader, GLShader>();
  shader->setName( name );
  shader->setResourceGroupName( groupName );
  shader->init( Shader::Type::Fragment );

  _getGroup( groupName )->fragmentShaders.insert( name, shader );
  return shader;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::VertexBufferPtr ResourceController::createVertexBuffer( const string& name, const Lore::MeshType& type, const string& groupName )
{
  auto vb = MemoryAccess::GetPrimaryPoolCluster()->create<VertexBuffer, GLVertexBuffer>();
  vb->setName( name );
  vb->setResourceGroupName( groupName );
  vb->init( type );

  _getGroup( groupName )->vertexBuffers.insert( name, vb );

  // If this vertex buffer is a stock type, index it in the hash table.
  // TODO: Clean this up, stock resources should not mix in here.
  // (Perhaps use functions for adding to resource group, e.g., addVertexBuffer().
  switch ( type ) {

  default:
    break;

  case MeshType::Quad:
  case MeshType::TexturedQuad:
    _vertexBufferTable.insert( { type, vb } );
    break;

  }

  return vb;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr ResourceController::createTexture( const string& name, const uint32_t width, const uint32_t height, const string& groupName )
{
  auto texture = MemoryAccess::GetPrimaryPoolCluster()->create<Texture, GLTexture>();
  texture->setName( name );
  texture->setResourceGroupName( groupName );
  texture->create( width, height );

  _getGroup( groupName )->textures.insert( name, texture );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr ResourceController::createTexture( const string& name, const uint32_t width, const uint32_t height, const Lore::Color& color, const string& groupName )
{
  auto texture = MemoryAccess::GetPrimaryPoolCluster()->create<Texture, GLTexture>();
  texture->setName( name );
  texture->setResourceGroupName( groupName );
  texture->create( width, height, color );

  _getGroup( groupName )->textures.insert( name, texture );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::RenderTargetPtr ResourceController::createRenderTarget( const string& name, const uint32_t width, const uint32_t height, const string& groupName )
{
  auto renderTarget = MemoryAccess::GetPrimaryPoolCluster()->create<RenderTarget, GLRenderTarget>();
  renderTarget->setName( name );
  renderTarget->setResourceGroupName( groupName );
  renderTarget->create( width, height );

  _getGroup( groupName )->renderTargets.insert( name, renderTarget );
  return renderTarget;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyTexture( Lore::TexturePtr texture )
{
  auto groupName = texture->getResourceGroupName();
  _getGroup( groupName )->textures.remove( texture->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Texture, GLTexture>( texture );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyTexture( const string& name, const string& groupName )
{
  auto texture = _getGroup( groupName )->textures.get( name );
  if ( texture ) {
    destroyTexture( texture );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
