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

#include "Node.h"

#include <LORE/Resource/Box.h>
#include <LORE/Resource/Entity.h>
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
  node->_entities = _entities.clone();
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

  // Simulate attaching entities to this node.
  auto it = _entities.getConstIterator();
  while ( it.hasMore() ) {
    auto entity = it.getNext();
    entity->_notifyAttached( node );
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
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::getChild( const string& name )
{
  return _childNodes.get( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::ChildNodeIterator Node::getChildNodeIterator()
{
  return _childNodes.getIterator();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::ConstChildNodeIterator Node::getConstChildNodeIterator()
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

void Node::attachObject( EntityPtr entity )
{
  _entities.insert( entity->getName(), entity );
  entity->_notifyAttached( this );
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

void Node::setPosition( const Vec2& position )
{
  _transform.position = Vec3( position );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const real x, const real y )
{
  setPosition( Vec2( x, y ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //


void Node::setPosition( const Vec3& position )
{
  _transform.position = position;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const real x, const real y, const real z1 )
{
  setPosition( Vec2( x, y ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const Vec2& offset )
{
  _transform.position += Vec3( offset );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const real xOffset, const real yOffset )
{
  translate( Vec2( xOffset, yOffset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const Radian& angle, const TransformSpace& ts )
{
  rotate( Math::POSITIVE_Z_AXIS, angle, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const Degree& angle, const TransformSpace& ts )
{
  rotate( Math::POSITIVE_Z_AXIS, angle, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const Vec3& axis, const Radian& angle, const TransformSpace& ts )
{
  Quaternion q = Math::CreateQuaternion( axis, angle );
  rotate( q, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const Vec3& axis, const Degree& angle, const TransformSpace& ts )
{
  Quaternion q = Math::CreateQuaternion( axis, angle );
  rotate( q, ts );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::rotate( const Quaternion& q, const TransformSpace& ts )
{
  Quaternion qnorm = q;
  qnorm.normalize();

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

void Node::setScale( const Vec2& scale )
{
  _transform.scale = Vec3( scale );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const Vec2& s )
{
  _transform.scale *= Vec3( s );
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const real s )
{
  Vec2 ss( s, s );
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

Matrix4 Node::_getLocalTransform()
{
  if ( _transform.dirty ) {
    _transform.local = Math::CreateTransformationMatrix( _transform.position,
                                                         _transform.orientation );
    _transform.dirty = false;
  }

  return _transform.local;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_updateWorldTransform( const Matrix4& m )
{
  // Apply scaling to final world transform.
  Matrix4 s;
  s[0][0] = _transform.derivedScale.x;
  s[1][1] = _transform.derivedScale.y;

  _transform.world = m * s;
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