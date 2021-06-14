// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "Forward2DRenderer.h"

#include <LORE/Config/Config.h>
#include <LORE/Math/Math.h>
#include <LORE/Resource/Box.h>
#include <LORE/Resource/Prefab.h>
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

Forward2DRenderer::Forward2DRenderer()
{
  // Initialize all available queues.
  _queues.resize( DefaultRenderQueueCount );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::addRenderData( PrefabPtr prefab,
                                     NodePtr node )
{
  // TODO: Consider a way of optimizing this by skipping processing data here
  // and directly storing references to nodes in lists in the renderer.
  const uint queueId = prefab->getRenderQueue();
  const bool blended = prefab->getMaterial()->blendingMode.enabled;

  // TODO: Fix two lookups here.
  // Add this queue to the active queue list if not already there.
  activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this prefab at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  if ( blended ) {
    RenderQueue::PrefabNodePair pair { prefab, node };
    queue.transparents.insert( { node->getDepth(), pair } );
  }
  else {
    if ( prefab->isInstanced() ) {
      RenderQueue::InstancedPrefabSet& set = queue.instancedSolids;
      // Only add instanced prefabs that haven't been processed yet.
      if ( set.find( prefab ) == set.end() ) {
        set.insert( prefab );
      }
    }
    else {
      RenderQueue::NodeList& nodes = queue.solids[prefab];
      nodes.push_back( node );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::addBox( BoxPtr box,
                              const glm::mat4& transform )
{
  const uint queueId = RenderQueue::General;

  activateQueue( queueId, _queues[queueId] );

  RenderQueue& queue = _queues.at( queueId );
  RenderQueue::BoxData data;
  data.box = box;
  data.model = transform;

  queue.boxes.push_back( data );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::addTextbox( TextboxPtr textbox,
                                  const glm::mat4& transform )
{
  const uint queueId = textbox->getRenderQueue();

  activateQueue( queueId, _queues[queueId] );
  RenderQueue& queue = _queues.at( queueId );

  RenderQueue::TextboxData data;
  data.textbox = textbox;
  data.model = transform;

  queue.textboxes.push_back( data );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::addLight( LightPtr light, const NodePtr node )
{
  const uint queueId = RenderQueue::General;

  activateQueue( queueId, _queues[queueId] );
  RenderQueue& queue = _queues.at( queueId );

  switch ( light->getType() ) {
  default:
    break;

  case Light::Type::Directional:
    queue.lights.directionalLights.push_back( static_cast< DirectionalLightPtr >( light ) );
    break;

  case Light::Type::Point:
    queue.lights.pointLights.emplace_back( static_cast< PointLightPtr >( light ), node->getWorldPosition() );
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::present( const RenderView& rv, const WindowPtr window )
{
  // Build render queues for this RenderView.
  rv.scene->updateSceneGraph();

  const real aspectRatio = (rv.renderTarget) ? rv.renderTarget->getAspectRatio() : window->getAspectRatio();
  rv.camera->updateTracking();

  if ( rv.renderTarget ) {
    rv.renderTarget->bind();
    _api->setViewport( 0,
                       0,
                       static_cast<uint32_t>( rv.viewport.w * rv.renderTarget->getWidth() ),
                       static_cast<uint32_t>( rv.viewport.h * rv.renderTarget->getHeight() ) );
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
  _api->clearColor( bg.r, bg.g, bg.b, 1.f );
  _api->setPolygonMode( IRenderAPI::PolygonMode::Fill );
  _api->setCullingMode( IRenderAPI::CullingMode::Back );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
  const glm::mat4 projection = glm::ortho( -aspectRatio, aspectRatio,
                                           -1.f, 1.f,
                                           1500.f, -1500.f );

  const glm::mat4 viewProjection = projection * rv.camera->getViewMatrix();

  // Render skybox before scene node prefabs.
  renderSkybox( rv, aspectRatio, projection );

  // Iterate through all active render queues and render each object.
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;

    // Render solids.
    renderSolids( rv, queue, viewProjection );

    // Render transparents.
    renderTransparents( rv, queue, viewProjection );

    // Render boxes.
    renderBoxes( queue, viewProjection );
  }

  // AABBs.
  /*auto renderAABBs = Config::GetValue( "RenderAABBs" );
  if ( GET_VARIANT<bool>( renderAABBs ) ) {
    RenderQueue tmpQueue;

    std::function<void( NodePtr )> AddAABB = [&] ( NodePtr node ) {
      AABBPtr aabb = node->getAABB();
      if ( aabb ) {
        RenderQueue::BoxData data;
        data.box = aabb->getBox();
        data.model = Math::CreateTransformationMatrix( node->getWorldPosition(), glm::quat(), aabb->getDimensions() * 5.f );
        data.model[3][2] = Node::Depth::Min;

        tmpQueue.boxes.push_back( data );
      }

      Node::ChildNodeIterator it = node->getChildNodeIterator();
      while ( it.hasMore() ) {
        AddAABB( it.getNext() );
      }
    };

    auto root = rv.scene->getRootNode();
    Node::ChildNodeIterator it = root->getChildNodeIterator();
    while ( it.hasMore() ) {
      AddAABB( it.getNext() );
    }
    renderBoxes( tmpQueue, viewProjection );
  }*/

  if ( rv.renderTarget ) {
    rv.renderTarget->flush();
    // Re-bind default frame buffer if custom render target was specified.
    _api->bindDefaultFramebuffer();
  }

  _clearRenderQueues();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::_clearRenderQueues()
{
  // Remove all data from each queue.
  for ( auto& queue : _queues ) {
    queue.clear();
  }

  // Erase all queues from the active queue list.
  _activeQueues.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::activateQueue( const uint id, RenderQueue& rq )
{
  const auto lookup = _activeQueues.find( id );
  if ( _activeQueues.end() == lookup ) {
    _activeQueues.insert( { id, rq } );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::renderSkybox( const RenderView& rv,
                                        const real aspectRatio,
                                        const glm::mat4& proj )
{
  SkyboxPtr skybox = rv.scene->getSkybox();
  const SkyboxLayerMap& layers = skybox->getLayerMap();

  ModelPtr model = StockResource::GetModel( "Skybox2D" );

  const glm::vec3 camPos = rv.camera->getPosition();

  for ( const auto& pair : layers ) {
    const SkyboxLayer& layer = pair.second;
    MaterialPtr mat = layer.getMaterial();

    // TODO: Move this to uniform updaters in Skybox2D program.
    if ( mat->sprite && mat->sprite->getTextureCount( 0, Texture::Type::Diffuse ) ) {
      GPUProgramPtr program = mat->program;

      // Enable blending if set.
      if ( mat->blendingMode.enabled ) {
        _api->setBlendingEnabled( true );
        _api->setBlendingFunc( mat->blendingMode.srcFactor, mat->blendingMode.dstFactor );
      }

      // Get the active sprite frame for this layer.
      size_t spriteFrame = 0;
      if ( layer.getSpriteController() ) {
        spriteFrame = layer.getSpriteController()->getActiveFrame();
      }

      TexturePtr texture = mat->sprite->getTexture( spriteFrame, Texture::Type::Diffuse );
      program->use();
      texture->bind();

      program->setUniformVar( "gamma", rv.gamma );

      Rect sampleRegion = mat->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );

      program->setUniformVar( "material.diffuse", mat->diffuse );

      // Apply scrolling and parallax offsets.
      glm::vec2 offset = mat->getTexCoordOffset();
      offset.x += camPos.x * layer.getParallax().x;
      offset.y += camPos.y * layer.getParallax().y;
      program->setUniformVar( "texSampleOffset", offset );

      glm::mat4 transform( 1.f );
      transform[0][0] = aspectRatio;
      transform[1][1] = glm::clamp( aspectRatio, 1.f, 90.f ); // HACK to prevent clipping skybox on aspect ratios < 1.0.
      transform[3][2] = layer.getDepth();
      program->setTransformVar( proj * transform );

      model->draw( program );
    }

    if ( mat->blendingMode.enabled ) {
      _api->setBlendingEnabled( false );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::renderSolids( const RenderView& rv,
                                      const RenderQueue& queue,
                                      const glm::mat4& viewProjection ) const
{
  // Render instanced solids.
  for ( const auto& prefab : queue.instancedSolids ) {
    MaterialPtr material = prefab->getMaterial();
    ModelPtr model = prefab->getInstancedModel();
    GPUProgramPtr program = material->program;

    const NodePtr node = prefab->getInstanceControllerNode();

    _api->setCullingMode( prefab->cullingMode );

    program->use();
    program->updateUniforms( rv, material, queue.lights );
    program->updateNodeUniforms( material, node, viewProjection );

    model->draw( program, prefab->getInstanceCount() );
  }

  // Render non-instanced solids.
  for ( auto& pair : queue.solids ) {
    const PrefabPtr prefab = pair.first;
    const RenderQueue::NodeList& nodes = pair.second;

    const MaterialPtr material = prefab->getMaterial();
    const ModelPtr model = prefab->getModel();
    const GPUProgramPtr program = material->program;

    _api->setCullingMode( prefab->cullingMode );

    program->use();
    program->updateUniforms( rv, material, queue.lights );

    // Render each node associated with this prefab.
    for ( const auto& node : nodes ) {
      program->updateNodeUniforms( material, node, viewProjection );
      model->draw( program );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::renderTransparents( const RenderView& rv,
                                          const RenderQueue& queue,
                                          const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );

  // Render in forward order, so the farthest back is rendered first.
  // (Depth values increase going farther back).
  for (const auto& pair : queue.transparents) {
    const PrefabPtr prefab = pair.second.first;
    NodePtr node = pair.second.second;

    const MaterialPtr material = prefab->getMaterial();
    GPUProgramPtr program = material->program;
    ModelPtr model = prefab->getModel();

    if ( prefab->isInstanced() ) {
      node = prefab->getInstanceControllerNode();
      model = prefab->getInstancedModel();
      switch ( model->getType() ) {
      default:
        throw Lore::Exception( "Instanced prefab must have an instanced model" );

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
    _api->setCullingMode( prefab->cullingMode );

    program->use();

    program->updateUniforms( rv, material, queue.lights );
    program->updateNodeUniforms( material, node, viewProjection );

    // Draw the prefab.
    model->draw( program, prefab->getInstanceCount() );
  }

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward2DRenderer::renderBoxes( const RenderQueue& queue,
                                     const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );
  _api->setBlendingFunc( BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha );

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
