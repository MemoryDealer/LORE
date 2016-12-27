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

#include "SceneGraphVisitor.h"

#include <LORE2D/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SceneGraphVisitor::SceneGraphVisitor()
: _stack()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SceneGraphVisitor::~SceneGraphVisitor()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneGraphVisitor::pushMatrix( const Matrix4 m )
{
    _stack.push( m );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneGraphVisitor::visit( NodePtr node, bool worldDirty )
{
    Vec4 v = _stack.top()[0];
    v = v * v;
    if ( worldDirty ) {
        Matrix4 world = _stack.top() * node->getTransformationMatrix();
        node->setWorldTransformationMatrix( world );
    }
    else if ( node->isTransformDirty() ) {
        Matrix4 world = _stack.top() * node->getTransformationMatrix();
        node->setWorldTransformationMatrix( world );
        worldDirty = true;
    }

    if ( node->hasChildNodes() ) {
        _stack.push( node->getWorldTransformationMatrix() );

        Node::ChildNodeIterator it = node->getChildNodeIterator();
        while ( it.hasMore() ) {
            NodePtr child = it.getNext();
            visit( child, worldDirty );
        }
    }
    
    _stack.pop();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
