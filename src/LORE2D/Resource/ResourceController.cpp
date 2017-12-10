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

#include "ResourceController.h"

#include <LORE2D/Core/Context.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Renderable/Box.h>
#include <LORE2D/Resource/Renderable/Textbox.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const string ResourceController::DefaultGroupName = "Default";

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::ResourceController()
  : _groups()
  , _defaultGroup( nullptr )
  , _indexer( nullptr )
{
  // Create default resource group.
  auto rg = std::make_unique<ResourceGroup>( DefaultGroupName );

  // Store and set to active group.
  _groups.insert( { DefaultGroupName, std::move( rg ) } );
  _defaultGroup = rg.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::~ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::indexResourceFile( const string& file )
{
  // Parse JSON.
  // ...

  // Add each to group.
  // Have static methods to index resource files and load groups after
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::createGroup( const string& name )
{
  auto rg = std::make_unique<ResourceGroup>( name );

  _groups.insert( { name, std::move( rg ) } );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyGroup( const string& name )
{
  auto group = _getGroup( name );
  if ( DefaultGroupName != group->name ) {
    unloadGroup( name );
    _groups.erase( _groups.find( name ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::addResourceLocation( const string& file, const bool recursive )
{
  // Platform specific directory traversal.
  // ...
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::loadGroup( const string& name )
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::unloadGroup( const string& name )
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr ResourceController::createEntity( const string& name, const MeshType& type, const string& groupName )
{
  auto group = _getGroup( groupName );
  if ( group->entities.exists( name ) ) {
    return group->entities.get( name );
  }

  auto entity = MemoryAccess::GetPrimaryPoolCluster()->create<Entity>();
  entity->setName( name );
  entity->setResourceGroupName( groupName );

  // Lookup stock mesh and assign it.
  entity->setMesh( StockResource::GetMesh( type ) );

  // Set default material.
  switch ( type ) {

  default:
    break;

  case MeshType::Quad:
    entity->setMaterial( StockResource::CloneMaterial( "Standard", "Standard_" + name ) );
    break;

  case MeshType::TexturedQuad:
    entity->setMaterial( StockResource::CloneMaterial( "StandardTextured", "StandardTextured_" + name ) );
    break;

  }

  // Register entity and return it.
  group->entities.insert( name, entity );
  return entity;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr ResourceController::createMaterial( const string& name, const string& groupName )
{
  auto mat = MemoryAccess::GetPrimaryPoolCluster()->create<Material>();
  mat->setName( name );
  mat->setResourceGroupName( groupName );

  _getGroup( groupName )->materials.insert( name, mat );
  return mat;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

CameraPtr ResourceController::createCamera( const string& name, const string& groupName )
{
  auto cam = MemoryAccess::GetPrimaryPoolCluster()->create<Camera>();
  cam->setResourceGroupName( groupName ); // TODO: Camera should not be a resource (something else should hold the registry).
  cam->setName( name );

  _getGroup( groupName )->cameras.insert( name, cam );
  return cam;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

BoxPtr ResourceController::createBox( const string& name, const string& groupName )
{
  auto box = MemoryAccess::GetPrimaryPoolCluster()->create<Box>();
  box->setName( name );
  box->setResourceGroupName( groupName );

  _getGroup( groupName )->boxes.insert( name, box );
  return box;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TextboxPtr ResourceController::createTextbox( const string& name, const string& groupName )
{
  auto textbox = MemoryAccess::GetPrimaryPoolCluster()->create<Textbox>();
  textbox->setName( name );
  textbox->setResourceGroupName( groupName );
  textbox->setFont( Lore::StockResource::GetFont( "Default" ) );

  _getGroup( groupName )->textboxes.insert( name, textbox );
  return textbox;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

UIPtr ResourceController::createUI( const string& name, const string& groupName )
{
  auto ui = MemoryAccess::GetPrimaryPoolCluster()->create<UI>();
  ui->setName( name );
  ui->setResourceGroupName( groupName );

  _getGroup( groupName )->uis.insert( name, ui );
  return ui;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr ResourceController::createEntity( const string& name, const string& groupName )
{
  return createEntity( name, MeshType::Custom, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr ResourceController::createMesh( const string& name, const MeshType& meshType, const string& groupName )
{
  auto mesh = MemoryAccess::GetPrimaryPoolCluster()->create<Mesh>();
  mesh->setName( name );
  mesh->setResourceGroupName( groupName );

  // If this mesh type is a stock type, assign the corresponding vertex buffer.
  if ( MeshType::Custom != meshType ) {
    auto vb = _vertexBufferTable.at( meshType );
    mesh->setVertexBuffer( vb );
  }

  _getGroup( groupName )->meshes.insert( name, mesh );
  return mesh;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr ResourceController::cloneMaterial( const string& name, const string& cloneName )
{
  auto material = getMaterial( name );
  auto clone = createMaterial( cloneName );

  *clone = *material;
  return clone;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr ResourceController::getGPUProgram( const string& name, const string& groupName )
{
  return _getGroup( groupName )->programs.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr ResourceController::getMaterial( const string& name, const string& groupName )
{
  return _getGroup( groupName )->materials.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr ResourceController::getTexture( const string& name, const string& groupName )
{
  return _getGroup( groupName )->textures.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr ResourceController::getVertexBuffer( const string& name, const string& groupName )
{
  return _getGroup( groupName )->vertexBuffers.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr ResourceController::getFont( const string& name, const string& groupName )
{
  return _getGroup( groupName )->fonts.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceGroupPtr ResourceController::_getGroup( const string& groupName )
{
  auto lookup = _groups.find( groupName );
  if ( _groups.end() != lookup ) {
    return lookup->second.get();
  }

  log_warning( "Resource group " + groupName + " not found, using default resource group" );
  return _defaultGroup;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::LoadTexture( const string& name, const string& file, const string& groupName )
{
  return ActiveContext->getResourceController()->loadTexture( name, file, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr Resource::LoadFont( const string& name, const string& file, const uint32_t size, const string& groupName )
{
  return ActiveContext->getResourceController()->loadFont( name, file, size, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr Resource::CreateGPUProgram( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createGPUProgram( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ShaderPtr Resource::CreateVertexShader( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createVertexShader( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ShaderPtr Resource::CreateFragmentShader( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createFragmentShader( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr Resource::CreateVertexBuffer( const string& name, const MeshType& type, const string& groupName )
{
  return ActiveContext->getResourceController()->createVertexBuffer( name, type, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Resource::CreateMaterial( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createMaterial( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr Resource::CreateMesh( const string& name, const MeshType& meshType, const string& groupName )
{
  return ActiveContext->getResourceController()->createMesh( name, meshType, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::CreateTexture( const string& name, const uint32_t width, const uint32_t height, const string& groupName )
{
  return ActiveContext->getResourceController()->createTexture( name, width, height, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderTargetPtr Resource::CreateRenderTarget( const string& name, const uint32_t width, const uint32_t height, const string& groupName )
{
  return ActiveContext->getResourceController()->createRenderTarget( name, width, height, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

CameraPtr Resource::CreateCamera( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createCamera( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

BoxPtr Resource::CreateBox( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createBox( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TextboxPtr Resource::CreateTextbox( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createTextbox( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

UIPtr Resource::CreateUI( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createUI( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Resource::CreateEntity( const string& name, const MeshType& meshType, const string& groupName )
{
  return ActiveContext->getResourceController()->createEntity( name, meshType, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Resource::CreateEntity( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createEntity( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Resource::CloneMaterial( const string& name, const string& cloneName )
{
  return ActiveContext->getResourceController()->cloneMaterial( name, cloneName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr Resource::GetGPUProgram( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getGPUProgram( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Resource::GetMaterial( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getMaterial( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::GetTexture( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getTexture( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr Resource::GetFont( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getFont( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyTexture( TexturePtr texture )
{
  return ActiveContext->getResourceController()->destroyTexture( texture );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyTexture( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->destroyTexture( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
