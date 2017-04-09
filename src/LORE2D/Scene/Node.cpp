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

#include "Node.h"

#include <LORE2D/Renderer/IRenderer.h>
#include <LORE2D/Resource/Renderable/Renderable.h>
#include <LORE2D/Scene/Scene.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::Node()
: _transform()
, _renderables()
, _scene( nullptr )
, _parent( nullptr )
, _childNodes()
, _lights()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::~Node()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::createChildNode( const string& name )
{
    auto node = MemoryAccess::GetPrimaryPoolCluster()->create<Node>();
    node->setName( name );
    node->_scene = _scene;
    node->_parent = this;
    
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

void Node::attachObject( RenderablePtr r )
{
    switch ( r->getType() ) {
    default:
    case Renderable::Type::Unknown:
        log_error( "Attempted to attach unknown renderable " + r->getName() );
        break;

    case Renderable::Type::Texture:
        _renderables.insert( { r->getName(), r } );
        _scene->getRenderer()->addRenderable( r, _transform.world );
        r->_notifyAttached();
        break;

    case Renderable::Type::Light:
        _renderables.insert( { r->getName(), r } );
        _lights.push_back( static_cast<LightPtr>( r ) );
        r->_notifyAttached();
        break;
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const Vec2& position )
{
    _transform.position = position;
    _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const Vec2& offset )
{
    _transform.position += offset;
    _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const real xOffset, const real yOffset )
{
    _transform.position.x += xOffset;
    _transform.position.y += yOffset;
    _dirty();
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
    _transform.scale = scale;
    _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const Vec2& s )
{
    _transform.scale *= s;
    _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const real s )
{
    Vec2 ss( s, s );
    scale( ss );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::_reset()
{
    _transform = Transform();
    _renderables.clear();
    _scene = nullptr;
    _parent = nullptr;
    //_childNodes.clear();
}

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

        // Update attached light positions.
        for ( auto& light : _lights ) {
            light->_position.x = _transform.world[3][0];
            light->_position.y = _transform.world[3][1];
        }

        _transform.dirty = false;
    }

    return _transform.local;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix4 Node::_getWorldTransform() const
{
    return _transform.world;
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
