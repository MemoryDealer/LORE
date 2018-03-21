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

#include "Forward3DRenderer.h"

#include <LORE/Core/DebugUI/DebugUI.h>
#include <LORE/Config/Config.h>
#include <LORE/Math/Math.h>
#include <LORE/Resource/Box.h>
#include <LORE/Resource/Entity.h>
#include <LORE/Renderer/IRenderAPI.h>
#include <LORE/Resource/Font.h>
#include <LORE/Resource/Mesh.h>
#include <LORE/Resource/Sprite.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Resource/Textbox.h>
#include <LORE/Resource/Texture.h>
#include <LORE/Scene/AABB.h>
#include <LORE/Scene/Camera.h>
#include <LORE/Scene/Light.h>
#include <LORE/Scene/Scene.h>
#include <LORE/Scene/SpriteController.h>
#include <LORE/Shader/GPUProgram.h>
#include <LORE/UI/UI.h>
#include <LORE/Window/RenderTarget.h>
#include <LORE/Window/Window.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Forward3DRenderer::Forward3DRenderer()
{
  // Initialize all available queues.
  _queues.resize( DefaultRenderQueueCount );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::addRenderData( EntityPtr entity,
                                       NodePtr node )
{
  // TODO: Consider a way of optimizing this by skipping processing data here
  // and directly storing references to nodes in lists in the renderer.
  const uint queueId = entity->getRenderQueue();
  const bool blended = entity->getMaterial()->blendingMode.enabled;

  // TODO: Fix two lookups here.
  // Add this queue to the active queue list if not already there.
  _activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this entity at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  if ( blended ) {
    RenderQueue::EntityNodePair pair { entity, node };
    queue.transparents.insert( { node->getDepth(), pair } );
  }
  else {
    if ( entity->isInstanced() ) {
      RenderQueue::InstancedEntitySet& set = queue.instancedSolids;
      // Only add instanced entities that haven't been processed yet.
      if ( set.find( entity ) == set.end() ) {
        set.insert( entity );
      }
    }
    else {
      RenderQueue::NodeList& nodes = queue.solids[entity];
      nodes.push_back( node );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::addBox( BoxPtr box,
                                const glm::mat4& transform )
{
  const uint queueId = RenderQueue::General;

  _activateQueue( queueId, _queues[queueId] );

  RenderQueue& queue = _queues.at( queueId );
  RenderQueue::BoxData data;
  data.box = box;
  data.model = transform;

  queue.boxes.push_back( data );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::addTextbox( TextboxPtr textbox,
                                    const glm::mat4& transform )
{
  const uint queueId = textbox->getRenderQueue();

  _activateQueue( queueId, _queues[queueId] );
  RenderQueue& queue = _queues.at( queueId );

  RenderQueue::TextboxData data;
  data.textbox = textbox;
  data.model = transform;

  queue.textboxes.push_back( data );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::addLight( LightPtr light,
                                  const NodePtr node )
{
  const uint queueId = RenderQueue::General;

  _activateQueue( queueId, _queues[queueId] );
  RenderQueue& queue = _queues.at( queueId );

  switch ( light->getType() ) {
  default:
    break;

  case Light::Type::Directional:
    queue.lights.directionalLights.push_back( static_cast< DirectionalLightPtr >( light ) );
    break;

  case Light::Type::Point:
    queue.lights.pointLights.emplace_back( static_cast< PointLightPtr >( light ), node->getDerivedPosition() );
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::present( const RenderView& rv,
                                 const WindowPtr window )
{
  // Build render queues for this RenderView.
  rv.scene->updateSceneGraph();

  // Add directional lights.
  auto dirLightIt = rv.scene->getDirectionalLights().getConstIterator();
  while ( dirLightIt.hasMore() ) {
    auto dirLight = dirLightIt.getNext();
    addLight( dirLight, nullptr );
  }

  const real aspectRatio = ( rv.renderTarget ) ? rv.renderTarget->getAspectRatio() : window->getAspectRatio();

  if ( rv.renderTarget ) {
    rv.renderTarget->bind();
    _api->setViewport( 0,
                       0,
                       static_cast< uint32_t >( rv.viewport.w * rv.renderTarget->getWidth() ),
                       static_cast< uint32_t >( rv.viewport.h * rv.renderTarget->getHeight() ) );
  }
  else {
    // TODO: Get rid of gl_viewport.
    _api->setViewport( rv.gl_viewport.x,
                       rv.gl_viewport.y,
                       rv.gl_viewport.width,
                       rv.gl_viewport.height );
  }

  _api->setDepthTestEnabled( true );

  Color bg = rv.scene->getSkyboxColor();
  _api->clear();
  _api->clearColor( bg.r, bg.g, bg.b, 0.f );
  _api->setPolygonMode( IRenderAPI::PolygonMode::Fill );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
  const glm::mat4 projection = glm::perspective( glm::radians( 45.f ),
                                                 aspectRatio,
                                                 0.1f, 1000.f );

  const glm::mat4 viewProjection = projection * rv.camera->getViewMatrix();

  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;

    // Render solids.
    _renderSolids( rv, queue, viewProjection );
  }

  _clearRenderQueues();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_clearRenderQueues()
{
  // Remove all data from each queue.
  for ( auto& queue : _queues ) {
    queue.clear();
  }

  // Erase all queues from the active queue list.
  _activeQueues.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_activateQueue( const uint id,
                                        RenderQueue& rq )
{
  const auto lookup = _activeQueues.find( id );
  if ( _activeQueues.end() == lookup ) {
    _activeQueues.insert( { id, rq } );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderSolids( const RenderView& rv,
                                       const RenderQueue& queue,
                                       const glm::mat4& viewProjection ) const
{
  const ScenePtr scene = rv.scene;

  // Render non-instanced solids.
  for ( auto& pair : queue.solids ) {
    const EntityPtr entity = pair.first;
    const RenderQueue::NodeList& nodes = pair.second;

    const MaterialPtr material = entity->getMaterial();
    const VertexBufferPtr vertexBuffer = entity->getMesh()->getVertexBuffer();
    const GPUProgramPtr program = material->program;

    program->use();
    vertexBuffer->bind();

    program->setUniformVar( "viewPos", rv.camera->getPosition() );

    // Lighting data will be the same for all nodes.
    if ( material->lighting ) {
      _updateLighting( material, program, scene, queue.lights );
    }

    // Render each node associated with this entity.
    for ( const auto& node : nodes ) {
      if ( material->sprite && material->sprite->getTextureCount() ) {
        _updateTextureData( material, program, node );
      }

      // Get the model matrix from the node.
      glm::mat4 model = node->getFullTransform();

      // Calculate model-view-projection matrix for this object.
      const glm::mat4 mvp = viewProjection * model;
      program->setTransformVar( mvp );

      if ( material->lighting ) {
        program->setUniformVar( "model", model );
      }

      vertexBuffer->draw();
    }

    // Rendering this entity is complete.
    vertexBuffer->unbind();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_updateTextureData( const MaterialPtr material,
                                            const GPUProgramPtr program,
                                            const NodePtr node ) const
{
  size_t spriteFrame = 0;
  const auto spc = node->getSpriteController();
  if ( spc ) {
    spriteFrame = spc->getActiveFrame();
  }

  const TexturePtr texture = material->sprite->getTexture( spriteFrame );
  texture->bind( 0 ); // TODO: Multi-texturing.
  texture->bind( 1 );
  program->setUniformVar( "texSampleOffset", material->getTexCoordOffset() );

  const Rect sampleRegion = material->getTexSampleRegion();
  program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
  program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
  program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
  program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_updateLighting( const MaterialPtr material,
                                         const GPUProgramPtr program,
                                         const ScenePtr scene,
                                         const RenderQueue::LightData& lights ) const
{
  // Update material uniforms.
  program->setUniformVar( "material.ambient", material->ambient );
  program->setUniformVar( "material.diffuse", material->diffuse );
  program->setUniformVar( "material.specular", material->specular );
  program->setUniformVar( "material.shininess", material->shininess );
  program->setUniformVar( "sceneAmbient", scene->getAmbientLightColor() );

  // Update uniforms for light data.
  program->setUniformVar( "numDirLights", static_cast<int>( lights.directionalLights.size() ) );
  program->setUniformVar( "numPointLights", static_cast<int>( lights.pointLights.size() ) );

  program->updateLights( lights );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
