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

#include <LORE2D/Scene/Scene.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::Node( const string& name, ScenePtr scene, NodePtr parent )
: _name( name )
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

void Node::attachNode( NodePtr node )
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

NodePtr Node::getChild( const string& name )
{
    auto lookup = _childNodes.find( name );
    if ( _childNodes.end() == lookup ) {
        throw Lore::Exception( "Child node " + name + " does not exist in node " + _name );
    }

    return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Node::ChildNodeIterator Node::getChildIterator()
{
    return ChildNodeIterator( _childNodes.begin(), _childNodes.end() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
