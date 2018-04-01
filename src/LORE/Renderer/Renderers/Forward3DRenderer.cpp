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

    _renderSolids( rv, queue, viewProjection );
    _renderTransparents( rv, queue, viewProjection );
  }

  // Render UI.
  if ( rv.ui ) {
    _renderUI( rv.ui, rv, aspectRatio, projection );
  }

  // Render debug UI if enabled.
  if ( DebugUI::IsStatsUIEnabled() ) {
    _renderUI( DebugUI::GetStatsUI(), rv, aspectRatio, projection );
  }
  if ( DebugUI::IsConsoleEnabled() ) {
    _renderUI( DebugUI::GetConsoleUI(), rv, aspectRatio, projection );
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

    vertexBuffer->bind();
    program->use();
    program->updateUniforms( rv, material, queue.lights );

    // Render each node associated with this entity.
    for ( const auto& node : nodes ) {
      program->updateNodeUniforms( material, node, viewProjection );
      vertexBuffer->draw();
    }

    // Rendering this entity is complete.
    vertexBuffer->unbind();
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
    VertexBufferPtr vertexBuffer = entity->getMesh()->getVertexBuffer();

    if ( entity->isInstanced() ) {
      node = entity->getInstanceControllerNode();
      vertexBuffer = entity->getInstancedVertexBuffer();
      switch ( vertexBuffer->getType() ) {
      default:
        throw Lore::Exception( "Instanced entity must have an instanced vertex buffer" );

      case VertexBuffer::Type::QuadInstanced:
        program = StockResource::GetGPUProgram( "StandardInstanced2D" );
        break;

      case VertexBuffer::Type::TexturedQuadInstanced:
        program = StockResource::GetGPUProgram( "StandardTexturedInstanced2D" );
        break;
      }
    }

    // Set blending mode using material settings.
    _api->setBlendingFunc( material->blendingMode.srcFactor, material->blendingMode.dstFactor );

    vertexBuffer->bind();

    program->use();
    program->updateUniforms( rv, material, queue.lights );
    program->updateNodeUniforms( material, node, viewProjection );

    // Draw the entity.
    vertexBuffer->draw( entity->getInstanceCount() );
    vertexBuffer->unbind();
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
  VertexBufferPtr vb = StockResource::GetVertexBuffer( "TexturedQuad" );

  program->use();
  vb->bind();

  for ( const RenderQueue::BoxData& data : queue.boxes ) {
    BoxPtr box = data.box;
    program->setUniformVar( "borderColor", box->getBorderColor() );
    program->setUniformVar( "fillColor", box->getFillColor() );
    program->setUniformVar( "borderWidth", box->getBorderWidth() );
    program->setUniformVar( "scale", glm::vec2( data.model[0][0], data.model[1][1] ) * box->getSize() );

    // Apply box scaling to final transform.
    glm::mat4 model = data.model;
    model[0][0] *= box->getWidth();
    model[1][1] *= box->getHeight();
    program->setTransformVar( viewProjection * model );

    vb->draw();
  }

  vb->unbind();
  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderTextboxes( const RenderQueue& queue,
                                          const glm::mat4& viewProjection ) const
{
  _api->setBlendingEnabled( true );
  _api->setBlendingFunc( Material::BlendFactor::SrcAlpha, Material::BlendFactor::OneMinusSrcAlpha );

  GPUProgramPtr program = StockResource::GetGPUProgram( "StandardText" );
  VertexBufferPtr vb = StockResource::GetVertexBuffer( "StandardText" );

  program->use();
  vb->bind();

  for ( auto& data : queue.textboxes ) {
    TextboxPtr textbox = data.textbox;
    string text = textbox->getText();
    FontPtr font = textbox->getFont();

    // TODO: Use a better design for this relationship.
    program->setUniformVar( "projection", viewProjection );
    program->setUniformVar( "depth", data.model[3][2] );
    program->setUniformVar( "color", textbox->getTextColor() );

    // Render each character.
    real x = data.model[3][0];
    const real y = data.model[3][1];
    const real scale = 1.f;
    for ( char & c : text ) {
      // Generate vertices to draw glyph and bind its associated texture.
      auto vertices = font->generateVertices( c,
                                              x,
                                              y,
                                              scale );
      font->bindTexture( c );

      vb->draw( vertices );

      x = font->advanceGlyphX( c, x, scale );
    }

  }

  vb->unbind();
  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Forward3DRenderer::_renderUI( const UIPtr ui,
                                   const RenderView& rv,
                                   const real aspectRatio,
                                   const glm::mat4& proj ) const
{
  auto panelIt = ui->getPanelIterator();

  RenderQueue queue;
  constexpr const real DepthOffset = -1101.f;

  std::vector<Node> nodes;
  nodes.resize( 10 );
  int idx = 0;

  // For each panel.
  while ( panelIt.hasMore() ) {
    auto panel = panelIt.getNext();
    const glm::vec2 panelOrigin = panel->getOrigin();

    // For each element in this panel.
    auto elementIt = panel->getElementIterator();
    while ( elementIt.hasMore() ) {
      auto element = elementIt.getNext();
      if ( !element->isVisible() ) {
        continue;
      }

      auto elementPos = panelOrigin + element->getPosition();
      auto elementDimensions = element->getDimensions();
      elementPos.x *= aspectRatio;
      elementDimensions.x *= aspectRatio;

      // Build list of entity data and textboxes to render.
      auto entity = element->getEntity();
      if ( entity ) {
        auto mat = entity->getMaterial();

        Node& node = nodes[idx++];
        node.setName( element->getName() );
        node.setPosition( elementPos );
        node.setScale( elementDimensions );
        node.setDepth( element->getDepth() + DepthOffset ); // Offset UI element depth beyond scene node depth.
        node.updateWorldTransform();

        if ( mat->blendingMode.enabled ) {
          RenderQueue::EntityNodePair pair { entity, &node };
          queue.transparents.insert( { node.getDepth(), pair } );
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
            RenderQueue::NodeList& solidNodes = queue.solids[entity];
            solidNodes.push_back( &node );
          }
        }
      }

      auto box = element->getBox();
      if ( box ) {
        RenderQueue::BoxData bd;
        bd.box = box;
        glm::quat q( 1.f, 0.f, 0.f, 0.f );
        bd.model = Math::CreateTransformationMatrix( glm::vec3( elementPos.x, elementPos.y, 0.f ), q, glm::vec3( elementDimensions.x, elementDimensions.y, 1.f ) );
        bd.model[3][2] = element->getDepth() + DepthOffset;
        queue.boxes.push_back( bd );
      }

      auto textbox = element->getTextbox();
      if ( textbox ) {
        RenderQueue::TextboxData td;
        td.textbox = textbox;
        glm::quat q( 1.f, 0.f, 0.f, 0.f );
        td.model = Math::CreateTransformationMatrix( glm::vec3( elementPos.x, elementPos.y, 0.f ), q, glm::vec3( 1.f ) );
        td.model[3][2] = element->getDepth() + DepthOffset - 0.01f; // Give text a little room over boxes.
        queue.textboxes.push_back( td );
      }
    }
  }

  const glm::mat4 projection = glm::ortho( -aspectRatio, aspectRatio,
                                           -1.f, 1.f,
                                           1500.f, -1500.f );

  // Now render the UI elements.
  _renderSolids( rv, queue, projection );
  _renderTransparents( rv, queue, projection );
  _renderBoxes( queue, projection );
  _renderTextboxes( queue, projection );

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
