// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

Node::Node( const string& name, ScenePtr scene, NodePtr parent )
: _name( name )
, _transform()
, _renderables()
, _scene( scene )
, _parent( parent )
, _childNodes()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::~Node()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::createChildNode( const string& name )
{
    NodePtr node = _scene->createNode( name );
    node->_parent = this;

    auto result = _childNodes.insert( { name, node } );
    if ( !result.second ) {
        throw Lore::Exception( "Failed to add child node " + name + " to " + _name );
    }

    return node;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::attachChildNode( NodePtr node )
{
    if ( node->_parent ) {
        throw Lore::Exception( "Node " + node->getName() +
                               " is already a child of " + node->getParent()->getName() );
    }

    auto result = _childNodes.insert( { node->getName(), node } );
    if ( !result.second ) {
        throw Lore::Exception( "Failed to attach node to node " + _name );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::removeChildNode( NodePtr node )
{
    auto lookup = _childNodes.find( node->getName() );
    if ( _childNodes.end() == lookup ) {
        throw Lore::Exception( "Tried to remove child node " + node->getName() + " from node " + _name +
                               ", which is not a child" );
    }

    _childNodes.erase( lookup );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Node::getChild( const string& name )
{
    auto lookup = _childNodes.find( name );
    if ( _childNodes.end() == lookup ) {
        throw Lore::Exception( "Child node " + name + " does not exist in node " + _name );
    }

    return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::ChildNodeIterator Node::getChildNodeIterator()
{
    return ChildNodeIterator( std::begin( _childNodes ), std::end( _childNodes ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::ConstChildNodeIterator Node::getConstChildNodeIterator() const
{
    return ConstChildNodeIterator( std::cbegin( _childNodes ), std::cend( _childNodes ) );
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
        _scene->getRenderer()->addRenderable( r, _transform.worldMatrix );
        break;
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setPosition( const Vec3& position )
{
    _transform.position = position;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::translate( const Vec3& offset )
{
    _transform.position += offset;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setScale( const Vec3& scale )
{
    _transform.scale = scale;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::scale( const Vec3& scale )
{
    _transform.scale += scale;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix4 Node::getTransformationMatrix()
{
    if ( _transform.dirty ) {
        _transform.matrix = CreateTransformationMatrix( _transform.position,
                                                        _transform.orientation,
                                                        _transform.scale );
        _transform.dirty = false;
    }

    return _transform.matrix;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix4 Node::getWorldTransformationMatrix()
{
    return _transform.worldMatrix;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::setWorldTransformationMatrix( const Matrix4& w )
{
    _transform.worldMatrix = w;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Node::dirty()
{
    _transform.dirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
