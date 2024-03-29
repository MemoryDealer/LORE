#include <memory>

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

#include "Scene.h"

#include <LORE/Renderer/SceneGraphVisitor.h>
#include <LORE/Resource/Material.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Scene/AABB.h>
#include <LORE/Scene/SceneLoader.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Scene::Scene()
{
  _root._name = "root";
  _root._scene = this;

  // TODO: Get stock skybox
  _skybox = MemoryAccess::GetPrimaryPoolCluster()->create<Skybox>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Scene::~Scene()
{
  _bgColor = StockColor::Black;
  MemoryAccess::GetPrimaryPoolCluster()->destroy<Skybox>( _skybox );
  _renderer = nullptr;

  // Clear all nodes.
  auto nodeIt = _nodes.getIterator();
  while ( nodeIt.hasMore() ) {
    destroyNode( nodeIt.getNext() );
  }
  _nodes.clear();

  // Clear all lights.
  auto dirLightIt = _directionalLights.getIterator();
  while ( dirLightIt.hasMore() ) {
    destroyLight( dirLightIt.getNext() );
  }
  auto pointLightIt = _pointLights.getIterator();
  while ( pointLightIt.hasMore() ) {
    destroyLight( pointLightIt.getNext() );
  }
  auto spotLightIt = _pointLights.getIterator();
  while ( spotLightIt.hasMore() ) {
    destroyLight( spotLightIt.getNext() );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Scene::createNode( const string& name )
{
  // Note: Not using make_unique here because Node's new operator is private.
  auto node = MemoryAccess::GetPrimaryPoolCluster()->create<Node>();
  node->_name = name;
  node->_scene = this;
  node->_aabb = std::make_unique<AABB>(  node  );

  _nodes.insert( name, node );
  _root.attachChildNode( node );

  return node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyNode( NodePtr node )
{
  destroyNode( node->getName() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyNode( const string& name )
{
  auto node = _nodes.get( name );

  if ( node ) {
    // Detach node from parent before destroying.
    node->detachFromParent();
    MemoryAccess::GetPrimaryPoolCluster()->destroy<Node>( node );
    _nodes.remove( name );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Scene::getNode( const string& name )
{
  return _nodes.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DirectionalLightPtr Scene::createDirectionalLight( const string& name )
{
  auto light = MemoryAccess::GetPrimaryPoolCluster()->create<DirectionalLight>();
  light->setName( name );
  light->init();
  _directionalLights.insert( name, light );

  return light;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

PointLightPtr Scene::createPointLight( const string& name )
{
  auto light = MemoryAccess::GetPrimaryPoolCluster()->create<PointLight>();
  light->setName( name );
  light->init();
  _pointLights.insert( name, light );

  return light;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyLight( LightPtr light )
{
  destroyLight( light->getType(), light->getName() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyLight( const Light::Type type, const string& name )
{
  LightPtr light = nullptr;
  switch ( type ) {
  default:
    break;

  case Light::Type::Directional:
    light = _directionalLights.get( name );
    MemoryAccess::GetPrimaryPoolCluster()->destroy<DirectionalLight>( static_cast< DirectionalLightPtr >( light ) );
    _directionalLights.remove( name );
    break;

  case Light::Type::Point:
    light = _pointLights.get( name );
    MemoryAccess::GetPrimaryPoolCluster()->destroy<PointLight>( static_cast< PointLightPtr >( light ) );
    _pointLights.remove( name );
    break;

  case Light::Type::Spot:
    light = _spotLights.get( name );
    MemoryAccess::GetPrimaryPoolCluster()->destroy<SpotLight>( static_cast< SpotLightPtr >( light ) );
    _spotLights.remove( name );
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyAllLights()
{
  auto directionalLightIt = _directionalLights.getIterator();
  while ( directionalLightIt.hasMore() ) {
    destroyLight( directionalLightIt.getNext() );
  }
  auto pointLightIt = _pointLights.getIterator();
  while ( pointLightIt.hasMore() ) {
    destroyLight( pointLightIt.getNext() );
  }
  auto spotLightIt = _spotLights.getIterator();
  while ( spotLightIt.hasMore() ) {
    destroyLight( spotLightIt.getNext() );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::updateSceneGraph()
{
  // Traverse the scene graph and update object transforms.
  Lore::SceneGraphVisitor sgv( getRootNode() );
  sgv.visit( _renderer );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
