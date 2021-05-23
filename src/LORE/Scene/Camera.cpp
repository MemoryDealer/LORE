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

#include "Camera.h"

#include <LORE/Math/Math.h>
#include <LORE/Resource/Entity.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  constexpr real ZoomLimit = 0.1f;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Camera::~Camera()
{
  Resource::DestroyRenderTarget( postProcessing->renderTarget );
  Resource::DestroyRenderTarget( postProcessing->doubleBuffer );
  Resource::DestroySprite( postProcessing->entity->_material->sprite );
  Resource::DestroySprite( postProcessing->doubleBufferEntity->_material->sprite );
  Resource::DestroyEntity( postProcessing->entity );
  Resource::DestroyEntity( postProcessing->doubleBufferEntity );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const glm::vec2& pos )
{
  setPosition( glm::vec3( pos.x, pos.y, 0.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const real x, const real y )
{
  setPosition( x, y, 0.f ); // Call glm::vec3 function with z-value of zero.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const real x, const real y, const real z )
{
  setPosition( glm::vec3( x, y, 0.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const glm::vec2& offset )
{
  translate( glm::vec3( offset.x, offset.y, 0.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const real xOffset, const real yOffset )
{
  translate( xOffset, yOffset, 0.f ); // Call glm::vec3 function with z-value of zero.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const real xOffset, const real yOffset, const real zOffset )
{
  translate( glm::vec3( xOffset, yOffset, zOffset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::zoom( const real amount )
{
  _zoom += amount * _zoom; // Scale zooming speed as it goes farther.
  if ( _zoom < ZoomLimit ) {
    _zoom = ZoomLimit;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setZoom( const real amount )
{
  _zoom = amount;
  if ( _zoom < ZoomLimit ) {
    _zoom = ZoomLimit;
  }
  
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Camera::getName() const
{
  return _name;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::vec3 Camera::getPosition() const
{
  return _position;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::vec3 Camera::getTarget() const
{
  return _target;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::mat4 Camera::getViewMatrix()
{
  if ( _viewMatrixDirty ) {
    _updateViewMatrix();
  }

  return _view;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::trackNode( NodePtr node, const TrackingStyle& mode )
{
  _trackingNode = node;
  _trackingStyle = mode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::updateTracking()
{
  if ( !_trackingNode ) {
    return;
  }

  const glm::vec3 nodePos = _trackingNode->getPosition();

  switch ( _trackingStyle ) {
  default:
  case TrackingStyle::Simple:
    _position.x = nodePos.x;
    _position.y = nodePos.y;
    break;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::initPostProcessing( const u32 width, const u32 height, const u32 sampleCount )
{
  if ( postProcessing ) {
    Resource::DestroyRenderTarget( postProcessing->renderTarget );
    Resource::DestroyRenderTarget( postProcessing->doubleBuffer );
    Resource::DestroySprite( postProcessing->entity->_material->sprite );
    Resource::DestroySprite( postProcessing->doubleBufferEntity->_material->sprite );
    Resource::DestroyEntity( postProcessing->entity );
    Resource::DestroyEntity( postProcessing->doubleBufferEntity );
  }

  postProcessing = std::make_unique<PostProcessing>();

  postProcessing->renderTarget = Resource::CreatePostProcessingBuffer( _name + "_post_buffer", width, height, 0 ); // multisampling is fucking everything up
  postProcessing->doubleBuffer = Resource::CreateDoubleBuffer( _name + "_double_buffer", width, height, 0 );

  // We need an entity for rendering our fullscreen quad.
  postProcessing->entity = Resource::CreateEntity( _name + "_entity", Mesh::Type::FullscreenQuad );
  postProcessing->entity->_material->program = StockResource::GetGPUProgram( "PostProcessing" );

  // Create a sprite for our render target texture.
  auto sprite = Resource::CreateSprite( _name + "_post_sprite" );
  sprite->addTexture( Texture::Type::Diffuse, postProcessing->renderTarget->getTexture() );
  postProcessing->entity->_material->sprite = sprite;
  postProcessing->entity->_material->lighting = false;

  // Also create an entity for the double buffering.
  postProcessing->doubleBufferEntity = Resource::CreateEntity( _name + "_db_entity", Mesh::Type::FullscreenQuad );
  postProcessing->doubleBufferEntity->_material->program = StockResource::GetGPUProgram( "GaussianBlur" );

  auto dbSprite = Resource::CreateSprite( _name + "_post_db_sprite" );
  dbSprite->addTexture( Texture::Type::Diffuse, postProcessing->renderTarget->getTexture() );
  postProcessing->doubleBufferEntity->_material->sprite = dbSprite;
  postProcessing->doubleBufferEntity->_material->lighting = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::_dirty()
{
  _viewMatrixDirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::setPosition( const glm::vec3& pos )
{
  _position = pos / _zoom;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::translate( const glm::vec3& offset )
{
  _position += offset / _zoom;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::_updateViewMatrix()
{
  glm::vec3 scale( _zoom, _zoom, 1.f );
  _position.z = 0.f;
  _view = Math::CreateTransformationMatrix( _position,
                                            glm::quat( 1.f, 0.f, 0.f, 0.f ),
                                            scale );

  // Wtf? Have to invert x/y values...not sure why at the moment.
  _view[3][0] *= -_zoom;
  _view[3][1] *= -_zoom;
  // TODO: What to do with z view value?
  //_view[3][2] *= _zoom;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::setPosition( const glm::vec3& pos )
{
  _position = pos;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::translate( const glm::vec3& offset )
{
  _position += offset;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::pitch( const real amount )
{
  // Have to reverse direction here, possibly related to Camera2D::_updateViewMatrix() requiring reversal.
  _pitch -= amount;

  // Limit pitching.
  if ( _pitch > 89.f ) {
    _pitch = 89.f;
  } 
  else if ( _pitch < -89.f ) {
    _pitch = -89.f;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::yaw( const real amount )
{
  _yaw += amount;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::lookAt( const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up )
{
  _position = eye;
  _target = target;
  _up = up;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::_updateViewMatrix()
{
  // Update target vector.
  glm::vec3 target { 0.f };
  target.x = std::cosf( glm::radians( _yaw ) ) * std::cosf( glm::radians( _pitch ) );
  target.y = std::sinf( glm::radians( _pitch ) );
  target.z = std::sinf( glm::radians( _yaw ) ) * std::cosf( glm::radians( _pitch ) );
  _target = glm::normalize( target );

  _view = glm::lookAt( _position, _position + _target, _up );

  _viewMatrixDirty = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
