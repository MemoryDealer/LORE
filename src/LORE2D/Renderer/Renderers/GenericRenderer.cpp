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
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Renderer/IRenderAPI.h>
#include <LORE2D/Resource/Mesh.h>
#include <LORE2D/Resource/Renderable/Texture.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Scene/Light.h>
#include <LORE2D/Scene/Scene.h>
#include <LORE2D/Shader/GPUProgram.h>
#include <LORE2D/Window/Window.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::GenericRenderer()
{
  // Initialize all available queues.
  _queues.resize( DefaultRenderQueueCount );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GenericRenderer::~GenericRenderer()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::addRenderData( Lore::EntityPtr e,
                                     Lore::NodePtr node )
{
  const uint queueId = e->getRenderQueue();
  const bool blended = e->getMaterial()->blendingMode.enabled;

  // Add this queue to the active queue list if not already there.
  activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this entity at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  if ( blended ) {
    RenderQueue::Transparent t;
    t.material = e->getMaterial();
    t.vertexBuffer = e->getMesh()->getVertexBuffer();
    t.model = node->getFullTransform();

    const auto depth = node->getDepth();
    t.model[3][2] = depth;

    queue.transparents.insert( { depth, t } );
  }
  else {
    // Acquire the render data list for this material/vb (or create one).
    RenderQueue::EntityData entityData;
    entityData.material = e->getMaterial();
    entityData.vertexBuffer = e->getMesh()->getVertexBuffer();

    RenderQueue::RenderDataList& renderData = queue.solids[entityData];

    // Fill out the render data and add it to the list.
    RenderQueue::RenderData rd;
    rd.model = node->getFullTransform();
    rd.model[3][2] = node->getDepth();

    renderData.push_back( rd );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::present( const Lore::RenderView& rv, const Lore::WindowPtr window )
{
  const real aspectRatio = window->getAspectRatio();
  rv.camera->updateTracking( aspectRatio );

  _api->setDepthTestEnabled( true );
  _api->setViewport( rv.gl_viewport.x,
                     rv.gl_viewport.y,
                     rv.gl_viewport.width,
                     rv.gl_viewport.height );

  Color bg = rv.scene->getBackgroundColor();
  _api->clear();
  _api->clearColor( bg.r, bg.g, bg.b, 0.f );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
  const Matrix4 projection = Math::OrthoRH( -aspectRatio, aspectRatio,
                                            -1.f, 1.f,
                                            1000.f, -1000.f );

  const Matrix4 viewProjection = rv.camera->getViewMatrix() * projection;

  // Render background before scene node entities.
  renderBackground( rv, aspectRatio, projection );

  // Iterate through all active render queues and render each object.
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;

    // Render solids.
    renderMaterialMap( rv.scene, queue.solids, viewProjection );

    // Render transparents.
    renderTransparents( rv.scene, queue.transparents, viewProjection );
  }

  _clearRenderQueues();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::_clearRenderQueues()
{
  // Remove all data from each queue.
  for ( auto& queue : _queues ) {
    queue.solids.clear();
    queue.transparents.clear();
  }

  // Erase all queues from the active queue list.
  _activeQueues.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::activateQueue( const uint id, Lore::RenderQueue& rq )
{
  const auto lookup = _activeQueues.find( id );
  if ( _activeQueues.end() == lookup ) {
    _activeQueues.insert( { id, rq } );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderBackground( const Lore::RenderView& rv,
                                        const real aspectRatio,
                                        const Lore::Matrix4& proj )
{
  BackgroundPtr background = rv.scene->getBackground();
  Background::LayerMap layers = background->getLayerMap();

  VertexBufferPtr vb = StockResource::GetVertexBuffer( "Background" );
  vb->bind();

  const Vec2 camPos = rv.camera->getPosition();

  for ( const auto& pair : layers ) {
    const Background::Layer& layer = pair.second;
    MaterialPtr mat = layer.getMaterial();

    GPUProgramPtr program = mat->program;
    TexturePtr texture = mat->texture;

    // Enable blending if set.
    if ( mat->blendingMode.enabled ) {
      _api->setBlendingEnabled( true );
      _api->setBlendingFunc( mat->blendingMode.srcFactor, mat->blendingMode.dstFactor );
    }

    if ( texture ) {
      program->use();
      texture->bind();

      Lore::Rect sampleRegion = mat->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );

      program->setUniformVar( "material.diffuse", mat->diffuse );

      // Apply scrolling and parallax offsets.
      Lore::Vec2 offset = mat->getTexCoordOffset();
      offset.x += camPos.x * layer.getParallax().x;
      offset.y -= camPos.y * layer.getParallax().y;
      program->setUniformVar( "texSampleOffset", offset );

      Lore::Matrix4 transform = Math::CreateTransformationMatrix( Lore::Vec2( 0.f, 0.f ), Lore::Quaternion() );
      transform[0][0] = aspectRatio;
      transform[1][1] = Math::Clamp( aspectRatio, 1.f, 90.f ); // HACK to prevent clipping background on aspect ratios < 1.0.
      transform[3][2] = layer.getDepth();
      program->setTransformVar( proj * transform );

      vb->draw();
    }

    if ( mat->blendingMode.enabled ) {
      _api->setBlendingEnabled( false );
    }
  }

  vb->unbind();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderMaterialMap( const Lore::ScenePtr scene,
                                         Lore::RenderQueue::EntityDataMap& mm,
                                         const Lore::Matrix4& viewProjection ) const
{
  // Iterate over render data lists for each material.

  for ( auto& pair : mm ) {

    const RenderQueue::EntityData& entityData = pair.first;
    const RenderQueue::RenderDataList& renderDataList = pair.second;

    //
    // Bind material settings to GPU.

    MaterialPtr mat = entityData.material;
    GPUProgramPtr program = mat->program;
    VertexBufferPtr vertexBuffer = entityData.vertexBuffer;
    TexturePtr texture = mat->texture;

    program->use();
    if ( texture ) {
      // TODO: Multi-texturing.
      texture->bind();
      program->setUniformVar( "texSampleOffset", mat->getTexCoordOffset() );

      Lore::Rect sampleRegion = mat->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
    }

    // Upload lighting data.
    if ( mat->lighting ) {

      // Material.
      program->setUniformVar( "material.ambient", mat->ambient );
      program->setUniformVar( "material.diffuse", mat->diffuse );

      // Scene.
      program->setUniformVar( "sceneAmbient", scene->getAmbientLightColor() );
      program->setUniformVar( "numLights", scene->getLightCount() );

      program->updateLights( scene->getLightConstIterator() );

    }

    vertexBuffer->bind();

    // Render each node associated with this entity data.
    for ( const RenderQueue::RenderData& rd : renderDataList ) {

      // Calculate model-view-projection matrix for this object.
      Matrix4 mvp = viewProjection * rd.model;

      program->setTransformVar( mvp );

      if ( mat->lighting ) {
        program->setUniformVar( "model", rd.model );
      }

      // Draw the entity.
      vertexBuffer->draw();

    }

    vertexBuffer->unbind();

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderTransparents( const Lore::ScenePtr scene,
                                          Lore::RenderQueue::TransparentDataMap& tm,
                                          const Lore::Matrix4& viewProjection ) const
{
  _api->setBlendingEnabled( true );

  // Render in forward order, so the farthest back is rendered first.
  // (Depth values increase going farther back).
  for ( auto it = tm.begin(); it != tm.end(); ++it ) {

    RenderQueue::Transparent& t = it->second;
    MaterialPtr mat = t.material;
    GPUProgramPtr program = mat->program;
    VertexBufferPtr vertexBuffer = t.vertexBuffer;
    TexturePtr texture = mat->texture;

    // Set blending mode using material settings.
    _api->setBlendingFunc( mat->blendingMode.srcFactor, mat->blendingMode.dstFactor );

    program->use();
    if ( texture ) {
      // TODO: Multi-texturing.
      texture->bind();
      program->setUniformVar( "texSampleOffset", mat->getTexCoordOffset() );

      Lore::Rect sampleRegion = mat->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
    }

    // Upload lighting data.
    if ( mat->lighting ) {

      // Material.
      program->setUniformVar( "material.ambient", mat->ambient );
      program->setUniformVar( "material.diffuse", mat->diffuse );

      // Scene.
      program->setUniformVar( "sceneAmbient", scene->getAmbientLightColor() );
      program->setUniformVar( "numLights", scene->getLightCount() );

      program->updateLights( scene->getLightConstIterator() );

    }

    vertexBuffer->bind();

    {
      // Calculate model-view-projection matrix for this object.
      Matrix4 mvp = viewProjection * t.model;

      program->setTransformVar( mvp );

      if ( mat->lighting ) {
        program->setUniformVar( "model", t.model );
      }

      // Draw the entity.
      vertexBuffer->draw();
    }

    vertexBuffer->unbind();

  }

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //