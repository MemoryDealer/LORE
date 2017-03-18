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

#include "Scene.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Scene::Scene( const string& name )
: _name( name )
, _bgColor( StockColor::Black )
, _renderer( nullptr )
, _root( "root", this, nullptr )
, _nodes()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Scene::~Scene()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Scene::createNode( const string& name )
{
    // Note: Not using make_unique here because Node's new operator is private.
    std::unique_ptr<Node> node( new Node( name, this, nullptr ) );

    auto result = _nodes.insert( { name, std::move( node ) } );
    if ( !result.second ) {
        throw Lore::Exception( "Failed to insert node " + name + " into Scene " + _name );
    }

    NodePtr p = result.first->second.get();

    _root.attachChildNode( p );

    return p;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyNode( NodePtr node )
{
    destroyNode( node->getName() );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Scene::destroyNode( const string& name )
{
    auto lookup = _nodes.find( name );
    if ( _nodes.end() == lookup ) {
        throw Lore::Exception( "Tried to destroy node " + name + " that does not exist in scene + " + name );
    }

    // Detach node from parent before destroying.
    NodePtr node = lookup->second.get();
    node->detachFromParent();

    _nodes.erase( lookup );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

NodePtr Scene::getNode( const string& name )
{
    auto lookup = _nodes.find( name );
    if ( _nodes.end() == lookup ) {
        throw Lore::Exception( "Node " + name + " does not exist in Scene " + _name );
    }

    return lookup->second.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
