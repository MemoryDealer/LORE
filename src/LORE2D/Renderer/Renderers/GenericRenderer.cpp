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

#include <LORE2D/Core/DebugUI/DebugUI.h>
#include <LORE2D/Config/Config.h>
#include <LORE2D/Math/Math.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Renderer/IRenderAPI.h>
#include <LORE2D/Resource/Font.h>
#include <LORE2D/Resource/Mesh.h>
#include <LORE2D/Resource/Renderable/Box.h>
#include <LORE2D/Resource/Renderable/Sprite.h>
#include <LORE2D/Resource/Renderable/Textbox.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Texture.h>
#include <LORE2D/Scene/AABB.h>
#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Scene/Light.h>
#include <LORE2D/Scene/Scene.h>
#include <LORE2D/Scene/SpriteController.h>
#include <LORE2D/Shader/GPUProgram.h>
#include <LORE2D/UI/UI.h>
#include <LORE2D/Window/RenderTarget.h>
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

  // TODO: Fix two lookups here.
  // Add this queue to the active queue list if not already there.
  activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this entity at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  // Get the active frame if this sprite is being manipulated with a SpriteController.
  size_t spriteFrame = 0;
  bool xFlipped = false;
  bool yFlipped = false;
  const auto spc = node->getSpriteController();
  if ( spc ) {
    spriteFrame = spc->getActiveFrame();
    xFlipped = spc->getXFlipped();
    yFlipped = spc->getYFlipped();
  }

  if ( blended ) {
    RenderQueue::Transparent t;
    t.material = e->getMaterial();
    t.vertexBuffer = e->getMesh()->getVertexBuffer();
    t.model = node->getFullTransform();
    t.xFlipped = xFlipped;
    t.yFlipped = yFlipped;
    t.spriteFrame = spriteFrame;

    const auto depth = node->getDepth();
    t.model[3][2] = depth;

    queue.transparents.insert( { depth, t } );
  }
  else {
    // Acquire the render data list for this material/vb (or create one).
    RenderQueue::EntityData entityData;
    entityData.material = e->getMaterial();
    entityData.vertexBuffer = e->getMesh()->getVertexBuffer();
    entityData.spriteFrame = spriteFrame;

    RenderQueue::RenderDataList& renderData = queue.solids[entityData];

    // Fill out the render data and add it to the list.
    RenderQueue::RenderData rd;
    rd.model = node->getFullTransform();
    rd.model[3][2] = node->getDepth();
    rd.xFlipped = xFlipped;
    rd.yFlipped = yFlipped;

    renderData.push_back( rd );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::addBox( Lore::BoxPtr box,
                              const Lore::Matrix4& transform )
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

void GenericRenderer::addTextbox( Lore::TextboxPtr textbox,
                                  const Lore::Matrix4& transform )
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

void GenericRenderer::addLight( Lore::LightPtr light, const Lore::NodePtr node )
{
  const uint queueId = RenderQueue::General;

  activateQueue( queueId, _queues[queueId] );
  RenderQueue& queue = _queues.at( queueId );

  RenderQueue::LightData data;
  data.light = light;
  data.pos = node->getDerivedPosition();

  queue.lights.push_back( data );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::present( const Lore::RenderView& rv, const Lore::WindowPtr window )
{
  // Build render queues for this RenderView.
  rv.scene->updateSceneGraph();

  const real aspectRatio = (rv.renderTarget) ? rv.renderTarget->getAspectRatio() : window->getAspectRatio();
  rv.camera->updateTracking( aspectRatio );

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

  Color bg = rv.scene->getBackgroundColor();
  _api->clear();
  _api->clearColor( bg.r, bg.g, bg.b, 0.f );
  _api->setPolygonMode( IRenderAPI::PolygonMode::Fill );

  // Setup view-projection matrix.
  // TODO: Take viewport dimensions into account. Cache more things inside window.
  const Matrix4 projection = Math::OrthoRH( -aspectRatio, aspectRatio,
                                            -1.f, 1.f,
                                            1500.f, -1500.f );

  const Matrix4 viewProjection = rv.camera->getViewMatrix() * projection;

  // Render background before scene node entities.
  renderBackground( rv, aspectRatio, projection );

  // Iterate through all active render queues and render each object.
  for ( const auto& activeQueue : _activeQueues ) {
    RenderQueue& queue = activeQueue.second;

    // Render solids.
    renderMaterialMap( rv.scene, queue, viewProjection );

    // Render transparents.
    renderTransparents( rv.scene, queue, viewProjection );

    // Render boxes.
    renderBoxes( queue, viewProjection );

    // Render text.
    renderTextboxes( queue, viewProjection );
  }

  // AABBs.
  auto renderAABBs = Config::GetValue( "RenderAABBs" );
  if ( GET_VARIANT<bool>( renderAABBs ) ) {
    RenderQueue tmpQueue;

    std::function<void( NodePtr )> AddAABB = [&] ( NodePtr node ) {
      AABBPtr aabb = node->getAABB();
      if ( aabb ) {
        RenderQueue::BoxData data;
        data.box = aabb->getBox();
        data.model = Math::CreateTransformationMatrix( node->getDerivedPosition(), Quaternion(), aabb->getDimensions() * 5.f );
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
  }

  // Render UI.
  if ( rv.ui ) {
    renderUI( rv.ui, rv.scene, aspectRatio, projection );
  }

  // Render debug UI if enabled.
  if ( DebugUI::IsStatsUIEnabled() ) {
    renderUI( DebugUI::GetStatsUI(), rv.scene, aspectRatio, projection );
  }
  if ( DebugUI::IsConsoleEnabled() ) {
    renderUI( DebugUI::GetConsoleUI(), rv.scene, aspectRatio, projection );
  }

  if ( rv.renderTarget ) {
    // Re-bind default frame buffer if custom render target was specified.
    _api->bindDefaultFramebuffer();
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
    queue.boxes.clear();
    queue.textboxes.clear();
    queue.lights.clear();
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

    if ( mat->sprite && mat->sprite->getTextureCount() ) {
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

      TexturePtr texture = mat->sprite->getTexture( spriteFrame );
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
      offset.y += camPos.y * layer.getParallax().y;
      program->setUniformVar( "texSampleOffset", offset );

      Lore::Matrix4 transform = Math::CreateTransformationMatrix( Lore::Vec2( 0.f, 0.f ) );
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
                                         const RenderQueue& queue,
                                         const Lore::Matrix4& viewProjection ) const
{
  // Iterate over render data lists for each material.

  for ( auto& pair : queue.solids ) {

    const RenderQueue::EntityData& entityData = pair.first;
    const RenderQueue::RenderDataList& renderDataList = pair.second;

    //
    // Bind material settings to GPU.

    MaterialPtr mat = entityData.material;
    GPUProgramPtr program = mat->program;
    VertexBufferPtr vertexBuffer = entityData.vertexBuffer;

    program->use();
    if ( mat->sprite && mat->sprite->getTextureCount() ) {
      const auto spriteFrame = pair.first.spriteFrame;
      TexturePtr texture = mat->sprite->getTexture( spriteFrame );

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
      program->setUniformVar( "numLights", static_cast<int>( queue.lights.size() ) );

      program->updateLights( queue.lights );

    }

    vertexBuffer->bind();

    // Render each node associated with this entity data.
    for ( const RenderQueue::RenderData& rd : renderDataList ) {

      Vec3 scale( 1.f, 1.f, 1.f );
      if ( rd.xFlipped ) {
        scale.x = -1.f;
      }
      if ( rd.yFlipped ) {
        scale.y = -1.f;
      }

      const Matrix4 FlipMatrix = Math::CreateTransformationMatrix( Lore::Vec3(), Lore::Quaternion(), scale );

      // Calculate model-view-projection matrix for this object.
      Matrix4 mvp = viewProjection * rd.model * FlipMatrix;

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
                                          const RenderQueue& queue,
                                          const Lore::Matrix4& viewProjection ) const
{
  _api->setBlendingEnabled( true );

  // Render in forward order, so the farthest back is rendered first.
  // (Depth values increase going farther back).
  for ( auto it = queue.transparents.begin(); it != queue.transparents.end(); ++it ) {

    const RenderQueue::Transparent& t = it->second;
    MaterialPtr mat = t.material;
    GPUProgramPtr program = mat->program;
    VertexBufferPtr vertexBuffer = t.vertexBuffer;

    // Set blending mode using material settings.
    _api->setBlendingFunc( mat->blendingMode.srcFactor, mat->blendingMode.dstFactor );

    program->use();
    if ( mat->sprite && mat->sprite->getTextureCount() ) {
      const auto spriteFrame = it->second.spriteFrame;
      TexturePtr texture = mat->sprite->getTexture( spriteFrame );

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
      program->setUniformVar( "numLights", static_cast<int>( queue.lights.size() ) );

      program->updateLights( queue.lights );

    }

    vertexBuffer->bind();

    {
      // Calculate model-view-projection matrix for this object.
      Vec3 scale( 1.f, 1.f, 1.f );
      if ( t.xFlipped ) {
        scale.x = -1.f;
      }
      if ( t.yFlipped ) {
        scale.y = -1.f;
      }

      const Matrix4 FlipMatrix = Math::CreateTransformationMatrix( Lore::Vec3(), Lore::Quaternion(), scale );

      Matrix4 mvp = viewProjection * t.model * FlipMatrix;

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

void GenericRenderer::renderBoxes( const RenderQueue& queue,
                                   const Matrix4& viewProjection ) const
{
  _api->setBlendingEnabled( true );
  _api->setBlendingFunc( Material::BlendFactor::SrcAlpha, Material::BlendFactor::OneMinusSrcAlpha );

  GPUProgramPtr program = StockResource::GetGPUProgram( "StandardBox" );
  VertexBufferPtr vb = StockResource::GetVertexBuffer( "TexturedQuad" );

  program->use();
  vb->bind();

  for ( auto& data : queue.boxes ) {
    BoxPtr box = data.box;
    program->setUniformVar( "borderColor", box->getBorderColor() );
    program->setUniformVar( "fillColor", box->getFillColor() );
    program->setUniformVar( "borderWidth", box->getBorderWidth() );
    program->setUniformVar( "scale", Vec2( data.model[0][0], data.model[1][1] ) * box->getSize() );

    // Apply box scaling to final transform.
    data.model[0][0] *= box->getWidth();
    data.model[1][1] *= box->getHeight();
    program->setTransformVar( viewProjection * data.model );

    vb->draw();
  }

  vb->unbind();
  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderTextboxes( const RenderQueue& queue,
                                       const Matrix4& viewProjection ) const
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
    for ( auto c = text.begin(); c != text.end(); ++c ) {
      // Generate vertices to draw glyph and bind its associated texture.
      auto vertices = font->generateVertices( *c,
                                              x,
                                              y,
                                              scale );
      font->bindTexture( *c );

      vb->draw( vertices );

      x = font->advanceGlyphX( *c, x, scale );
    }

  }

  vb->unbind();
  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderUI( const Lore::UIPtr ui,
                                const Lore::ScenePtr scene,
                                const real aspectRatio,
                                const Matrix4& proj ) const
{
  auto panelIt = ui->getPanelIterator();

  RenderQueue queue;
  constexpr const real DepthOffset = -1101.f;

  // For each panel.
  while ( panelIt.hasMore() ) {
    auto panel = panelIt.getNext();
    const Vec2 panelOrigin = panel->getOrigin();

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

        RenderQueue::RenderData rd;
        rd.model = Math::CreateTransformationMatrix( elementPos, Quaternion(), elementDimensions );
        rd.model[3][2] = element->getDepth() + DepthOffset; // Offset UI element depth beyond scene node depth.

        if ( mat->blendingMode.enabled ) {
          RenderQueue::Transparent t;
          t.material = entity->getMaterial();
          t.vertexBuffer = entity->getMesh()->getVertexBuffer();
          t.model = rd.model;

          queue.transparents.insert( { t.model[3][2], t } );
        }
        else {
          RenderQueue::EntityData entityData;
          entityData.material = mat;
          entityData.vertexBuffer = entity->getMesh()->getVertexBuffer();

          queue.solids[entityData].push_back( rd );
        }
      }

      auto box = element->getBox();
      if ( box ) {
        RenderQueue::BoxData bd;
        bd.box = box;
        bd.model = Math::CreateTransformationMatrix( elementPos, Quaternion(), elementDimensions );
        bd.model[3][2] = element->getDepth() + DepthOffset;
        queue.boxes.push_back( bd );
      }

      auto textbox = element->getTextbox();
      if ( textbox ) {
        RenderQueue::TextboxData td;
        td.textbox = textbox;
        td.model = Math::CreateTransformationMatrix( elementPos );
        td.model[3][2] = element->getDepth() + DepthOffset;
        queue.textboxes.push_back( td );
      }
    }
  }

  // Now render the UI elements.
  renderMaterialMap( scene, queue, proj );
  renderTransparents( scene, queue, proj );
  renderBoxes( queue, proj );
  renderTextboxes( queue, proj );

  _api->setBlendingEnabled( false );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
