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

#include "ResourceController.h"

#include <LORE/Core/Context.h>
#include <LORE/Resource/Box.h>
#include <LORE/Resource/Entity.h>
#include <LORE/Resource/IO/ResourceFileProcessor.h>
#include <LORE/Resource/Sprite.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Resource/Textbox.h>
#include <LORE/Scene/IO/ModelLoader.h>
#include <LORE/Scene/SpriteController.h> // TODO: This should be in resources?

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceGroup::ResourceGroup( const string& name )
: _name( name )
{
  // Initialize resource registries for all resource types.
  _addResourceType<Box>();
  _addResourceType<Entity>();
  _addResourceType<Font>();
  _addResourceType<Material>();
  _addResourceType<Mesh>();
  _addResourceType<Model>();
  _addResourceType<GPUProgram>();
  _addResourceType<RenderTarget>();
  _addResourceType<Shader>();
  _addResourceType<Textbox>();
  _addResourceType<Texture>();
  _addResourceType<SpriteAnimationSet>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceGroup::addIndexedResource( const string& name, const IndexedResource& ir )
{
  _index.insert( { name, ir } );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const string& ResourceGroup::getName() const
{
  return _name;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const string ResourceController::DefaultGroupName = "Core";

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::ResourceController()
{
  // Create default resource group.
  auto rg = std::make_shared<ResourceGroup>(DefaultGroupName);

  // Store and set to active group.
  _groups.insert( { DefaultGroupName, rg } );
  _defaultGroup = rg.get();

  _workingDirectory = "./";
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::~ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::createGroup( const string& groupName )
{
  auto rg = std::make_shared<ResourceGroup>( groupName );
  _groups.insert( { groupName, rg } );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyGroup( const string& groupName )
{
  auto group = _getGroup( groupName );
  if ( DefaultGroupName != group->getName() ) {
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
  auto& resourceIndex = group->_index;
  for ( auto& it : resourceIndex ) {
    auto& indexedResource = it.second;
    if ( !indexedResource.loaded ) {
      loadQueues[static_cast< int >( indexedResource.type )].push_back( &indexedResource );
    }
  }

  auto LoadResourcesByType = [&, this] ( const SerializableResource type ) {
    auto& queue = loadQueues[static_cast< int >( type )];
    for ( auto& indexedResource : queue ) {
      ResourceFileProcessor processor( indexedResource->file, type );
      processor.load( groupName, this );
      indexedResource->loaded = true;
    }
  };

  // Load resource types in correct order so dependencies are ready.
  std::vector<SerializableResource> typeOrder = {
    SerializableResource::Cubemap,
    SerializableResource::Texture,
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
    LogWrite( Warning, "Group %s not found, not unloading", groupName.c_str() );
    return;
  }

  destroyAllInGroup<Box>( groupName );
  destroyAllInGroup<Entity>( groupName );
  destroyAllInGroup<Font>( groupName );
  destroyAllInGroup<GPUProgram>( groupName );
  destroyAllInGroup<Material>( groupName );
  destroyAllInGroup<Mesh>( groupName );
  destroyAllInGroup<Model>( groupName );
  destroyAllInGroup<RenderTarget>( groupName );
  destroyAllInGroup<Shader>( groupName );
  destroyAllInGroup<Sprite>( groupName );
  destroyAllInGroup<SpriteAnimationSet>( groupName );
  destroyAllInGroup<Texture>( groupName );
  destroyAllInGroup<Textbox>( groupName );

  // Set all indexed resources not loaded.
  for ( auto& it : group->_index ) {
    auto& indexedResource = it.second;
    indexedResource.loaded = false;
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
    LogWrite( Warning, "Not indexing file %s, not a valid file type", file.c_str() );
    return;
  }

  ResourceFileProcessor processor( file, resourceType );

  if ( processor.hasData() ) {
    // Store location of this resource file and its type.
    ResourceGroup::IndexedResource index;
    index.file = file;
    index.type = resourceType;
    index.loaded = false;

    // Insert index into resource group. This can be loaded/unloaded at will.
    auto group = _getGroup( groupName );
    group->addIndexedResource( processor.getName(), index );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceGroupPtr ResourceController::_getGroup( const string& groupName )
{
  auto lookup = _groups.find( groupName );
  if ( _groups.end() != lookup ) {
    return lookup->second.get();
  }

  LogWrite( Info, "Resource group %s not found, creating resource group", groupName.c_str() );
  auto rg = std::make_shared<ResourceGroup>( groupName );
  _groups.insert( { groupName, rg } );
  return rg.get();
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

string Resource::GetWorkingDirectory()
{
  return ActiveContext->getResourceController()->_workingDirectory;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::SetWorkingDirectory( const std::string& dir )
{
  ActiveContext->getResourceController()->_workingDirectory = dir;

  std::replace( ActiveContext->getResourceController()->_workingDirectory.begin(),
                ActiveContext->getResourceController()->_workingDirectory.end(),
                '\\', '/' );
  if ( !StringUtil::EndsWith( ActiveContext->getResourceController()->_workingDirectory, '/' ) ) {
    ActiveContext->getResourceController()->_workingDirectory.append( "/" );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr Resource::LoadFont( const string& name, const string& file, const uint32_t size, const string& groupName )
{
  auto font = ActiveContext->getResourceController()->create<Font>( name, groupName );
  font->loadFromFile( file, size );
  return font;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::LoadTexture( const string& name, const string& file, const string& groupName )
{
  auto texture = ActiveContext->getResourceController()->create<Texture>( name, groupName );
  texture->loadFromFile( file );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Resource::LoadEntity( const string& name, const string& path, const string& groupName )
{
  auto rc = ActiveContext->getResourceController();
  if ( rc->resourceExists<Entity>( name, groupName ) ) {
    LogWrite( Warning, "Entity %s already exists, returning existing entity", name.c_str() );
    return rc->get<Entity>( name, groupName );
  }
  auto entity = ActiveContext->getResourceController()->create<Entity>( name, groupName );

  // Give the entity a material.
  auto material = StockResource::GetMaterial( "StandardTextured3D" );
  auto entityMaterial = material->clone( "StandardTextured3D_" + name );
  entity->setMaterial( entityMaterial );

  // Load the specified model.
  ModelLoader loader( groupName );
  auto model = loader.load( path );
  entity->setModel( model );

  return entity;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

BoxPtr Resource::CreateBox( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->create<Box>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Resource::CreateEntity( const string& name, const Mesh::Type& modelType, const string& groupName )
{
  auto rc = ActiveContext->getResourceController();
  if ( rc->resourceExists<Entity>( name, groupName ) ) {
    LogWrite( Warning, "Entity %s already exists, returning existing entity", name.c_str() );
    return rc->get<Entity>( name, groupName );
  }
  auto entity = ActiveContext->getResourceController()->create<Entity>( name, groupName );

  // Lookup stock model and assign it.
  entity->setModel( StockResource::GetModel( modelType ) );

  // Set default material.
  switch ( modelType ) {

  default:
    break;

  case Mesh::Type::Quad:
  {
    auto material = StockResource::GetMaterial( "Standard2D" );
    entity->setMaterial( material->clone( "Standard2D_" + name ) );
  }
  break;

  case Mesh::Type::TexturedQuad:
  {
    auto material = StockResource::GetMaterial( "StandardTextured2D" );
    entity->setMaterial( material->clone( "StandardTextured2D_" + name ) );
  }
  break;

  case Mesh::Type::Cube:
  {
    auto material = StockResource::GetMaterial( "Standard3D" );
    entity->setMaterial( material->clone( "Standard3D_" + name ) );
  }
  break;

  case Mesh::Type::TexturedCube:
  {
    auto material = StockResource::GetMaterial( "StandardTextured3D" );
    entity->setMaterial( material->clone( "StandardTextured3D_" + name ) );
  }
  break;

  case Mesh::Type::Quad3D:
  {
    auto material = StockResource::GetMaterial( "Standard3D" );
    entity->setMaterial( material->clone( "Standard3D_" + name ) );
  }
  break;

  case Mesh::Type::TexturedQuad3D:
  {
    auto material = StockResource::GetMaterial( "StandardTextured3D" );
    entity->setMaterial( material->clone( "StandardTextured3D_" + name ) );
  }
  break;

  }

  return entity;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr Resource::CreateGPUProgram( const string& name, const string& groupName )
{
  auto program = ActiveContext->getResourceController()->create<GPUProgram>( name, groupName );
  program->init();
  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Resource::CreateMaterial( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->create<Material>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr Resource::CreateMesh( const string& name, const Mesh::Type& type, const string& groupName )
{
  auto mesh = ActiveContext->getResourceController()->create<Mesh>( name, groupName );
  mesh->init( type );
  return mesh;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Resource::CreateModel( const string& name, const Mesh::Type& type, const string& groupName )
{
  return ActiveContext->getResourceController()->create<Model>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderTargetPtr Resource::CreateRenderTarget( const string& name, const uint32_t width, const uint32_t height, const uint32_t sampleCount, const string& groupName )
{
  auto rt = ActiveContext->getResourceController()->create<RenderTarget>( name, groupName );
  rt->init( width, height, sampleCount );
  return rt;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderTargetPtr Resource::CreateDepthShadowMap( const string& name, const uint32_t width, const uint32_t height, const uint32_t sampleCount, const string& groupName )
{
  auto rt = ActiveContext->getResourceController()->create<RenderTarget>( name, groupName );
  rt->initDepthShadowMap( width, height, sampleCount );
  return rt;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderTargetPtr Resource::CreateDepthShadowCubemap( const string& name, const uint32_t width, const uint32_t height, const string& groupName )
{
  auto rt = ActiveContext->getResourceController()->create<RenderTarget>( name, groupName );
  rt->initDepthShadowCubemap( width, height );
  return rt;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ShaderPtr Resource::CreateShader( const string& name,
                                  const Shader::Type type,
                                  const string& groupName )
{
  auto shader = ActiveContext->getResourceController()->create<Shader>( name, groupName );
  shader->init( type );
  return shader;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpritePtr Resource::CreateSprite( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->create<Sprite>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteAnimationSetPtr Resource::CreateSpriteAnimationSet( const string& name,
                                                          const string& groupName )
{
  return ActiveContext->getResourceController()->create<SpriteAnimationSet>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::CreateTexture( const string& name, const uint32_t width, const uint32_t height, const uint32_t sampleCount, const string& groupName )
{
  auto texture = ActiveContext->getResourceController()->create<Texture>( name, groupName );
  texture->create( width, height, sampleCount );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::CreateTexture( const string& name, const uint32_t width, const uint32_t height, const Color& color, const string& groupName )
{
  auto texture = ActiveContext->getResourceController()->create<Texture>( name, groupName );
  texture->create( width, height, color );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::CreateDepthTexture( const string& name,
                                         const uint32_t width,
                                         const uint32_t height,
                                         const string& groupName )
{
  auto texture = ActiveContext->getResourceController()->create<Texture>( name, groupName );
  texture->createDepth( width, height );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::CreateCubemap( const string& name,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const string& groupName )
{
  auto texture = ActiveContext->getResourceController()->create<Texture>( name, groupName );
  texture->createCubemap( width, height );
  return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TextboxPtr Resource::CreateTextbox( const string& name, const string& groupName )
{
  auto textbox = ActiveContext->getResourceController()->create<Textbox>( name, groupName );
  textbox->setFont( Lore::StockResource::GetFont( "Default" ) );
  return textbox;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

BoxPtr Resource::GetBox( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Box>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Resource::GetEntity( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Entity>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr Resource::GetFont( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Font>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr Resource::GetGPUProgram( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<GPUProgram>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Resource::GetMaterial( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Material>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr Resource::GetMesh( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Mesh>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Resource::GetModel( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Model>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderTargetPtr Resource::GetRenderTarget( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<RenderTarget>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ShaderPtr Resource::GetShader( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Shader>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpritePtr Resource::GetSprite( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Sprite>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteAnimationSetPtr Resource::GetSpriteAnimationSet( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<SpriteAnimationSet>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr Resource::GetTexture( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Texture>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TextboxPtr Resource::GetTextbox( const string& name, const string& groupName )
{
  return ActiveContext->getResourceController()->get<Textbox>( name, groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyBox( BoxPtr box )
{
  ActiveContext->getResourceController()->destroy<Box>( box );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyEntity( EntityPtr entity )
{
  ActiveContext->getResourceController()->destroy<Entity>( entity );
}
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyFont( FontPtr font )
{
  ActiveContext->getResourceController()->destroy<Font>( font );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyGPUProgram( GPUProgramPtr program )
{
  ActiveContext->getResourceController()->destroy<GPUProgram>( program );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyMaterial( MaterialPtr material )
{
  ActiveContext->getResourceController()->destroy<Material>( material );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyMesh( MeshPtr mesh )
{
  ActiveContext->getResourceController()->destroy<Mesh>( mesh );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyModel( ModelPtr model )
{
  ActiveContext->getResourceController()->destroy<Model>( model );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyRenderTarget( RenderTargetPtr rt )
{
  ActiveContext->getResourceController()->destroy<RenderTarget>( rt );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyShader( ShaderPtr shader )
{
  ActiveContext->getResourceController()->destroy<Shader>( shader );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroySprite( SpritePtr sprite )
{
  ActiveContext->getResourceController()->destroy<Sprite>( sprite );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroySpriteAnimationSet( SpriteAnimationSetPtr sas )
{
  ActiveContext->getResourceController()->destroy<SpriteAnimationSet>( sas );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyTexture( TexturePtr texture )
{
  ActiveContext->getResourceController()->destroy<Texture>( texture );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyTextbox( TextboxPtr textbox )
{
  ActiveContext->getResourceController()->destroy<Textbox>( textbox );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::DestroyEntitiesInGroup( const string& groupName )
{
  ActiveContext->getResourceController()->destroyAllInGroup<Entity>( groupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceControllerPtr Resource::GetResourceController()
{
  return ActiveContext->getResourceController();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Resource::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
