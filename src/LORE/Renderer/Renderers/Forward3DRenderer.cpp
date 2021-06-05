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

Forward3DRenderer::Forward3DRenderer()
{
  // Initialize all available queues.
  _queues.resize( DefaultRenderQueueCount );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::addRenderData( PrefabPtr prefab,
                                       NodePtr node )
{
  // TODO: Consider a way of optimizing this by skipping processing data here
  // and directly storing references to nodes in lists in the renderer.
  const uint queueId = prefab->getRenderQueue();
  const bool blended = prefab->getMaterial()->blendingMode.enabled;

  // TODO: Fix two lookups here.
  // Add this queue to the active queue list if not already there.
  _activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this prefab at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  if ( blended ) {
    RenderQueue::PrefabNodePair pair { prefab, node };
    queue.transparents.insert( { glm::length2( _camera->getPosition() - node->getPosition() ), pair } );
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
    queue.lights.pointLights.emplace_back( static_cast<PointLightPtr>( light ), node->getWorldPosition() );
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

  //
  // Render shadow maps first.
  
  _renderShadowMaps( rv, _queues.at( RenderQueue::General ) );

  //
  // Render scene.

  real aspectRatio = 0.f;
  RenderTargetPtr rt = nullptr;
  if ( rv.camera->postProcessing ) {
    _api->setViewport( 0,
                       0,
                       static_cast<uint32_t>( rv.viewport.w * rv.camera->postProcessing->renderTarget->getWidth() ),
                       static_cast<uint32_t>( rv.viewport.h * rv.camera->postProcessing->renderTarget->getHeight() ) );
    rv.camera->postProcessing->renderTarget->bind();
    aspectRatio = rv.camera->postProcessing->renderTarget->getAspectRatio();

    rt = rv.camera->postProcessing->renderTarget;
  } else if ( rv.renderTarget ) {
    _api->setViewport( 0,
                       0,
                       static_cast< uint32_t >( rv.viewport.w * rv.renderTarget->getWidth() ),
                       static_cast< uint32_t >( rv.viewport.h * rv.renderTarget->getHeight() ) );
    rv.renderTarget->bind();
    aspectRatio = rv.renderTarget->getAspectRatio();

    rt = rv.renderTarget;
  }
  else {
    // TODO: Get rid of gl_viewport.
    _api->setViewport( rv.gl_viewport.x,
                       rv.gl_viewport.y,
                       rv.gl_viewport.width,
                       rv.gl_viewport.height );

    _api->bindDefaultFramebuffer();
    aspectRatio = rv.gl_viewport.aspectRatio;
  }

  Color bg = rv.scene->getSkyboxColor();
  _api->clear();
  _api->clearColor( bg.r, bg.g, bg.b, 1.f );
  _api->setPolygonMode( IRenderAPI::PolygonMode::Fill );
  _api->setCullingMode( IRenderAPI::CullingMode::Back );
  _api->setDepthTestEnabled( true );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
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
  if ( rt ) {
    // We don't want to render to the 2nd color output (bright buffer), so bloom isn't occluded by transparent objects.
    rt->setColorAttachmentCount( 1 );
  }
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;
    _renderTransparents( rv, queue, viewProjection );
  }
  if ( rt ) {
    // Restore bright buffer output.
    rt->setColorAttachmentCount( 2 );
  }

  if ( rv.renderTarget ) {
    rv.renderTarget->flush();
    _api->bindDefaultFramebuffer();
  }

  // Post processing.
  if ( rv.camera->postProcessing ) {
    rv.camera->postProcessing->renderTarget->flush();
    _presentPostProcessing( rv, window );
  }

  _clearRenderQueues();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_presentPostProcessing( const RenderView& rv,
                                                const WindowPtr window )
{
  real aspectRatio = 0.f;
  if ( rv.renderTarget ) {
    _api->setViewport( 0,
                       0,
                       static_cast<uint32_t>( rv.viewport.w * rv.renderTarget->getWidth() ),
                       static_cast<uint32_t>( rv.viewport.h * rv.renderTarget->getHeight() ) );
    rv.renderTarget->bind();
    aspectRatio = rv.renderTarget->getAspectRatio();
  }
  else {
    // TODO: Get rid of gl_viewport.
    _api->setViewport( rv.gl_viewport.x,
                       rv.gl_viewport.y,
                       rv.gl_viewport.width,
                       rv.gl_viewport.height );

    _api->bindDefaultFramebuffer();
    aspectRatio = rv.gl_viewport.aspectRatio;
  }

  _api->clear();
  _api->clearColor( 1.f, 0.f, 0.f, 1.f );

  const glm::mat4 projection = glm::ortho( -aspectRatio, aspectRatio,
                                           -1.f, 1.f,
                                           1.f, -1.f );
  static glm::mat4 view = Math::CreateTransformationMatrix( Vec3Zero,
                                                            glm::quat( 1.f, 0.f, 0.f, 0.f ),
                                                            glm::vec3( 1.f, 1.f, 0.f ) );
  const glm::mat4 viewProjection = projection * view;

  _api->setCullingMode( IRenderAPI::CullingMode::Back );

  const Camera::PostProcessing* p = rv.camera->postProcessing.get();

  //
  // First do a Gaussian blur for bloom.

  ModelPtr blurModel = p->doubleBufferPrefab->getModel();
  GPUProgramPtr blurProgram = p->doubleBufferPrefab->_material->program;
  blurProgram->use();

  int blurAmount = 10;
#ifdef LORE_DEBUG_UI
  blurAmount = DebugConfig::bloomBlurPassCount;
#endif

  TexturePtr blurBuffer = p->doubleBuffer->getTexture();

  bool horizontal = true, firstIt = true;
  for ( int i = 0; i < blurAmount; ++i ) {
    p->doubleBuffer->bind( static_cast<u32>( horizontal ) );

    blurProgram->setUniformVar( "horizontal", horizontal );
    if ( firstIt ) {
      // Bind the bright pixel color buffer for first iteration.
      p->renderTarget->getTexture()->bind( 0, 1 );
      firstIt = false;
    }
    else {
      const auto idx = static_cast<u32>( !horizontal );
      blurBuffer->bind( 0, idx);
    }

    blurModel->draw( blurProgram );

    horizontal = !horizontal;
  }

  if ( rv.renderTarget ) {
    rv.renderTarget->bind();
  }
  else {
    _api->bindDefaultFramebuffer();
  }

  //
  // Render the final output to a fullscreen quad.

  _api->clear();

  ModelPtr model = p->prefab->getModel();
  GPUProgramPtr program = p->prefab->_material->program;
  program->use();

  TexturePtr buffer = p->renderTarget->getTexture();
  buffer->bind( 0 );
  program->setUniformVar( "frameBuffer", 0 );

  const auto bloomIdx = static_cast<u32>( !horizontal );
  blurBuffer->bind( 1, bloomIdx );
  program->setUniformVar( "bloomBlur", 1 );

  program->setUniformVar( "gamma", p->gamma );

  // Debug UI enabled values.
#ifdef LORE_DEBUG_UI
  program->setUniformVar( "exposure", DebugConfig::hdrExposure );
#else
  program->setUniformVar( "exposure", p->exposure );
#endif

  model->draw( program );
}

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

void Forward3DRenderer::_renderShadowMaps( const RenderView& rv,
                                           const RenderQueue& queue )
{
  _api->setCullingMode( IRenderAPI::CullingMode::Back );

  // Directional lights.
  for ( const auto& dirLight : queue.lights.directionalLights ) {
    // Bind this light's shadow map.
    if ( !dirLight->shadowMap ) {
      continue;
    }

    _api->setViewport( 0, 0, dirLight->shadowMap->getWidth(), dirLight->shadowMap->getHeight() );
    dirLight->shadowMap->bind();
    _api->clearDepthBufferBit();

    const auto orthoSize = 10.f;
    glm::mat4 lightProj = glm::ortho( -orthoSize, orthoSize, -orthoSize, orthoSize, 1.f, 60.f );
    glm::mat4 lightView = glm::lookAt( -dirLight->getDirection() * 20.f,
                                       Vec3Zero,
                                       Vec3PosY );

    GPUProgramPtr shadowProgram = StockResource::GetGPUProgram( "DirectionalShadowMapInstanced" );
    shadowProgram->use();

    dirLight->viewProj = lightProj * lightView;
    shadowProgram->setUniformVar( "viewProjection", dirLight->viewProj );

    // Instanced solids.
    for ( const auto& prefab : queue.instancedSolids ) {
      ModelPtr model = prefab->getInstancedModel();

      const NodePtr node = prefab->getInstanceControllerNode();

      shadowProgram->updateUniforms( rv, nullptr, queue.lights );
      shadowProgram->updateNodeUniforms( nullptr, node, dirLight->viewProj );

      model->draw( shadowProgram, prefab->getInstanceCount(), false, false );
    }

    shadowProgram = StockResource::GetGPUProgram( "DirectionalShadowMap" );
    shadowProgram->use();
    shadowProgram->setUniformVar( "viewProjection", dirLight->viewProj );

    // Render non-instanced solids.
    for ( auto& pair : queue.solids ) {
      const PrefabPtr prefab = pair.first;
      const RenderQueue::NodeList& nodes = pair.second;
      const ModelPtr model = prefab->getModel();

      // Render each node associated with this prefab.
      for ( const auto& node : nodes ) {
        shadowProgram->updateNodeUniforms( nullptr, node, dirLight->viewProj ); // Note: dirLight->viewProj not used.
        model->draw( shadowProgram, 0, false, false );
      }
    }

    // Render transparents.
    // TODO: Account for shadow strength based on opacity...
    for ( auto it = queue.transparents.rbegin(); it != queue.transparents.rend(); ++it ) {
      const PrefabPtr prefab = it->second.first;
      NodePtr node = it->second.second;
      ModelPtr model = prefab->getModel();

      shadowProgram->updateNodeUniforms( nullptr, node, dirLight->viewProj ); // Note: dirLight->viewProj not used.
      model->draw( shadowProgram, 0, false, false );
    }
  }

  // Point lights.
  for ( const auto& pointLightPair : queue.lights.pointLights ) {
    const auto& light = pointLightPair.first;

    if ( !light->shadowMap ) {
      continue;
    }

    const PointLightPtr pointLight = static_cast<PointLightPtr> ( light );
    const auto& lightPos = pointLightPair.second;

    _api->setViewport( 0, 0, pointLight->shadowMap->getWidth(), pointLight->shadowMap->getHeight() );
    pointLight->shadowMap->bind();
    _api->clearDepthBufferBit();


    // Instanced solids.
    GPUProgramPtr shadowProgram = StockResource::GetGPUProgram( "OmnidirectionalShadowMapInstanced" );
    shadowProgram->use();

    for ( int i = 0; i < 6; ++i ) {
      shadowProgram->setUniformVar( "shadowMatrices[" + std::to_string( i ) + "]", pointLight->shadowTransforms[i] );
    }
    shadowProgram->setUniformVar( "lightPos", lightPos );
    shadowProgram->setUniformVar( "farPlane", pointLight->shadowFarPlane );

    for ( const auto& prefab : queue.instancedSolids ) {
      ModelPtr model = prefab->getInstancedModel();

      const NodePtr node = prefab->getInstanceControllerNode();

      glm::mat4 ident; // Not used in updater.
      shadowProgram->updateNodeUniforms( nullptr, node, ident );

      model->draw( shadowProgram, prefab->getInstanceCount(), false, false );
    }

    // Non-instanced solids.
    shadowProgram = StockResource::GetGPUProgram( "OmnidirectionalShadowMap" );
    shadowProgram->use();

    for ( int i = 0; i < 6; ++i ) {
      shadowProgram->setUniformVar( "shadowMatrices[" + std::to_string( i ) + "]", pointLight->shadowTransforms[i] );
    }
    shadowProgram->setUniformVar( "lightPos", lightPos );
    shadowProgram->setUniformVar( "farPlane", pointLight->shadowFarPlane );

    for ( auto& pair : queue.solids ) {
      const PrefabPtr prefab = pair.first;
      const RenderQueue::NodeList& nodes = pair.second;
      const ModelPtr model = prefab->getModel();

      // Render each node associated with this prefab.
      glm::mat4 ident; // Not used in updater.
      for ( const auto& node : nodes ) {
        shadowProgram->updateNodeUniforms( nullptr, node, ident );
        model->draw( shadowProgram, 0, false, false );
      }
    }

    // Render transparents.
    // TODO: Account for shadow strength based on opacity...
    for ( auto it = queue.transparents.rbegin(); it != queue.transparents.rend(); ++it ) {
      const PrefabPtr prefab = it->second.first;
      NodePtr node = it->second.second;
      ModelPtr model = prefab->getModel();

      glm::mat4 ident; // Not used in updater.
      shadowProgram->updateNodeUniforms( nullptr, node, ident );
      model->draw( shadowProgram, 0, false, false );
    }
  }

  _api->setCullingMode( IRenderAPI::CullingMode::Back);
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

    if ( rv.camera->postProcessing ) {
      program->setUniformVar( "gamma", rv.camera->postProcessing->gamma );
    }
    else {
      program->setUniformVar( "gamma", 1.0f );
    }

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
  for ( const auto& prefab : queue.instancedSolids ) {
    MaterialPtr material = prefab->getMaterial();
    ModelPtr model = prefab->getInstancedModel();
    GPUProgramPtr program = nullptr;

    const NodePtr node = prefab->getInstanceControllerNode();

    // Acquire the correct GPUProgram for this instanced model type.
    switch ( model->getType() ) {
    default:
      throw Lore::Exception( "Instanced prefab must have an instanced model" );

    case Mesh::Type::Quad3DInstanced:
    case Mesh::Type::CubeInstanced:
      program = StockResource::GetGPUProgram( "StandardInstanced3D" );
      break;

    case Mesh::Type::TexturedQuad3DInstanced:
    case Mesh::Type::TexturedCubeInstanced:
    case Mesh::Type::CustomInstanced:
      if ( material->sprite->getTextureCount( 0, Texture::Type::Normal ) > 0 ) {
        program = StockResource::GetGPUProgram( "StandardTexturedNormalMappingInstanced3D" );
      }
      else {
        program = StockResource::GetGPUProgram( "StandardTexturedInstanced3D" );
      }
      break;
    }

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

void Forward3DRenderer::_renderTransparents( const RenderView& rv,
                                             const RenderQueue& queue,
                                             const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );

  // Render in reverse order, so the farthest back is rendered first.
  for ( auto it = queue.transparents.rbegin(); it != queue.transparents.rend(); ++it ) {
    const PrefabPtr prefab = it->second.first;
    NodePtr node = it->second.second;

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
