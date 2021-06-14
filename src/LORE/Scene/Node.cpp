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

#include "Node.h"

#include <LORE/Resource/Box.h>
#include <LORE/Resource/Prefab.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/Textbox.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::Node()
{
  //_getLocalTransform();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::~Node()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::clone( const string& name, const bool cloneChildNodes )
{
  if ( !_parent ) {
    throw Lore::Exception( "Cannot clone the root node" );
  }

  auto node = MemoryAccess::GetPrimaryPoolCluster()->create<Node>();
  node->_name = name;
  node->_scene = _scene;
  node->_aabb = std::make_unique<AABB>( node );
  // TODO: Clone sprite controller...
  node->_transform = _transform;
  node->_depth = _depth;
  node->_prefabs = _prefabs.clone();
  node->_boxes = _boxes.clone();
  node->_textboxes = _textboxes.clone();
  node->_parent = _parent;
  if ( cloneChildNodes ) {
    // Clone all child nodes.
    auto it = _childNodes.getConstIterator();
    while ( it.hasMore() ) {
      auto childNode = it.getNext();
      const string childCloneName = name + "_" + childNode->getName();
      auto clonedChild = childNode->clone( childCloneName, true );
      node->_childNodes.insert( childCloneName, clonedChild );
    }
  }
  node->_lights = _lights.clone();

  // Simulate attaching prefabs to this node.
  auto it = _prefabs.getConstIterator();
  while ( it.hasMore() ) {
    auto prefab = it.getNext();
    prefab->_notifyAttached( node );
  }

  _parent->_childNodes.insert( name, node );
  _scene->_nodes.insert( name, node );
  
  return node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::createChildNode( const string& name )
{
  auto node = MemoryAccess::GetPrimaryPoolCluster()->create<Node>();
  node->_name = name;
  node->_scene = _scene;
  node->_parent = this;
  node->_aabb = std::make_unique<AABB>( node );

  _scene->_nodes.insert( name, node );
  _childNodes.insert( name, node );

  return node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachChildNode( NodePtr node )
{
  if ( node->_parent ) {
    throw Lore::Exception( "Node " + node->getName() +
                           " is already a child of " + node->getParent()->getName() );
  }

  _childNodes.insert( node->getName(), node );

  node->_parent = this;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::removeChildNode( NodePtr node )
{
  _childNodes.remove( node->getName() );
  if ( _scene ) {
    _scene->_nodes.remove( node->getName() );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::removeAllChildNodes()
{
  _childNodes.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::getChild( const string& name )
{
  return _childNodes.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ChildNodeIterator Node::getChildNodeIterator()
{
  return _childNodes.getIterator();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ConstChildNodeIterator Node::getConstChildNodeIterator()
{
  return _childNodes.getConstIterator();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::detachFromParent()
{
  if ( _parent ) {
    _parent->removeChildNode( this );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachObject( PrefabPtr prefab )
{
  _prefabs.insert( prefab->getName(), prefab );
  prefab->_notifyAttached( this );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachObject( LightPtr l )
{
  _lights.insert( l->getName(), l );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachObject( BoxPtr b )
{
  _boxes.insert( b->getName(), b );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachObject( TextboxPtr t )
{
  _textboxes.insert( t->getName(), t );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Node::intersects( NodePtr rhs ) const
{
  return _aabb->intersects( *rhs->_aabb );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setName( const string& name )
{
  _name = name;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const glm::vec2& position )
{
  _transform.position = glm::vec3( position.x, position.y, 0.f );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const real x, const real y )
{
  setPosition( glm::vec2( x, y ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //


void Node::setPosition( const glm::vec3& position )
{
  _transform.position = position;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const real x, const real y, const real z )
{
  setPosition( glm::vec3( x, y, z ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const glm::vec2& offset )
{
  _transform.position += glm::vec3( offset.x, offset.y, 0.f );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const real xOffset, const real yOffset )
{
  translate( glm::vec2( xOffset, yOffset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const glm::vec3& offset )
{
  _transform.position += offset;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const real xOffset, const real yOffset, const real zOffset )
{
  translate( glm::vec3( xOffset, yOffset, zOffset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setOrientation( const glm::quat& orientation, const TransformSpace& ts )
{
  _transform.orientation = orientation;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setOrientation( const glm::vec3& orientation, const TransformSpace& ts )
{
  rotate( glm::quat( glm::radians( orientation ) ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const real angle, const TransformSpace& ts )
{
  rotate( glm::vec3( 0.f, 0.f, 1.f ), angle, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const glm::vec3& axis, const real angle, const TransformSpace& ts )
{
  glm::quat q( 1.f, 0.f, 0.f, 0.f );
  q = glm::rotate( q, angle, axis );
  rotate( q, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const glm::quat& q, const TransformSpace& ts )
{
  glm::quat qnorm = glm::normalize( q );

  switch ( ts ) {
  case TransformSpace::Local:
    _transform.orientation = _transform.orientation * qnorm;
    break;

  case TransformSpace::Parent:
    _transform.orientation = qnorm * _transform.orientation;
    break;

  case TransformSpace::World:
    // ...
    break;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setScale( const glm::vec2& scale )
{
  setScale( glm::vec3( scale.x, scale.y, 1.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setScale( const glm::vec3& scale )
{
  _transform.scale = scale;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setScale( const real s )
{
  setScale( glm::vec3( s, s, s ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const glm::vec2& s )
{
  _transform.scale *= glm::vec3( s.x, s.y, 1.f );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const glm::vec3& s )
{
  _transform.scale *= s;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const real s )
{
  glm::vec3 ss( s, s, s );
  scale( ss );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::updateWorldTransform()
{
  _transform.derivedScale = _transform.scale;
  _updateWorldTransform( _getLocalTransform() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteControllerPtr Node::createSpriteController()
{
  _spriteController.reset(); // TODO: Necessary?
  _spriteController = std::make_unique<SpriteController>();
  return _spriteController.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Node::getName() const
{
  return _name;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

AABBPtr Node::getAABB() const
{
  return _aabb.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteControllerPtr Node::getSpriteController() const
{
  return _spriteController.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::mat4 Node::getFlipMatrix() const
{
  if ( !_spriteController ) {
    return glm::mat4( 1.f );
  }

  glm::vec3 scale( 1.f, 1.f, 1.f );
  if ( _spriteController->getXFlipped() ) {
    scale.x = -1.f;
  }
  if ( _spriteController->getYFlipped() ) {
    scale.y = -1.f;
  }

  glm::quat q( 1.f, 0.f, 0.f, 0.f );
  return Math::CreateTransformationMatrix( glm::vec3( 0.f ), q, scale );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_dirty()
{
  _transform.dirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Node::_transformDirty() const
{
  return _transform.dirty;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::mat4 Node::_getLocalTransform()
{
  if ( _transform.dirty ) {
    _transform.local = Math::CreateTransformationMatrix( _transform.position,
                                                         _transform.orientation,
                                                         glm::vec3( 1.f ) );
    _transform.dirty = false;
  }

  return _transform.local;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_updateWorldTransform( const glm::mat4& m )
{
  // Apply scaling to final world transform.
  glm::mat4 s( 1.f );
  s[0][0] = _transform.derivedScale.x;
  s[1][1] = _transform.derivedScale.y;
  s[2][2] = _transform.derivedScale.z;

  _transform.world = m * s;

  // Any point light's space transforms for shadows need updating as well.
  auto it = _lights.getIterator();
  while ( it.hasMore() ) {
    auto light = it.getNext();
    if ( Light::Type::Point == light->_type ) {
      light->updateShadowTransforms( getWorldPosition() );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_updateChildrenScale()
{
  if ( _parent && _parent->getName() == "root" ) {
    _transform.derivedScale = _transform.scale;
  }

  auto it = getChildNodeIterator();
  while ( it.hasMore() ) {
    auto node = it.getNext();
    auto scale = node->getScale();
    node->_transform.derivedScale = _transform.derivedScale * scale;

    // Recurse on all children.
    node->_updateChildrenScale();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_updateDepthValue()
{
  // Apply depth to z-value of the model matrix (overwrite SceneGraphVisitor transformations).
  // This is necessary for depth values on nodes to work correctly.
  _transform.world[3][2] = static_cast< real >( _depth );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
