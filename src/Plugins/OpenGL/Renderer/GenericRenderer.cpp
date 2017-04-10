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

#include "GenericRenderer.h"

#include <LORE2D/Math/Math.h>
#include <LORE2D/Renderer/SceneGraphVisitor.h>
#include <LORE2D/Resource/Renderable/Renderable.h>
#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Scene/Light.h>
#include <LORE2D/Scene/Scene.h>
#include <LORE2D/Shader/GPUProgram.h>

#include <Plugins/OpenGL/Math/MathConverter.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Window/GLWindow.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::GenericRenderer()
: _queues()
, _activeQueues()
{
    // Initialize all available queues.
    _queues.resize( DefaultRenderQueueCount );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::~GenericRenderer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::addRenderable( RenderablePtr r, NodePtr node )
{
    const uint queueId = r->getRenderQueue();

    RenderQueue& queue = _queues.at( queueId );

    // Get the right RenderableMap for this material.
    MaterialPtr mat = r->getMaterial();
    RenderQueue::RenderableMap& rm = queue.solids[mat];

    // Create a RenderableInstance for this renderable.
    RenderQueue::RenderableInstance ri;
    ri.model = node->getFullTransform();
    ri.depth = node->getDepth();
    ri.colorModifier = node->getColorModifier();

    RenderQueue::RIList& riList = rm[r];
    riList.push_back( ri );

    // Add this queue to the active queue list if not already there.
    activateQueue( queueId, _queues[queueId] );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::present( const Lore::RenderView& rv, const Lore::WindowPtr window )
{
    // Traverse the scene graph and update object transforms.
    Lore::SceneGraphVisitor sgv( rv.scene->getRootNode() );
    sgv.visit( *this );

    // TODO: Cache which scenes have been visited and check before doing it again.
    // [OR] move visitor to context?
    // ...

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    glViewport( rv.gl_viewport.x,
                rv.gl_viewport.y,
                rv.gl_viewport.width,
                rv.gl_viewport.height );

    Color bg = rv.scene->getBackgroundColor();
    glClearColor( bg.r, bg.g, bg.b, 0.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Setup view-projection matrix.
    // TODO: Take viewport dimensions into account. Cache more things inside window.
    const float aspectRatio = window->getAspectRatio();
    const Matrix4 projection = Math::OrthoRH( -aspectRatio, aspectRatio,
                                              -1.f, 1.f,
                                              -100.f, 100.f );

    const Matrix4 viewProjection = rv.camera->getViewMatrix() * projection;
    
    // Iterate through all active render queues and render each object.
    for ( const auto& activeQueue : _activeQueues ) {
        RenderQueue& queue = activeQueue.second;

        // Render solids.
        renderMaterialMap( rv.scene, queue.solids, viewProjection );
    }

    _clearRenderQueues();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::_clearRenderQueues()
{
    // Remove all data from each queue.
    for ( auto& queue : _queues ) {
        queue.solids.clear();
    }

    // Erase all queues from the active queue list.
    _activeQueues.clear();
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

void GenericRenderer::renderMaterialMap( const Lore::ScenePtr scene,
                                         Lore::RenderQueue::MaterialMap& mm,
                                         const Lore::Matrix4& viewProjection ) const
{
    for ( auto& pair : mm ) {
        MaterialPtr material = pair.first;
        const RenderQueue::RenderableMap& rm = pair.second;

        // Setup material settings.
        // TODO: Multi-pass rendering.
        Lore::Material::Pass& pass = material->getPass( 0 );
        Lore::GPUProgramPtr program = pass.program;
        VertexBufferPtr vb = program->getVertexBuffer();
        program->use();

        // Setup per-material uniform values.
        if ( pass.lighting ) {
            //
            // Set material lighting data.

            program->setUniformVar( "material.ambient", pass.ambient );
            program->setUniformVar( "material.diffuse", pass.diffuse );

            //
            // Set scene values.

            program->setUniformVar( "sceneAmbient", scene->getAmbientLightColor() );
            program->setUniformVar( "numLights", scene->getLightCount() );

            auto lights = scene->getLightIterator();
            std::vector<LightPtr> lightArray;
            while ( lights.hasMore() ) {
                auto light = lights.getNext();
                lightArray.push_back( light );
            }

            program->updateLights( lightArray );

        }

        vb->bind();
        
        // For each Renderable, get the map of ZOrder --> RenderableInstanceList.
        for ( auto& riListMap : rm ) {
            RenderablePtr renderable = riListMap.first;

            // Bind this renderable for rendering all of its instances.
            renderable->bind();

            for ( const RenderQueue::RenderableInstance& ri : riListMap.second ) {

                // Apply depth to z-value of the model matrix (overwrite SceneGraphVisitor transformations).
                // This is necessary for depth values on nodes to work correctly.
                Matrix4 model = ri.model;
                model[3][2] = static_cast< real >( ri.depth );

                // Calculate model-view-projection matrix for this object.
                Matrix4 mvp = viewProjection * model;

                // Update the MVP value in the shader.
                program->setTransformVar( mvp );

                if ( pass.lighting ) {
                    program->setUniformVar( "model", ri.model );
                }

                if ( pass.colorMod ) {
                    program->setUniformVar( "colorMod", ri.colorModifier );
                }

                // Draw the renderable.
                vb->draw();
            }
        }

        vb->unbind();
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
