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
#include <LORE2D/Resource/ResourceFileProcessor.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Renderable/Box.h>
#include <LORE2D/Resource/Renderable/Sprite.h>
#include <LORE2D/Resource/Renderable/Textbox.h>
#include <LORE2D/Scene/SpriteController.h> // TODO: This should be in resources?

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const string ResourceController::DefaultGroupName = "Core";

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::ResourceController()
{
  // Create default resource group.
  auto rg = std::make_shared<ResourceGroup>();
  rg->name = DefaultGroupName;

  // Store and set to active group.
  _groups.insert( { DefaultGroupName, rg } );
  _defaultGroup = rg.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::~ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::createGroup( const string& groupName )
{
  auto rg = std::make_shared<ResourceGroup>();
  rg->name = groupName;

  _groups.insert( { groupName, rg } );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyGroup( const string& groupName )
{
  auto group = _getGroup( groupName );
  if ( DefaultGroupName != group->name ) {
    unloadGroup( groupName );
    _groups.erase( _groups.find( groupName ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::loadGroup( const string& groupName )
{
  // Place indexed resources into queue so they can be loaded in the correct order.
  std::vector<ResourceGroup::IndexedResource*> loadQueues[static_cast< int >( SerializableResource::Count )];
  auto group = _getGroup( groupName );
  for ( auto& it : group->index ) {
    auto& index = it.second;
    if ( !index.loaded ) {
      loadQueues[static_cast< int >( index.type )].push_back( &index );
    }
  }

  auto LoadResourcesByType = [&, this] ( const SerializableResource type ) {
    auto& queue = loadQueues[static_cast< int >( type )];
    for ( auto& index : queue ) {
      ResourceFileProcessor processor( index->file, type );
      processor.load( groupName, this );
      index->loaded = true;
    }
  };

  // Load resource types in correct order so dependencies are ready.
  std::vector<SerializableResource> typeOrder = { SerializableResource::Texture,
    SerializableResource::Sprite,
    SerializableResource::SpriteList,
    SerializableResource::SpriteAnimation,
    SerializableResource::Font,
    SerializableResource::GPUProgram,
    SerializableResource::Material };
  for ( const auto& type : typeOrder ) {
    LoadResourcesByType( type );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::unloadGroup( const string& groupName )
{
  // Unload everything from the index and any other resources not indexed.
  auto group = _getGroup( groupName );
  if ( !group ) {
    return;
  }

  // Textures.
  auto textureIt = group->textures.getConstIterator();
  while ( textureIt.hasMore() ) {
    destroyTexture( textureIt.getNext() );
  }

  // Shaders, programs, and vertex buffers.
  auto vertexShaderIt = group->vertexShaders.getConstIterator();
  while ( vertexShaderIt.hasMore() ) {
    destroyVertexShader( vertexShaderIt.getNext() );
  }
  auto fragmentShaderIt = group->fragmentShaders.getConstIterator();
  while ( fragmentShaderIt.hasMore() ) {
    destroyFragmentShader( fragmentShaderIt.getNext() );
  }
  auto programIt = group->programs.getConstIterator();
  while ( programIt.hasMore() ) {
    destroyGPUProgram( programIt.getNext() );
  }
  auto vertexBufferIt = group->vertexBuffers.getConstIterator();
  while ( vertexBufferIt.hasMore() ) {
    destroyVertexBuffer( vertexBufferIt.getNext() );
  }

  // Meshes and entities.
  auto meshIt = group->meshes.getConstIterator();
  while ( meshIt.hasMore() ) {
    destroyMesh( meshIt.getNext() );
  }
  auto entityIt = group->entities.getConstIterator();
  while ( entityIt.hasMore() ) {
    destroyEntity( entityIt.getNext() );
  }

  // Fonts, boxes, and textboxes.
  auto fontIt = group->fonts.getConstIterator();
  while ( fontIt.hasMore() ) {
    destroyFont( fontIt.getNext() );
  }
  auto boxIt = group->boxes.getConstIterator();
  while ( boxIt.hasMore() ) {
    destroyBox( boxIt.getNext() );
  }
  auto textboxIt = group->textboxes.getConstIterator();
  while ( textboxIt.hasMore() ) {
    destroyTextbox( textboxIt.getNext() );
  }

  // Sprites and animation sets.
  auto spriteIt = group->sprites.getConstIterator();
  while ( spriteIt.hasMore() ) {
    destroySprite( spriteIt.getNext() );
  }
  auto animationSetIt = group->animationSets.getConstIterator();
  while ( animationSetIt.hasMore() ) {
    destroyAnimationSet( animationSetIt.getNext() );
  }

  // Render targets and UIs.
  auto rtIt = group->renderTargets.getConstIterator();
  while ( rtIt.hasMore() ) {
    destroyRenderTarget( rtIt.getNext() );
  }
  auto uiIt = group->uis.getConstIterator();
  while ( uiIt.hasMore() ) {
    destroyUI( uiIt.getNext() );
  }

  // Set all indexed resources not loaded.
  for ( auto& it : group->index ) {
    auto& index = it.second;
    index.loaded = false;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::reloadGroup( const string& groupName )
{
  unloadGroup( groupName );
  loadGroup( groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::loadResourceConfiguration( const string& file )
{
  ResourceFileProcessor::LoadConfiguration( file, this );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::indexResourceLocation( const string& directory, const string& groupName, const bool recursive )
{
  // Index all resource files.
  ResourceIndexer indexer( groupName );
  indexer.traverseDirectory( directory, this, recursive );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::indexResourceFile( const string& file, const string& groupName )
{
  // Validate file type.
  const auto resourceType = ResourceFileProcessor::GetResourceFileType( file );
  if ( SerializableResource::Count == resourceType ) {
    log_warning( "Not indexing file " + file + ", not a valid file type" );
    return;
  }

  ResourceFileProcessor processor( file, resourceType );

  // Store location of this resource file and its type.
  ResourceGroup::IndexedResource index;
  index.file = file;
  index.type = resourceType;
  index.loaded = false;

  // Insert index into resource group. This can be loaded/unloaded at will.
  auto group = _getGroup( groupName );
  group->index.insert( { processor.getName(), index } );
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

SpritePtr ResourceController::createSprite( const string& name, const string& groupName )
{
  auto sprite = MemoryAccess::GetPrimaryPoolCluster()->create<Sprite>();
  sprite->setName( name );
  sprite->setResourceGroupName( groupName );

  _getGroup( groupName )->sprites.insert( name, sprite );
  return sprite;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteAnimationSetPtr ResourceController::createAnimationSet( const string& name, const string& groupName )
{
  auto animationSet = MemoryAccess::GetPrimaryPoolCluster()->create<SpriteAnimationSet>();
  animationSet->setName( name );
  animationSet->setResourceGroupName( groupName );

  _getGroup( groupName )->animationSets.insert( name, animationSet );
  return animationSet;
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

MeshPtr ResourceController::createMesh( const string& name, const MeshType& meshType, const string& groupName )
{
  auto mesh = MemoryAccess::GetPrimaryPoolCluster()->create<Mesh>();
  mesh->setName( name );
  mesh->setResourceGroupName( groupName );

  // If this mesh type is a stock type, assign the corresponding vertex buffer.
  switch ( meshType ) {
  default:
  case MeshType::Custom:
    break;

  case MeshType::Quad:
    mesh->setVertexBuffer( getVertexBuffer( "Quad" ) );
    break;

  case MeshType::Text:
    mesh->setVertexBuffer( getVertexBuffer( "Text" ) );
    break;

  case MeshType::TexturedQuad:
    mesh->setVertexBuffer( getVertexBuffer( "TexturedQuad" ) );
    break;
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

bool ResourceController::textureExists( const string& name, const string& groupName )
{
  return _getGroup( groupName )->textures.exists( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr ResourceController::getGPUProgram( const string& name, const string& groupName )
{
  return _getGroup( groupName )->programs.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpritePtr ResourceController::getSprite( const string& name, const string& groupName )
{
  return _getGroup( groupName )->sprites.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteAnimationSetPtr ResourceController::getAnimationSet( const string& name, const string& groupName )
{
  return _getGroup( groupName )->animationSets.get( name );
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

void ResourceController::destroyMesh( MeshPtr mesh )
{
  auto groupName = mesh->getResourceGroupName();
  _getGroup( groupName )->meshes.remove( mesh->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Mesh>( mesh );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyEntity( EntityPtr entity )
{
  auto groupName = entity->getResourceGroupName();
  _getGroup( groupName )->entities.remove( entity->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Entity>( entity );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyBox( BoxPtr box )
{
  auto groupName = box->getResourceGroupName();
  _getGroup( groupName )->boxes.remove( box->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Box>( box );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyTextbox( TextboxPtr textbox )
{
  auto groupName = textbox->getResourceGroupName();
  _getGroup( groupName )->textboxes.remove( textbox->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Textbox>( textbox );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroySprite( SpritePtr sprite )
{
  auto groupName = sprite->getResourceGroupName();
  _getGroup( groupName )->sprites.remove( sprite->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<Sprite>( sprite );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyAnimationSet( SpriteAnimationSetPtr animationSet )
{
  auto groupName = animationSet->getResourceGroupName();
  _getGroup( groupName )->animationSets.remove( animationSet->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<SpriteAnimationSet>( animationSet );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyUI( UIPtr ui )
{
  auto groupName = ui->getResourceGroupName();
  _getGroup( groupName )->uis.remove( ui->getName() );

  MemoryAccess::GetPrimaryPoolCluster()->destroy<UI>( ui );
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

void Resource::CreateGroup( const string& groupName )
{
  return ActiveContext->getResourceController()->createGroup( groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::LoadGroup( const string& groupName )
{
  return ActiveContext->getResourceController()->loadGroup( groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::UnloadGroup( const string& groupName )
{
  return ActiveContext->getResourceController()->unloadGroup( groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::LoadResourceConfiguration( const string& file )
{
  return ActiveContext->getResourceController()->loadResourceConfiguration( file );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::IndexResourceLocation( const string& directory, const string& groupName, const bool recursive )
{
  return ActiveContext->getResourceController()->indexResourceLocation( directory, groupName, recursive );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::ReloadGroup( const string& groupName )
{
  return ActiveContext->getResourceController()->unloadGroup( groupName );
}

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

SpritePtr Resource::CreateSprite( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->createSprite( name, groupName );
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

SpritePtr Resource::GetSprite( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getSprite( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteAnimationSetPtr Resource::GetAnimationSet( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->getAnimationSet( name, groupName );
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
