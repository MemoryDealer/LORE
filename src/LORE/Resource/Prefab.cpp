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

#include "Prefab.h"

#include <LORE/Resource/Material.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Prefab::~Prefab()
{
  // HACK?
  if ( _material ) {
    Resource::DestroyMaterial( _material );
  }

  disableInstancing();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

PrefabPtr Prefab::clone( const string& name )
{
  auto rc = Resource::GetResourceController();
  auto prefab = rc->create<Prefab>( name, getResourceGroupName() );

  prefab->_material = _material;
  prefab->_model = _model;
  prefab->_renderQueue = _renderQueue;

  return prefab;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Prefab::getInstancedModel() const
{
  return _instancedModel;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

size_t Prefab::getInstanceCount() const
{
  return _instanceCount;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::enableInstancing( const size_t max )
{
  if ( isInstanced() ) {
   LogWrite( Info, "Instancing is already enabled" );
   return;
  }

  // Create an instanced model.
  auto rc = Resource::GetResourceController();
  _instancedModel = rc->create<Model>( _name + "_instanced", getResourceGroupName() );

  const std::map< Mesh::Type, Mesh::Type> InstancedModelMap = {
    { Mesh::Type::Quad, Mesh::Type::QuadInstanced },
    { Mesh::Type::TexturedQuad, Mesh::Type::TexturedQuadInstanced },
    { Mesh::Type::Quad3D, Mesh::Type::Quad3DInstanced },
    { Mesh::Type::TexturedQuad3D, Mesh::Type::TexturedQuad3DInstanced },
    { Mesh::Type::Cube, Mesh::Type::CubeInstanced },
    { Mesh::Type::TexturedCube, Mesh::Type::TexturedCubeInstanced }
  };

  // Determine which type of instanced model to use.
  const auto lookup = InstancedModelMap.find( _model->getType() );
  if ( InstancedModelMap.end() != lookup ) {
    auto mesh = rc->create<Mesh>( _name + "_instanced", getResourceGroupName() );
    mesh->initInstanced( lookup->second, max );
    _instancedModel->attachMesh( mesh );
  }
  else {
    // Custom mesh.
    for ( const auto& mesh : _model->_meshes ) {
      mesh->initInstanced( Mesh::Type::CustomInstanced, max );
      _instancedModel->attachMesh( mesh );
    }
  }

  // Acquire the correct GPUProgram for this instanced model type.
  switch ( _instancedModel->getType() ) {
  default:
    throw Lore::Exception( "Instanced prefab must have an instanced model" );

  case Mesh::Type::TexturedQuadInstanced:
    _material->program = StockResource::GetGPUProgram( "StandardTexturedInstanced2D" );
    break;

  case Mesh::Type::QuadInstanced:
    _material->program = StockResource::GetGPUProgram( "StandardInstanced2D" );
    break;

  case Mesh::Type::Quad3DInstanced:
  case Mesh::Type::CubeInstanced:
    _material->program = StockResource::GetGPUProgram( "StandardInstanced3D" );
    break;

  case Mesh::Type::TexturedQuad3DInstanced:
  case Mesh::Type::TexturedCubeInstanced:
  case Mesh::Type::CustomInstanced:
    if ( _material->sprite->getTextureCount( 0, Texture::Type::Cubemap ) ) {
      if ( _material->program->getName() == "Reflect3D" ) {
        _material->program = StockResource::GetGPUProgram( "Reflect3DInstanced" );
      }
      else if ( _material->program->getName() == "Refract3D" ) {
        _material->program = StockResource::GetGPUProgram( "Refract3DInstanced" );
      }
      return;
    }
    if ( _material->sprite->getTextureCount( 0, Texture::Type::Normal ) > 0 ) {
      _material->program = StockResource::GetGPUProgram( "StandardTexturedNormalMappingInstanced3D" );
    }
    else {
      _material->program = StockResource::GetGPUProgram( "StandardTexturedInstanced3D" );
    }
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::disableInstancing()
{
  if ( isInstanced() ) {
    Resource::DestroyModel( _instancedModel );
    _instancedModel = nullptr;
  }
  _instanceCount = 0;
  _instanceControllerNode = nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::setInstanceControllerNode( const NodePtr node )
{
  _instanceControllerNode = node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::setSprite( SpritePtr sprite )
{
  _material->sprite = sprite;

  // Program was assigned in Resource::LoadPrefab()...if normal maps are
  // detected auto-switch to the normal mapping program.
  if ( _material->sprite->getTextureCount( 0, Texture::Type::Normal ) > 0 ) {
    _material->program = StockResource::GetGPUProgram( "StandardTexturedNormalMapping3D" );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::setMaterial( MaterialPtr material )
{
  _material = material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::setModel( ModelPtr buffer )
{
  _model = buffer;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Prefab::getInstanceControllerNode() const
{
  return _instanceControllerNode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Prefab::getMaterial() const
{
  return _material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Prefab::getModel() const
{
  return _model;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

uint Prefab::getRenderQueue() const
{
  return _renderQueue;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Prefab::isInstanced() const
{
  return !!( _instancedModel );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::updateInstancedMatrix( const size_t idx, const glm::mat4& matrix )
{
  _instancedModel->updateInstanced( idx, matrix );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Prefab::_notifyAttached( const NodePtr node )
{
  if ( isInstanced() ) {
    node->_instanceID = _instanceCount++;
    if ( 0 == node->_instanceID ) {
      // First attached node becomes the instance controller node.
      _instanceControllerNode = node;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
