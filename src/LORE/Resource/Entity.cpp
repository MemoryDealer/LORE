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

#include "Entity.h"

#include <LORE/Resource/Material.h>
#include <LORE/Resource/ResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Entity::~Entity()
{
  // HACK?
  if ( _material ) {
    Resource::DestroyMaterial( _material );
  }

  disableInstancing();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Entity::clone( const string& name )
{
  auto rc = Resource::GetResourceController();
  auto entity = rc->create<Entity>( name, getResourceGroupName() );

  entity->_material = _material;
  entity->_model = _model;
  entity->_renderQueue = _renderQueue;

  return entity;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Entity::getInstancedModel() const
{
  return _instancedModel;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

size_t Entity::getInstanceCount() const
{
  return _instanceCount;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::enableInstancing( const size_t max )
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
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::disableInstancing()
{
  if ( isInstanced() ) {
    Resource::DestroyModel( _instancedModel );
    _instancedModel = nullptr;
  }
  _instanceCount = 0;
  _instanceControllerNode = nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::setInstanceControllerNode( const NodePtr node )
{
  _instanceControllerNode = node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::setSprite( SpritePtr sprite )
{
  _material->sprite = sprite;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::setMaterial( MaterialPtr material )
{
  _material = material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::setModel( ModelPtr buffer )
{
  _model = buffer;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Entity::getInstanceControllerNode() const
{
  return _instanceControllerNode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Entity::getMaterial() const
{
  return _material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr Entity::getModel() const
{
  return _model;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

uint Entity::getRenderQueue() const
{
  return _renderQueue;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Entity::isInstanced() const
{
  return !!( _instancedModel );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::updateInstancedMatrix( const size_t idx, const glm::mat4& matrix )
{
  _instancedModel->updateInstanced( idx, matrix );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::_notifyAttached( const NodePtr node )
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
