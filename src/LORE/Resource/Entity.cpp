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
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

EntityPtr Entity::clone( const string& name )
{
  auto rc = Resource::GetResourceController();
  auto entity = rc->create<Entity>( name, getResourceGroupName() );

  entity->_material = _material;
  entity->_mesh = _mesh;
  entity->_renderQueue = _renderQueue;

  return entity;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr Entity::getInstancedVertexBuffer() const
{
  return _instancedVertexBuffer;
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
    throw Lore::Exception( "Instancing is already enabled" );
  }

  // Create an instanced vertex buffer.
  auto rc = Resource::GetResourceController();
  _instancedVertexBuffer = rc->create<VertexBuffer>( _name + "_instanced", getResourceGroupName() );
  _instancedVertexBuffer->initInstanced( VertexBuffer::Type::TexturedQuadInstanced, max );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::disableInstancing()
{
  if ( isInstanced() ) {
    auto rc = Resource::GetResourceController();

    rc->destroy<VertexBuffer>( _instancedVertexBuffer );
    _instancedVertexBuffer = nullptr;
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

void Entity::setMesh( MeshPtr mesh )
{
  _mesh = mesh;
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

MeshPtr Entity::getMesh() const
{
  return _mesh;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

uint Entity::getRenderQueue() const
{
  return _renderQueue;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Entity::isInstanced() const
{
  return !!( _instancedVertexBuffer );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Entity::updateInstancedMatrix( const size_t idx, const glm::mat4& matrix )
{
  _instancedVertexBuffer->updateInstanced( idx, matrix );
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
