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

// TODO: Pull this class out of GL plugin and abstract away GL calls.
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

  // Add this queue to the active queue list if not already there.
  activateQueue( queueId, _queues[queueId] );

  //
  // Add render data for this entity at the node's position to the queue.

  RenderQueue& queue = _queues.at( queueId );

  // Acquire the render data list for this material/vb (or create one).
  RenderQueue::EntityData entityData;
  entityData.material = e->getMaterial();
  entityData.vertexBuffer = e->getMesh()->getVertexBuffer();

  RenderQueue::RenderDataList& renderData = queue.solids[entityData];

  // Fill out the render data and add it to the list.
  RenderQueue::RenderData rd;
  rd.model = node->getFullTransform();
  rd.model[3][2] = static_cast< real >( node->getDepth() );
  rd.colorModifier = node->getColorModifier();

  renderData.push_back( rd );
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
                                            1000.f, -1000.f );

  const Matrix4 viewProjection = rv.camera->getViewMatrix() * projection;

  // Render background before scene node entities.
  renderBackground( rv.scene, projection );

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
  const auto lookup = _activeQueues.find( id );
  if ( _activeQueues.end() == lookup ) {
    _activeQueues.insert( { id, rq } );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GenericRenderer::renderBackground( const Lore::ScenePtr scene, const Lore::Matrix4& proj )
{
  BackgroundPtr background = scene->getBackground();
  Background::LayerMap layers = background->getLayerMap();

  VertexBufferPtr vb = Lore::StockResource::GetVertexBuffer( "Background" );
  vb->bind();

  for( const auto& pair : layers ){
    const Background::Layer& layer = pair.second;
    MaterialPtr mat = layer.getMaterial();

    Material::Pass& pass = mat->getPass();
    GPUProgramPtr program = pass.program;
    TexturePtr texture = pass.texture;

    if ( texture ) {
      program->use();
      texture->bind();
      program->setUniformVar( "texSampleOffset", pass.getTexCoordOffset() );

      Lore::Rect sampleRegion = pass.getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );

      switch ( layer.getMode() ) {
      default:
      case Background::Layer::Mode::Static:
        // Draw on left half of viewport.
        {
          Lore::Matrix4 transform = Math::CreateTransformationMatrix( Lore::Vec2( -1.f, 0.f ), Lore::Quaternion() );
          transform[3][2] = layer.getDepth();
          program->setTransformVar( proj * transform );

          vb->draw();
        }

        // Draw on right half of viewport.
        {
          Lore::Matrix4 transform = Math::CreateTransformationMatrix( Lore::Vec2( 1.f, 0.f ), Lore::Quaternion() );
          transform[3][2] = layer.getDepth();
          program->setTransformVar( proj * transform );

          vb->draw();
        }
        break;

      case Background::Layer::Mode::Dynamic:
        {
          Lore::Matrix4 transform = Math::CreateTransformationMatrix( Lore::Vec2( 0.f, 0.f ), Lore::Quaternion() );
          transform[3][2] = layer.getDepth();
          program->setTransformVar( proj * transform );

          vb->draw();
        }
        break;
      }
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

    // TODO: Multi-pass.
    Material::Pass& pass = entityData.material->getPass();
    GPUProgramPtr program = pass.program;
    VertexBufferPtr vertexBuffer = entityData.vertexBuffer;
    TexturePtr texture = pass.texture;

    program->use();
    if ( texture ) {
      // TODO: Multi-texturing.
      texture->bind();
      program->setUniformVar( "texSampleOffset", pass.getTexCoordOffset() );

      Lore::Rect sampleRegion = pass.getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
    }

    // Upload lighting data.
    if ( pass.lighting ) {

      // Material.
      program->setUniformVar( "material.ambient", pass.ambient );
      program->setUniformVar( "material.diffuse", pass.diffuse );

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

      if ( pass.lighting ) {
        program->setUniformVar( "model", rd.model );
      }

      if ( pass.colorMod ) {
        program->setUniformVar( "colorMod", rd.colorModifier );
      }

      // Draw the entity.
      vertexBuffer->draw();

    }

    vertexBuffer->unbind();

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
