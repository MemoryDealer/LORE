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

#include "GenericRenderer.h"

#include <LORE2D/Resource/Renderable.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::GenericRenderer()
: _queues()
, _activeQueues()
{
    // Initialize all available queues.
    _queues.resize( DefaultRenderQueueCount );

    // Active default queues.
    activateQueue( RenderQueue::General, _queues.at( RenderQueue::General ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::~GenericRenderer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::addRenderable( RenderablePtr r, Lore::Matrix4& model )
{
    const uint queueId = r->getRenderQueue();

    RenderQueue::Object obj( r, model );
    RenderQueue& queue = _queues.at( queueId );

    // Insert render queue object into list for associated material.
    queue.solids[r->getMaterial()].push_back( obj );

    // Add this queue to the active queue list if not already there.
    activateQueue( queueId, _queues[queueId] );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::present( const Lore::RenderView& rv )
{
    glViewport( rv.gl_viewport.x,
                rv.gl_viewport.y,
                rv.gl_viewport.width,
                rv.gl_viewport.height );

    Color bg = rv.scene->getBackgroundColor();
    glClearColor( bg.r, bg.g, bg.b, bg.a );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    NodePtr root = rv.scene->getRootNode();
    Node::ConstChildNodeIterator it = root->getConstChildNodeIterator();
    while ( it.hasMore() ) {
        NodePtr node = it.getNext();

        
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::activateQueue( const uint id, Lore::RenderQueue& rq )
{
    auto lookup = _activeQueues.find( id );
    if ( _activeQueues.end() == lookup ) {
        _activeQueues.insert( { id, rq } );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
