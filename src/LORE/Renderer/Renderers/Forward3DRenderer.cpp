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

#include <LORE/Config/Config.h>
#include <LORE/Math/Math.h>
#include <LORE/Resource/Box.h>
#include <LORE/Resource/Entity.h>
#include <LORE/Renderer/IRenderAPI.h>
#include <LORE/Resource/Font.h>
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
    queue.transparents.insert( { glm::length( _camera->getPosition() - node->getPosition() ), pair } );
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
  _camera = rv.camera;

  // Build render queues for this RenderView.
  rv.scene->updateSceneGraph();

  // Add directional lights.
  auto dirLightIt = rv.scene->getDirectionalLights().getConstIterator();
  while ( dirLightIt.hasMore() ) {
    auto dirLight = dirLightIt.getNext();
    addLight( dirLight, nullptr );
  }

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

  Color bg = rv.scene->getSkyboxColor();
  _api->clear();
  _api->clearColor( bg.r, bg.g, bg.b, 1.f );
  _api->setPolygonMode( IRenderAPI::PolygonMode::Fill );
  _api->setDepthTestEnabled( true );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
  const real aspectRatio = ( rv.renderTarget ) ? rv.renderTarget->getAspectRatio() : window->getAspectRatio();
  const glm::mat4 projection = glm::perspective( glm::radians( 45.f ),
                                                 aspectRatio,
                                                 0.1f, 20000.f );

  const glm::mat4 viewProjection = projection * rv.camera->getViewMatrix();

  // Render all solids first.
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;
    _renderSolids( rv, queue, viewProjection );
  }

  // Render skybox.
  _api->setDepthFunc( IRenderAPI::DepthFunc::LessEqual );
  _renderSkybox( rv, viewProjection );
  _api->setDepthFunc( IRenderAPI::DepthFunc::Less );

  // Render any blended objects last.
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;
    _renderTransparents( rv, queue, viewProjection );
  }

  // Test - render AABBs.
  _renderingBoundingBoxes( rv, viewProjection );

  if ( rv.renderTarget ) {
    rv.renderTarget->flush();
    _api->bindDefaultFramebuffer();
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

  _aabbs.clear();
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

void Forward3DRenderer::_renderSkybox( const RenderView& rv,
                                       const glm::mat4& viewProjection ) const
{
  SkyboxPtr skybox = rv.scene->getSkybox();
  ModelPtr model = StockResource::GetModel( "Skybox3D" );

  _api->setDepthMaskEnabled( false );

  const Skybox::LayerMap& layers = skybox->getLayerMap();
  for ( const auto& pair : layers ) {
    const Skybox::Layer& layer = pair.second;
    MaterialPtr material = layer.getMaterial();

    RenderQueue::LightData emptyLightData; // Not needed for skybox.

    // Enable blending if set.
    if ( material->blendingMode.enabled ) {
      _api->setBlendingEnabled( true );
      _api->setBlendingFunc( material->blendingMode.srcFactor, material->blendingMode.dstFactor );
    }

    // Setup GPUProgram.
    GPUProgramPtr program = material->program;
    program->use();
    program->updateUniforms( rv, material, emptyLightData );
    // TODO: Pass in camera node when camera is updated to use a scene node.
    program->updateNodeUniforms( material, nullptr, viewProjection );
    model->draw( program );
  }

  _api->setDepthMaskEnabled( true );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderSolids( const RenderView& rv,
                                       const RenderQueue& queue,
                                       const glm::mat4& viewProjection ) const
{
  const ScenePtr scene = rv.scene;

  // Render instanced solids.
  for ( const auto& entity : queue.instancedSolids ) {
    MaterialPtr material = entity->getMaterial();
    ModelPtr model = entity->getInstancedModel();
    GPUProgramPtr program = nullptr;

    const NodePtr node = entity->getInstanceControllerNode();

    // Acquire the correct GPUProgram for this instanced model type.
    switch ( model->getType() ) {
    default:
      throw Lore::Exception( "Instanced entity must have an instanced model" );

    case Mesh::Type::Quad3DInstanced:
    case Mesh::Type::CubeInstanced:
      program = StockResource::GetGPUProgram( "StandardInstanced3D" );
      break;

    case Mesh::Type::TexturedQuad3DInstanced:
    case Mesh::Type::TexturedCubeInstanced:
      program = StockResource::GetGPUProgram( "StandardTexturedInstanced3D" );
      break;
    }

    program->use();

    program->updateUniforms( rv, material, queue.lights );
    program->updateNodeUniforms( material, node, viewProjection );

    model->draw( program, entity->getInstanceCount() );
  }

  // Render non-instanced solids.
  for ( auto& pair : queue.solids ) {
    const EntityPtr entity = pair.first;
    const RenderQueue::NodeList& nodes = pair.second;

    const MaterialPtr material = entity->getMaterial();
    const ModelPtr model = entity->getModel();
    const GPUProgramPtr program = material->program;

    program->use();
    program->updateUniforms( rv, material, queue.lights );

    // Render each node associated with this entity.
    for ( const auto& node : nodes ) {
      program->updateNodeUniforms( material, node, viewProjection );
      model->draw( program );

      //
      _aabbs.push_back( node );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderTransparents( const RenderView& rv,
                                             const RenderQueue& queue,
                                             const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );

  // Render in reverse order, so the farthest back is rendered first.
  for ( auto it = queue.transparents.rbegin(); it != queue.transparents.rend(); ++it ) {
    const EntityPtr entity = it->second.first;
    NodePtr node = it->second.second;

    const MaterialPtr material = entity->getMaterial();
    GPUProgramPtr program = material->program;
    ModelPtr model = entity->getModel();

    if ( entity->isInstanced() ) {
      node = entity->getInstanceControllerNode();
      model = entity->getInstancedModel();
      switch ( model->getType() ) {
      default:
        throw Lore::Exception( "Instanced entity must have an instanced model" );

      case Mesh::Type::QuadInstanced:
        program = StockResource::GetGPUProgram( "StandardInstanced2D" );
        break;

      case Mesh::Type::TexturedQuadInstanced:
        program = StockResource::GetGPUProgram( "StandardTexturedInstanced2D" );
        break;
      }
    }

    // Set blending mode using material settings.
    _api->setBlendingFunc( material->blendingMode.srcFactor, material->blendingMode.dstFactor );

    program->use();
    program->updateUniforms( rv, material, queue.lights );
    program->updateNodeUniforms( material, node, viewProjection );

    // Draw the entity.
    model->draw( program, entity->getInstanceCount() );
  }

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderBoxes( const RenderQueue& queue,
                                      const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );
  _api->setBlendingFunc( Material::BlendFactor::SrcAlpha, Material::BlendFactor::OneMinusSrcAlpha );

  GPUProgramPtr program = StockResource::GetGPUProgram( "StandardBox2D" );
  ModelPtr model = StockResource::GetModel( "TexturedQuad" );

  program->use();

  for ( const RenderQueue::BoxData& data : queue.boxes ) {
    BoxPtr box = data.box;
    program->setUniformVar( "borderColor", box->getBorderColor() );
    program->setUniformVar( "fillColor", box->getFillColor() );
    program->setUniformVar( "borderWidth", box->getBorderWidth() );
    program->setUniformVar( "scale", glm::vec2( data.model[0][0], data.model[1][1] ) * box->getSize() );

    // Apply box scaling to final transform.
    glm::mat4 modelMatrix = data.model;
    modelMatrix[0][0] *= box->getWidth();
    modelMatrix[1][1] *= box->getHeight();
    program->setTransformVar( viewProjection * modelMatrix );

    model->draw( program );
  }

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderingBoundingBoxes( const RenderView& rv,
                                                 const glm::mat4& viewProjection ) const
{
  _api->setPolygonMode( IRenderAPI::PolygonMode::Line );

  GPUProgramPtr program = StockResource::GetGPUProgram( "UnlitStandard3D" );
  ModelPtr model = StockResource::GetModel( "BoundingBox" );

  for ( const auto& node : _aabbs ) {
    MaterialPtr material = nullptr;
    auto entityIt = node->getEntityListConstIterator();
    while ( entityIt.hasMore() ) {
      auto entity = entityIt.getNext();
      material = entity->getMaterial();
      if ( material ) {
        break;
      }
    }

    if ( !material ) {
      continue;
    }

    program->use();
    program->updateUniforms( rv, material, {} );
    // TODO: Create a child node of every node that is its AABB?
    // Pass it into here and update it accordingly.
    program->updateNodeUniforms( material, node, viewProjection );

    model->draw( program );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
