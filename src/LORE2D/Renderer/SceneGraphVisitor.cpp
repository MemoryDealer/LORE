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

#include "SceneGraphVisitor.h"

#include <LORE2D/Renderer/IRenderer.h>
#include <LORE2D/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SceneGraphVisitor::SceneGraphVisitor( NodePtr root )
  : _stack()
  , _node( root )
{
  if ( _node->_transformDirty() ) {
    _node->_updateWorldTransform( _node->_getLocalTransform() );
  }

  // Recursively push down scale changes to child nodes. This must be done
  // here because scales are not included in transform/rotation matrix updates.
  _node->_updateChildrenScale();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SceneGraphVisitor::~SceneGraphVisitor()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneGraphVisitor::visit( IRenderer& renderer, bool parentDirty )
{
  const bool transformDirty = _node->_transformDirty();
  const Matrix4 transform = _node->_getLocalTransform();

  if ( parentDirty ) {
    _node->_updateWorldTransform( _stack.top() * transform );
  }
  else if ( transformDirty ) {
    _node->_updateWorldTransform( _stack.top() * transform );
    parentDirty = true;
  }

  // Add any Renderables attached to this node to the Renderer.
  auto it = _node->getEntityListConstIterator();
  while ( it.hasMore() ) {
    EntityPtr entity = it.getNext();
    renderer.addRenderData( entity, _node );
  }

  // Recurse over children.
  if ( _node->hasChildNodes() ) {
    // Push this node's transform onto the stack, so the next call can
    // use it to calculate its derived transform.
    _stack.push( transform );

    Node::ChildNodeIterator it = _node->getChildNodeIterator();
    while ( it.hasMore() ) {
      _node = it.getNext();
      visit( renderer, parentDirty );
    }

    // Recursion on this node's children is done, we can pop off the transform.
    _stack.pop();
  }

  // Manually update depth value (z) in world transformation matrix after
  // traversing all children.
  //_node->_updateDepthValue();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
